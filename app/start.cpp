/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>
#include <inf/pid.hpp>
#include <q/synth.hpp>

#include "sustainer.hpp"
#include "ui.hpp"

///////////////////////////////////////////////////////////////////////////////
// Frequency-Locked Synthesizer sustain test with PID
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pins PA4 and PA5 to an oscilloscope to see the input and output waveforms.
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
   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 1024;
   static constexpr auto latency = buffer_size / sps_div;
   static constexpr auto start_phase = q::osc_phase(0);

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      auto synth_out = _sustainer(s, _sample_clock++);

      out[0] = synth_out * _level;
      out[1] = s;
   }

   using sustainer_type = inf::sustainer<sps, latency>;

   sustainer_type _sustainer;
   uint32_t       _sample_clock = 0;
   float          _level = 0;
};

inf::multi_channel_processor<inf::processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
struct level_pid_config
{
   float static constexpr p = 0.05f;         // Proportional gain
   float static constexpr i = 0.0f;          // Integral gain
   float static constexpr d = 0.03f;         // derivative gain
   float static constexpr sps = 100.0f;      // 100 Hz
};

inf::pid<level_pid_config> level_pid;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   ui.setup(),
   proc.config<0>()
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

      // Set the sustain level
      proc._level += level_pid(ui.level(), proc._sustainer.envelope());
      if (proc._level > 1.0f)
         proc._level = 1.0f;
      else if (proc._level < 0.0f)
         proc._level = 0.0f;

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
