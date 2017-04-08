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
constexpr uint32_t adc_sampling_rate = 40000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<3> pwm(pwm_freq, pwm_resolution);
inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/adc_sampling_rate);

inf::adc<1, 1> adc_conv(adc_clock);

// our synthesizer
inf::sin s1(220.0, sps);
inf::sin s2(440.0, sps);
inf::sin s3(660.0, sps);

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

   // PWM channel 1, portc pin 7
   pwm.enable_channel<1, portc + 7>();
   pwm.width<1>(0);

   pwm.enable_interrupt();
   pwm.enable();

   while (true)
   {
   }
}

inline void irq(adc_conversion_complete)
{
   // We generate a 10 bit signal
   pwm.width<1>(adc_conv[0][0] / 4); // adc is 12 bits
}

void irq(timer_task<3>)
{
   constexpr inf::gain g1{0.5};
   constexpr inf::gain g2{0.3};
   constexpr inf::gain g3{0.15};
   auto val = g1(s1()) + g2(s2()) + g3(s3());

   // We generate a 10 bit signal
   pwm.width<0>((val * 511) + 512);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
