/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t pwm_freq = 84000000;
constexpr uint32_t pwm_resolution = 1024;
constexpr uint32_t sps = pwm_freq / pwm_resolution;

inf::timer<3> pwm(pwm_freq, pwm_resolution);

// our synthesizer
//inf::sin synth(1000.0, sps);
inf::pulse synth(1000.0, 0.5, sps);

void start()
{
   pwm.enable_channel<0, portc + 6>();
   pwm.width<0>(0);
   pwm.enable_interrupt();
   pwm.enable();

   while (true)
   {
   }
}

void irq(timer_task<3>)
{
   // We generate a 10 bit signal
   uint16_t val = (synth() * 511) + 512;
   pwm.width<0>(val);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
