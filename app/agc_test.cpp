/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include "agc.hpp"
#include <inf/app.hpp>
#include <inf/support.hpp>

///////////////////////////////////////////////////////////////////////////////
// AGC test. Tests the AGC (automatic gain control).
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////
namespace inf = cycfi::infinity;

///////////////////////////////////////////////////////////////////////////////
// Our multi-processor
static constexpr auto clock = 64000;
static constexpr auto sps_div = 4;
static constexpr auto sps = clock / sps_div;

struct my_processor : inf::agc<sps>
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 8;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {      
      out[0] = (*this)(s);
      out[1] = s;
   }
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
