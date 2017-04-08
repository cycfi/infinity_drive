/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/dac.hpp>

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t pwm_freq = 84000000;
constexpr uint32_t pwm_resolution = 1024;
constexpr uint32_t sps = pwm_freq / pwm_resolution;

inf::timer<3> pwm(pwm_freq, pwm_resolution);

// our synthesizer
inf::pulse synth(440.0, 0.5, sps);

inf::low_pass lp1(600.0, sps);
inf::low_pass lp2(600.0, sps);

inf::dac dac;

void start()
{
   pwm.enable_interrupt();
   pwm.enable();

   while (true)
   {
   }
}

void irq(timer_task<3>)
{
   float val = synth();
   //val = lp2(lp1(val));

   // We generate a 12 bit signal
   dac((val * 2047) + 2048);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
