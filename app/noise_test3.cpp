/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>

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

template <std::uint32_t sps>
struct sustainer
{
   static constexpr float threshold = 0.01f;

   float operator()(float s)
   {
      // DC block
      s = _dc_block(s);

      // Envelope follower
      auto env = _env(s);

      // Noise Gate
      if (env < threshold)
         return 0;

      // Drive
      s = s * _level;

      // Soft Clip
      s = _clip(s);

      // Low pass
      s = _lp(s);

      return s * 0.8;
   }

   q::dc_block          _dc_block   { 0.1_Hz, sps };
   q::one_pole_lowpass  _lp         { 2637_Hz, sps };
   q::envelope_follower _env        { 50_ms, 100_ms, sps };
   q::soft_clip         _clip;
   float                _drive;
   float                _level;
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

      // if (channel == 0)
      // {
      //    out[0] += _p[channel](s);
      // }


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
         s._level = level * 20;
   }

   void update_drive(float drive)
   {
      for (auto& s : _p)
    	   ; // s._drive = drive;
   }

   std::array<sustainer<sps>, channels> _p;
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

      proc.update_level(ui.level());
      proc.update_drive(ui.drive());

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
