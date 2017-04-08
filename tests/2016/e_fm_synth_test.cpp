/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;

using inf::fxp;
using inf::osc_freq;
using inf::portc;

inf::timer<3> pwm(84000000, 255);
inf::timer<4> sample_clock(8000000, 256);
constexpr uint32_t sps = 8000000 / 256;

// our synthesizer
inf::fm synth(440.0, 0.2, 110.0, sps);

void start()
{
   pwm.enable_channel<0, portc + 6>();
   pwm.width<0>(0);
   pwm.enable();

   sample_clock.enable_interrupt();
   sample_clock.enable();

   while (true)
   {
   }
}

void irq(timer_task<4>)
{
   // We generate an 8 bit signal
   uint16_t val = (synth() * 127) + 128;
   pwm.width<0>(val);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
