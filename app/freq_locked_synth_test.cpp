/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>
#include <q/synth.hpp>
#include "freq_locked_synth.hpp"

///////////////////////////////////////////////////////////////////////////////
// Frequency-Locked Synthesizer test
//
// The freq_locked_synth looks at the input audio and extracts the
// fundamental frequency and phase from the waveform and uses these
// information to set the frequency and phase of a sine-wave synthesiser.
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pins PA4 and PA5 to an oscilloscope to see the input and output waveforms.
///////////////////////////////////////////////////////////////////////////////
namespace inf = cycfi::infinity;
namespace q = cycfi::q;

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
   static constexpr auto start_phase = q::osc_phase(q::pi / 2);

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      auto synth_out = _fls(s, _ticks++);

      out[0] = s;
      out[1] = synth_out * 0.8;  // don't let it saturate
   }

   using sin_synth = decltype(q::sin(1.0, sps, 0.0));
   using fls_type = inf::freq_locked_synth<sin_synth, sps, latency>;

   sin_synth   _synth = q::sin(0.0, sps, 0.0);
   fls_type    _fls = {_synth, start_phase};
   uint32_t    _ticks = 0;
};

inf::multi_channel_processor<inf::processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
      proc.config<0>()
   );

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   proc.start();
   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
