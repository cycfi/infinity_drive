/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/dac.hpp>

///////////////////////////////////////////////////////////////////////////////
// This test generates a 1kHz sine wave and outputs the signal using
// the dac. This is done using a timer interrupt set to trigger at
// 78125 kHz (80000000 / 1024).
//
// Setup: connect pin PA4 to an oscilloscope to see the generated sine wave.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 100000;

inf::sin synth(1000.0, sps);

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac;
inf::timer<3> tmr;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   // We generate a 12 bit signal, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit 
   // the signal to 0.9.
   uint16_t val = (0.9f * synth() * 2047) + 2048;
   dac(val);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac.setup(),
   tmr.setup(tmr_freq, sps, timer_task)   // calls timer_task every 100kHz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();
   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
