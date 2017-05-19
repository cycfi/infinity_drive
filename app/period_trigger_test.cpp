/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "processor_test.hpp"
#include <inf/period_trigger.hpp>

///////////////////////////////////////////////////////////////////////////////
// Period Trigger test. Generates square pulses that correspond the period
// of a waveform.
//
// Setup: Connect an input signal (e.g. signal gen) to pin PC0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;

struct my_processor
{
   my_processor()
    : _pt()
    , _index(0)
    , _val(0.0f)
   {}
   
   float process(float val)
   {
//      if (++_index & 0x3)
//         _val += val;
//      else
//         return _pt(_val / 4.0f);
      
      return _pt(val);
   }
   
   int _index;
   float _val;
   inf::period_trigger<32000> _pt;
};

inf::mono_processor<my_processor> proc;
inf::output_pin<inf::port::portc + 3> pin; // portc, pin 3

void start()
{
   proc.start();
   while (true)
      inf::delay_ms(500);
}

inline void irq(adc_conversion_half_complete<1>)
{
   pin = 1;
   proc.irq_conversion_half_complete();
   pin = 0;
}

inline void irq(adc_conversion_complete<1>)
{
   pin = 1;
   proc.irq_conversion_complete();
   pin = 0;
}

void irq(timer_task<2>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
