/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/adc.hpp>

// ADC and PWM test. We generate a signal and send it through portc pin 6
// as PWM. The filtered PWM signal is then sent to the ADC channel 11,
// portc pin 1. This signal is then sent down as PWM to portc pin 7.

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t pwm_freq = 84000000;
constexpr uint32_t pwm_resolution = 1024;
constexpr uint32_t sps = pwm_freq / pwm_resolution;
constexpr uint32_t adc_sampling_rate = 32000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<3> pwm(pwm_freq, pwm_resolution);
inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/adc_sampling_rate);

inf::adc<1, 4> adc_conv(adc_clock);

constexpr uint32_t adc_sps = adc_sampling_rate/4; // 4x oversampling

inf::low_pass lp(350, adc_sps);
inf::low_pass lp2(175, adc_sps);
inf::period_detector pd;

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
   float val = adc_conv[0] + adc_conv[1] + adc_conv[2] + adc_conv[3];
   val = (val / 8190.0f) - 1.0f; // 8190 = (4095*4)/2
                                 // range: -1.0 to 1.0
   val = lp2(pd(lp(val)));

   // We generate a 10 bit signal
   pwm.width<0>((val * 511) + 512);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
