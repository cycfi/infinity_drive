/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/adc.hpp>
#include <inf/dac.hpp>
#include <inf/period_detector.hpp>

#include "diag/Trace.h"

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t adc_sps = 32000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/adc_sps);

inf::adc<1, 1, 1> adc_conv(adc_clock);
inf::dac dac;

void start()
{
   // ADC channel 11, portc pin 1
   adc_conv.enable_channel<11, portc + 1, 1>();

   adc_conv.enable();
   adc_clock.enable();
   adc_conv.enable_interrupt();

   while (true)
   {
   }
}

inf::period_trigger<adc_sps> pd;

inf::window_comparator wc;

inline void irq(adc_conversion_complete)
{
   float val = (adc_conv[0][0] / 2048.0f) - 1.0f;

   val = pd(val);

   // We generate a 12 bit signal
   dac((val * 2047) + 2048);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
