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

   my_processor()
   {
      _sustainers[0].gain(0.9f);    // E
      _sustainers[1].gain(0.9f);    // A
      _sustainers[2].gain(0.8f);    // D
      _sustainers[3].gain(1.1f);    // G
      _sustainers[4].gain(1.25f);   // B
      _sustainers[5].gain(1.6f);    // E

      _sustainers[0].cutoff(659);   // E
      _sustainers[1].cutoff(880);   // A
      _sustainers[2].cutoff(1174);  // D
      _sustainers[3].cutoff(1568);  // G
      _sustainers[4].cutoff(1975);  // B
      _sustainers[5].cutoff(2637);  // E
   }

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      // // Disable the sustainer if sustain level is zero.
      // _sustainers[channel].enable(ui.level() != 0.0f);

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
         out0 = _lim(out0, _env[0]());
         out1 = _lim(out1, _env[1]());

         // Soft-clip the final result and limit the output to 0.8
         // of the value so the DAC won't clip (The STM32F4 DAC has
         // a built-in buffer that is not rail-to-rail).
         static constexpr q::soft_clip clip;
         out[0] = clip(out0 * 1.25f) * 0.8f;
         out[1] = clip(out1 * 1.25f) * 0.8f;
      }
   }

   void update_level(float level)
   {
      for (auto& s : _sustainers)
         s.update_level(ui.level());
   }

   using sustainer_type = inf::sustainer<sps>;
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
