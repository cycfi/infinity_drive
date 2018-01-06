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
   static constexpr auto channels = 3;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 1024;
   static constexpr auto latency = buffer_size / sps_div;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      out[0] += _sustainers[channel](s, _sample_clock);
      out[1] += s;

      if (channel == channels-1)
         ++_sample_clock;
   }

   void update_level(float level)
   {
      constexpr float max = 1.0f / channels;
      for (auto& s : _sustainers)
         s.update_level(ui.level(), max);
   }

   using sustainer_type = inf::sustainer<sps, latency>;
   using sustainer_array_type = std::array<sustainer_type, channels>;

   sustainer_array_type _sustainers;
   uint32_t             _sample_clock = 0;
};

inf::multi_channel_processor<inf::processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   ui.setup(),
   proc.config<0, 1, 2>()
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
