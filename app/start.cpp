/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>

#include "ui.hpp"
#include <array>

///////////////////////////////////////////////////////////////////////////////
// Driver
///////////////////////////////////////////////////////////////////////////////
#include <q/fx.hpp>

float gfreq = 440.0;

namespace cycfi { namespace infinity
{
   using namespace q::literals;

   template <std::uint32_t N, typename T = std::uint32_t>
   struct bitset
   {
      static_assert(q::is_pow2(N), "N must be a power of 2, except 0");
      static constexpr auto array_size = sizeof(T) / N;

      void set(std::uint32_t i, bool val)
      {
         auto index = i * sizeof(T);
         auto* p = bits + index;
         auto mask = i & (sizeof(T) - 1);
         *p |= 1 << mask;
      }

      static std::uint32_t count_bits(std::uint32_t i)
      {
         // GCC only!!!
         return __builtin_popcount(i);
      }

      static std::uint64_t count_bits(std::uint64_t i)
      {
         // GCC only!!!
         return __builtin_popcountll(i);
      }

      std::uint32_t auto_correlate(int pos)
      {
         auto index = pos * sizeof(T);
         auto* p1 = bits;
         auto* p2 = bits + index;
         auto shift = N % pos;
         auto count = 0;

         if (shift == 0)
         {
            for (auto i = 0; i != array_size / 2; ++i)
               count += count_bits(*p1++ ^ *p2++);
         }
         else
         {
            auto shift2 = sizeof(T) - shift;
            for (auto i = 0; i != array_size / 2; ++i)
            {
               auto v = *p2++ >> shift;
               v |= *p2 << shift2;
               count += count_bits(*p1++ ^ v);
            }
         }
      }

      void auto_correlate(std::array<std::uint32_t, N / 2>& out)
      {
         for (auto i = 0; i != N / 2; ++i)
            out[i] = auto_correlate(i);
      }

      T bits[array_size];
   };

   template <std::uint32_t sps, std::uint32_t buffer_size>
   struct driver
   {
      static constexpr float threshold = 0.02f;

      float operator()(float s)
      {
         // DC block
         s = _dc_block(s);

         // Envelope follower
         auto env = _env(s);

         // Noise Gate
         if (env < threshold)
            return 0;

         // Normalize
         s = s * q::fast_inverse(env);

         // Peak Trigger
         bool peak = _trig(s);

         // Collect peaks and process when bitset is full
         if (_index == buffer_size)
         {
            auto_correlate();
            _index = 0;
         }
         _peaks.set(_index++, peak);

         s = peak;
         return s * 0.8;
      }

      // binary auto correlation
      void auto_correlate()
      {
      }

      q::dc_block                _dc_block   { 0.1_Hz, sps };
      q::peak_envelope_follower  _env        { 100_ms, sps };
      q::window_comparator       _trig       { -0.1, 0.1 };
      float                      _drive;
      float                      _level;

      bitset<buffer_size>        _peaks;
      std::uint32_t              _index      = 0;
   };
}}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace inf = cycfi::infinity;
namespace q = cycfi::q;
using namespace q::literals;
using inf::delay_ms;

///////////////////////////////////////////////////////////////////////////////
// Our UI
inf::ui ui;

///////////////////////////////////////////////////////////////////////////////
// Our multi-processor
static constexpr auto clock = 80000;
static constexpr auto sps_div = 4;
static constexpr auto sps = clock / sps_div;

struct my_processor
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 6;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 2048;
   static constexpr auto latency = buffer_size / sps_div;
   static constexpr auto headroom = 2.0f;

   my_processor()
   {
   }

   std::uint32_t downsample(std::uint32_t s, std::uint32_t i)
   {
      // FIR coefficients
      static constexpr float coeffs[] = {
         0.12744100383454346,
         0.38185474794932256,
         0.38185474794932256,
         0.12744100383454346
      };

      return s * coeffs[i];
   }

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      if (channel == 0)
      {
         out[0] = _driver[channel](s);
         out[1] = s;
      }
   }

   void update_level(float level)
   {
      for (auto& s : _driver)
         s._level = level * 20;
   }

   void update_drive(float drive)
   {
      for (auto& s : _driver)
    	   s._drive = drive;
   }

   std::array<inf::driver<sps, buffer_size>, channels> _driver;
};

inf::multi_channel_processor<inf::oversampling_processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   ui.setup(),
   proc.config<0, 1, 2, 10, 11, 12>()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   proc.start();
   ui.start();

   while (true)
   {
      // ui.refresh();

      // proc.update_level(ui.level());
      // proc.update_drive(ui.drive());

      ui.display("Freq:", gfreq * 10, 1);

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
