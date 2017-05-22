/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "processor_test.hpp"
#include <inf/fx.hpp>

///////////////////////////////////////////////////////////////////////////////
// AGC test. Tests the AGC (automatic gain control).
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
//static constexpr auto sps_div = 4;

struct my_processor
{  
   float process(float val)
   {
      using inf::exp;
      using inf::_2pi;
      using inf::linear_interpolate;
      static constexpr auto sps = 32000;
      
      // DC block
      val = _dc_blk(val);

      // Automatic filter control
      static constexpr float fl = 1.0f - exp(-_2pi * 100/sps);
      static constexpr float f2 = 1.0f - exp(-_2pi * 16000/sps);
      lp.a = {linear_interpolate(fl, f2, _ef())};
      val = lp(val);

      // Update the envelope follower
      auto env = _ef(std::abs(val));

      // Automatic gain control
      inf::gain g = {1.0f / std::max(env, 0.0001f)};
      val = g(val);
      
//      static constexpr float noise_threshold = 0.001f;
//      if (env < noise_threshold)
//      {
//         // downward expander
//         inf::gain g = {1000.0f * (env / noise_threshold)};
//         val = g(val);
//      }

//      // Noise gate
//      constexpr inf::noise_gate ng = {0.001f};
//      val = ng(val, env);
      
      if (noise_gate(0.001f, env))
         return 0.0f;
      
      return val;
   }
   
   inf::schmitt_trigger noise_gate = {0.001};
   inf::envelope_follower _ef = {0.0001f};
   inf::one_pole_lp lp = {0.0f};
   inf::dc_block _dc_blk;
};

inf::mono_processor<inf::processor<my_processor, 2048>> proc;

void start()
{
   proc.start();
   while (true)
      inf::delay_ms(500);
}

inline void irq(adc_conversion_half_complete<1>)
{
   proc.irq_conversion_half_complete();
}

inline void irq(adc_conversion_complete<1>)
{
   proc.irq_conversion_complete();
}

void irq(timer_task<2>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
