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
#include "pls.hpp"

///////////////////////////////////////////////////////////////////////////////
// PLL test
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
namespace q = cycfi::q;
using namespace inf::port;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 20000;

// auto comp = q::pi/3.2;
auto synth = q::sin(440.0, sps, 0);
auto ref_synth = q::sin(440.05, sps, 0);

using pls_type = inf::pls<decltype(ref_synth), sps, 1000>;
pls_type pls{ref_synth, q::phase::angle(q::pi/2)};

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

   auto val = synth();
   dac1((0.8f * val * 2047) + 2048);
   dac2((0.8f * pls(val) * 2047) + 2048);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac1.setup(),
   dac2.setup(),
   tmr.setup(tmr_freq, sps, timer_task)   // calls timer_task every 100kHz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();

   while (true)
   {
      // auto freq = q::osc_freq(400, sps);
      // auto incr = freq / 1000;
      // synth.freq(freq);

      // for (int i = 0; i < 1000; ++i)
      // {
      //    synth.freq(synth.freq() + incr);
      //    inf::delay_ms(10);
      // }

   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
