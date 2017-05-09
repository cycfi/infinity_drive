/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

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
// 78125 kHz (80000000 / 2014).
//
// Setup: connect pin PA4 to an oscilloscope to see the generated sine wave.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

constexpr uint32_t tmr_freq = 80000000;
constexpr uint32_t tmr_resolution = 1024;
constexpr uint32_t sps = tmr_freq / tmr_resolution;

// our synthesizer
inf::sin synth(1000.0, sps);

// The DAC
inf::dac<0> dac;

void start()
{
   inf::timer<3> tmr(tmr_freq, tmr_resolution);
   tmr.enable_interrupt();
   tmr.start();

   while (true)
   {
   }
}

void irq(timer_task<3>)
{
   // We generate a 12 bit signal
   uint16_t val = (synth() * 2047) + 2048;
   dac(val);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
