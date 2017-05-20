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

struct my_processor
{  
   float process(float val)
   {
      constexpr inf::agc agc = {1000, 1.0f};
      constexpr inf::noise_gate ng = {0.03f};
      
      auto env = _ef(std::abs(val));
      return ng(agc(val, env), env);
   }
   
   inf::envelope_follower _ef = {0.0001f};
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
