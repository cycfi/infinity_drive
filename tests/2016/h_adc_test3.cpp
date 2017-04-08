/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/adc.hpp>

// ADC and PWM test. We read the signal from ADC channel 11 portc pin 1
// and send it out as PWM through portc pin 6.

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t pwm_freq = 84000000;
constexpr uint32_t pwm_resolution = 1024;
constexpr uint32_t sps = pwm_freq / pwm_resolution;
constexpr uint32_t adc_sampling_rate = 32000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<3> pwm(pwm_freq, pwm_resolution);
inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/adc_sampling_rate);

inf::adc<1, 1> adc_conv(adc_clock);

void start()
{
   // ADC channel 11, portc pin 1
   adc_conv.enable_channel<11, portc + 1, 1>();

   adc_conv.enable();
   adc_clock.enable();
   adc_conv.enable_interrupt();

   // PWM channel 0, portc pin 6
   pwm.enable_channel<0, portc + 6>();
   pwm.width<0>(0);
   pwm.enable();

   while (true)
   {
   }
}

inline void irq(adc_conversion_complete)
{
   // We generate a 10 bit signal
   pwm.width<0>(adc_conv[0][0] / 4); // adc is 12 bits
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
