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
constexpr std::uint32_t sps = 20000;

auto synth = q::sin(110.0, sps);
auto ref_synth = q::sin(150.0, sps, q::pi/4);

using pls_type = inf::pls<decltype(ref_synth), sps, 100>;
pls_type pls{ ref_synth };

template <typename Synth>
struct synth_gate
{
   enum { start, sustain, release, stop };

   synth_gate(Synth& synth)
    : _synth(synth) {}

   float operator()()
   {
      if (state == start)
      {
         _synth.phase(0);
         state = sustain;
      }
      else if (state == release)
      {
         if (_synth.is_start())
            state = stop;
      }
      bool enable = (state == sustain || state == release);
      return enable? _synth() : 0.0f;
   };

   void note_on()
   {
      if (state == stop)
         state = start;
   }

   void note_off()
   {
      if (state == sustain)
         state = release;
   }

   Synth& _synth;
   int state = stop;
};

synth_gate<decltype(synth)> sgate{ synth };

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

   static std::uint32_t sample_clock = 0;

   auto val = sgate();
   dac1((0.8f * val * 2047) + 2048);
   dac2((0.8f * pls(val, sample_clock++) * 2047) + 2048);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr std::uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac1.setup(),
   dac2.setup(),
   tmr.setup(tmr_freq, sps, timer_task)   // calls timer_task every 20kHz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();

   // auto start = q::phase::freq(1000, sps);
   auto start = q::phase::freq(55, sps);
   auto stop = q::phase::freq(1600, sps);
   auto constexpr step = 1.059463094359295f;
   bool continuous = false;

   // synth.freq(q::phase::freq(174.61411571650107, sps));
   // synth.freq(start);
   // sgate.note_on();
   // while (true)
   //    ;

   while (true)
   {
      sgate.note_off();
      inf::delay_ms(300);
      sgate.note_on();
      synth.freq(start);
      while (synth.freq() < stop)
      {
         synth.freq(synth.freq() * step);
         inf::delay_ms(2000);
         if (!continuous)
         {
            sgate.note_off();
            inf::delay_ms(300);
            sgate.note_on();
         }
      }
      continuous = !continuous;
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
