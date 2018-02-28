/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>
#include <q/delay.hpp>

#include "sustainer.hpp"
#include "ui.hpp"

#include <array>

///////////////////////////////////////////////////////////////////////////////
// Noise test
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

namespace cycfi { namespace q
{
   struct smoother2
   {
      smoother2(float threshold_, frequency f1, frequency f2, std::uint32_t sps)
       : threshold(threshold_)
       , a1(1.0f - std::exp(-2_pi * double(f1) / sps))
       , a2(1.0f - std::exp(-2_pi * double(f2) / sps))
      {}

      float operator()(float s)
      {
         auto dy = s - y;
         // if (dy < threshold)
         //    return y += a1 * dy;
         // else
            return y += a2 * dy;
      }

      float operator()() const
      {
         return y;
      }

      smoother2& operator=(float y_)
      {
         y = y_;
         return *this;
      }

      void config(float threshold_, frequency f1, frequency f2, std::uint32_t sps)
      {
         threshold = threshold_;
         a1 = 1.0f - std::exp(-2_pi * double(f1) / sps);
         a2 = 1.0f - std::exp(-2_pi * double(f2) / sps);
      }

      float y = 0.0f, threshold, a1, a2;
   };

   struct smoother
   {
      smoother(frequency min, frequency max, frequency dfreq, std::uint32_t sps)
       : _lp(double(max), sps)
       , _dlp(double(dfreq), sps)
       , _min_a(1.0f -(2_pi * double(min) / sps))
       , _max_a(1.0f -(2_pi * double(max) / sps))
      {}

      float operator()(float s)
      {
         float dy = s - _lp();
         float dlp = _dlp(std::abs(dy));
         _lp.cutoff(linear_interpolate(_min_a, _max_a, dlp));
         return _lp(s);
      }

      one_pole_lowpass _lp;
      one_pole_lowpass _dlp;
      float _min_a, _max_a;
   };

   struct dynamic_smoother
   {
      dynamic_smoother(frequency base, std::uint32_t sps)
       : dynamic_smoother(base, 0.5, sps)
      {}

      dynamic_smoother(frequency base, float sensitivity, std::uint32_t sps)
       : sense(sensitivity * 4.0f)  // efficient linear cutoff mapping
       , wc(double(base) / sps)
      {
         auto gc = std::tan(pi * wc);
         g0 = 2.0f * gc / (1.0f + gc);
      }

      float operator()(float s)
      {
         auto lowlz = low1;
         auto low2z = low2;
         auto bandz = lowlz - low2z;
         auto g = std::min(g0 + sense * std::abs(bandz), 1.0f);
         low1 = lowlz + g * (s - lowlz);
         low2 = low2z + g * (low1 - low2z);
         return low2z;
      }

      void base_frequency(frequency base)
      {
         wc = double(base) / sps;
         auto gc = std::tan(pi * wc);
         g0 = 2.0f * gc / (1.0f + gc);
      }

      float sense, wc, g0;
      float low1 = 0.0f;
      float low2 = 0.0f;
   };
}}

template <std::uint32_t sps>
struct fx
{
   static constexpr float low_threshold = 0.5f;
   static constexpr float high_threshold = 0.6f;

   float operator()(float s)
   {
      // DC block
      s = _dc_block(s);

      // Smooth
      // s = _sm(s);
      // s = _sm2(s);
      s = _sm3(s);

      // Envelope follower
      auto env = _env(s);



      // // Low pass
      // s = _lp(s);


      // Noise Gate
      // if (env < 0.01)
      //    return 0;

      // Delay
      // s = _delay(s);



      // Drive
      s = s * _drive;

      // Soft Clip
      static constexpr q::clip clip;
      s = clip(s);

      return s;
   }

   // q::smoother2         _sm3 { 0.01, 300_Hz, 1000_Hz, sps };
   q::smoother2         _sm3 { 0.005, 500_Hz, 2637_Hz, sps };

   q::single_delay<float>  _delay { 1024u, 0u };

   q::dc_block          _dc_block;
   // q::one_pole_lowpass  _lp{ 4_kHz, sps };

   q::lowpass           _lp{ 4_kHz, sps, 0.707 };

   q::envelope_follower _env{ 50_ms, 100_ms, sps };
   q::window_comparator _noise_gate { low_threshold, high_threshold };
   float                _drive;
};

struct my_processor
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 6;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 8;
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
      if (channel == 5)
      {
         out[1] += _p[channel](s);
      }


      // if (channel == 3)
      // {
      //    out[1] = _p[channel](s) * _level * 20;
      //    out[0] = _p[channel]._env();
      // }

      // if (channel < 3)
      // {
      //    // out[0] += 0; // _p[channel](s) * _level * 10;
      // }
      // else
      // {
      //    out[1] += _p[channel](s) / 3;
      //    // if (channel == 3)
      //    //    out[0] = _p[channel]._env();
      // }

      // if (channel == channels-1)
      // {
      //    out[0] = 0; //_lp1(out[0]);
      //    out[1] = _lp2(out[1]);
      // }
   }

   void update_level(float level)
   {
      for (auto& s : _p)
         s._drive = level * 20;
   }

   void update_cutoff(float cutoff)
   {
      for (auto& s : _p)
         s._lp.config(cutoff, sps, 0.707);
         // s._sm.base_frequency(cutoff);
         //s._lp.cutoff(cutoff, sps);
   }

   void update_delay(float delay)
   {
      for (auto& s : _p)
         s._delay.samples_delay(delay);
   }

   // q::lowpass _lp1{4_kHz, sps, 0.707};
   // q::lowpass _lp2{4_kHz, sps, 0.707};

   std::array<fx<sps>, channels> _p;
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
      ui.refresh();

      // Update the sustain level
      proc.update_level(ui.level());

      proc.update_cutoff(ui.cutoff());

      proc.update_delay(ui.delay());

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
