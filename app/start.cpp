/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>
#include <q/synth.hpp>

#include "sustainer.hpp"
#include "ui.hpp"

#include <array>

///////////////////////////////////////////////////////////////////////////////
// Phase-Locked Synthesizer sustain test with PID
//
// Setup: Connect input signals to ADC channels 0, 1 and 2. Connect
// pins PA4 (DAC out) to the sustain driver.
///////////////////////////////////////////////////////////////////////////////
namespace inf = cycfi::infinity;
namespace q = cycfi::q;
using inf::delay_ms;

///////////////////////////////////////////////////////////////////////////////
// Our UI
inf::ui ui;

///////////////////////////////////////////////////////////////////////////////
// Our multi-processor
static constexpr auto clock = 80000;
static constexpr auto sps_div = 4;
static constexpr auto sps = clock / sps_div;

struct my_processor
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 6;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 8;
   static constexpr auto latency = buffer_size / sps_div;
   static constexpr auto headroom = 2.0f;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      if (channel < 3)
         out[0] += _sustainers[channel](s);
      else
         out[1] += _sustainers[channel](s);

      if (channel == channels-1)
      {
         auto out0 = out[0];
         auto out1 = out[1];

         // Update the output envelope followers with
         // some headroom for the limiters.
         _env[0](out0 * headroom);
         _env[1](out1 * headroom);

         // Limit the outputs to 1.0f
         out[0] = _lim(out0, _env[0]());
         out[1] = _lim(out1, _env[1]());
      }
   }

   void update_level(float level)
   {
      for (auto& s : _sustainers)
         s.update_level(ui.level());
   }

   using sustainer_type = inf::sustainer<sps, latency>;
   using sustainer_array_type = std::array<sustainer_type, channels>;

   sustainer_array_type                _sustainers;
   std::array<q::envelope_follower, 2> _env;
   q::hard_limiter                     _lim;
};

inf::multi_channel_processor<inf::processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   ui.setup(),
   proc.config<0, 1, 2, 10, 11, 12>()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   proc.start();
   ui.start();

   while (true)
   {
      ui.refresh();

      // Update the sustain level
      proc.update_level(ui.level());
      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
