/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/adc.hpp>
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::delay_ms;

constexpr int sampling_rate = 16000;
inf::timer<2> clock(2000000, 2000000/sampling_rate);
inf::adc<1, 2> adc_conv(clock);
volatile int32_t adc_val = 0;

void start()
{
   // channel 11, porta pin 4
   adc_conv.enable_channel<9, porta + 4, 1>();

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
