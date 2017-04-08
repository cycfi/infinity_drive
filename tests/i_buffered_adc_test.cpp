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

namespace inf = cycfi::infinity;
using inf::portc;

// ADC and PWM test. We read the signal from ADC channel 11 portc pin 1
// and send it out through the DAC porta pin 5. The ADC is buffered.

constexpr uint32_t adc_sps = 32000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/(adc_sps/2));
inf::timer<3> dac_clock(adc_clock_rate, adc_clock_rate/adc_sps);

constexpr unsigned buffer_size = 1024;

typedef inf::adc<1, 1, buffer_size> adc_type;
adc_type ad_conv(adc_clock);

inf::dac dac;
adc_type::buffer_iterator_type out = 0;
uint32_t count = 0;

void start()
{
   // ADC channel 11, portc pin 1
   ad_conv.enable_channel<11, portc + 1, 1>();

   ad_conv.enable();
   adc_clock.enable();
   ad_conv.enable_interrupt();

   dac_clock.enable();
   dac_clock.enable_interrupt();

   while (true)
   {
   }
}

inline void irq(adc_conversion_half_complete)
{
   out = ad_conv.begin();
   count = 0;
}

inline void irq(adc_conversion_complete)
{
   out = ad_conv.middle();
   count = 0;
}

void irq(timer_task<3>)
{
   if (out && count++ < (buffer_size/2))
   {
      // We generate a 12 bit signal
      dac((*out++)[0]);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
