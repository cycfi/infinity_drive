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
bool note_on = true;

auto synth = q::sin(110.0, sps);
auto ref_synth = q::sin(150.0, sps, q::pi/4);

using pls_type = inf::pls<decltype(ref_synth), sps, 100>;
pls_type pls{ ref_synth };

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac1;
inf::dac<1> dac2;
inf::timer<3> tmr;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   // We generate 12 bit signals, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signals to 0.8.

   static uint32_t sample_clock = 0;
   auto val = note_on? synth() : 0.0f;
   dac1((0.8f * val * 2047) + 2048);
   dac2((0.8f * pls(val, sample_clock++) * 2047) + 2048);
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
   inf::delay_ms(3000);
   auto start = q::phase::freq(80.0, sps);
   auto stop = q::phase::freq(1600.0, sps);
   auto constexpr step = 1.059463094359295f;
;
   while (true)
   {
      note_on = false;
      inf::delay_ms(500);
      note_on = true;
      synth.freq(start);
      while (synth.freq() < stop)
      {
         synth.freq(synth.freq() * step);
         inf::delay_ms(1000);
         note_on = false;
         inf::delay_ms(500);
         note_on = true;
      }
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
