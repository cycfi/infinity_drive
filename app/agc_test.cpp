/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include "processor_test.hpp"
#include <inf/fx.hpp>

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
    : _agc(1000, 0.01f, 1.0f, 0.0001f)
   {}
   
   float process(float val)
   {
      return _agc(val);
   }
   
   inf::agc _agc;
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
