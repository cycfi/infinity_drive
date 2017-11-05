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
// This test generates an 440Hz sine wave FM modulated by a 110Hz sine wave
// (mfactor = 4.0). A reference sine wave is also generated to sync the
// scope with.
//
// Setup: connect pin PA4 and PA5 to an oscilloscope to see the generated
// waveforms.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
namespace q = cycfi::q;
using namespace inf::port;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 40000;

auto synth1 = q::fm(440.0, 0.2, 4.0, sps);
auto synth2 = q::sin(440.0, sps);

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac1;
inf::dac<1> dac2;
inf::timer<3> tmr;
inf::output_pin<portc + 12> tpin;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   // We generate a 12 bit signals, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signals to 0.8.

   tpin = 1;
   dac1((0.8f * synth1() * 2047) + 2048);
   tpin = 0;
   dac2((0.8f * synth2() * 2047) + 2048);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   tpin.setup(),
   dac1.setup(),
   dac2.setup(),
   tmr.setup(tmr_freq, sps, timer_task)   // calls timer_task every 40kHz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();
   while (true)
   {
      constexpr q::phase_t incr{0.001};
      constexpr q::phase_t max{0.5};
      constexpr q::phase_t zero{0.0};

      auto g = synth1.mgain() + incr;
      if (g >= max)
         g = zero;
      synth1.mgain(g);
      inf::delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
