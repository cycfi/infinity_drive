/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/adc.hpp>
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Test the adc. 
// 
// Setup: Connect a 3.3v voltage divider using a potentiometer to pin   
//        PA4. Set a breakpoint at the adc_val = val; statement in the 
//        irq(adc_conversion_complete<1>) function. See the value change 
//        when you move the pot.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::delay_ms;

constexpr int sampling_rate = 16000;
inf::timer<2> clock(2000000, sampling_rate);
inf::adc<1, 2> adc_conv(clock);
volatile int32_t adc_val = 0;

void start()
{
   // channel 4, porta pin 4
   adc_conv.enable_channel<4, porta + 4, 1>();

   adc_conv.start();
   clock.start();

   while (true)
   {
      delay_ms(500);
   }
}

inline void irq(adc_conversion_complete<1>)
{
   int val = 0;
   for (int i = 0; i < 8; ++i)
      val += adc_conv[0][0];
   val /= 8;
   adc_val = val;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
