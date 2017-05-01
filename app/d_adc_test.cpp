/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/adc.hpp>
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;

using inf::portc;
using inf::delay_ms;

constexpr int sampling_rate = 16000;
inf::timer<8> clock(2000000, 2000000/sampling_rate);
inf::adc<1, 8> adc_conv(clock);
volatile int32_t adc_val = 0;

void start()
{
   // channel 11, portc pin 1
   adc_conv.enable_channel<11, portc + 1, 1>();

   adc_conv.start();
   clock.start();

   while (true)
   {
      delay_ms(500);
   }
}

inline void irq(adc_conversion_complete)
{
   int val = 0;
   for (int i = 0; i < 8; ++i)
      val += adc_conv[0][0];
   val /= 8;
   adc_val = val;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
