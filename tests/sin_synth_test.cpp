/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>
#include <inf/dac.hpp>
#include <q/synth.hpp>
#include <q/fx.hpp>

///////////////////////////////////////////////////////////////////////////////
// This test generates two 1kHz sine waves, one shifted 90 degrees and outputs
// the signal using the dacs. This is done using a timer interrupt set to
// trigger at 100kHz kHz.
//
// Setup: connect pin PA4 and PA5 to an oscilloscope to see the generated
// waveforms.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
namespace q = cycfi::q;
using namespace inf::port;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 44100;

auto synth1 = q::sin(440.0, sps);
auto synth2 = q::sin(440.0, sps, q::pi / 2); // 90 degree shift

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac1;
inf::dac<1> dac2;
inf::timer<3> tmr;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   // We generate a 12 bit signals, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signals to 0.8.

   dac1((0.8f * synth1() * 2047) + 2048);
   dac2((0.8f * synth2() * 2047) + 2048);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac1.setup(),
   dac2.setup(),
   tmr.setup(tmr_freq, sps, timer_task)   // calls timer_task every 44.1kHz
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
