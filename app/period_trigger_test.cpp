/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "processor_test.hpp"
#include <inf/period_trigger.hpp>

///////////////////////////////////////////////////////////////////////////////
// AGC test. Tests the AGC (automatic gain control).
//
// Setup: Connect an input signal (e.g. signal gen) to pin PC0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;

struct my_processor
{
   my_processor()
    : _pt()
   {}
   
   float process(float val)
   {
      return _pt(val);
   }
   
   inf::period_trigger<32000> _pt;
};

inf::mono_processor<my_processor> proc;

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

void irq(timer_task<1>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
