/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/control_acquisition.hpp>
#include "agc.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;

///////////////////////////////////////////////////////////////////////////////
static constexpr auto control_sps = 1000;
float gain = 0.0f;

struct infinity_controller
{
   static constexpr auto adc_id = 2;
   static constexpr auto timer_id = 3;
   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = control_sps;
   static constexpr auto buffer_size = 2;

   template <typename T>
   void process(std::array<T, channels> const& in)
   {
      lp(in[0] / 4096.0f);
   }

   void complete()
   {
      gain = lp();
   }

   template <typename Adc>
   void setup_channels(Adc& adc)
   {
      adc.template enable_channels<6>();
   }

   inf::one_pole_lp lp = {10.0f /*hz*/, control_sps};
};

inf::control_acquisition<infinity_controller> ctrl;

///////////////////////////////////////////////////////////////////////////////
static constexpr auto audio_clock = 64000;
static constexpr auto audio_sps_div = 4;
static constexpr auto audio_sps = audio_clock / audio_sps_div;

struct infinity_processor
{
   static constexpr auto oversampling = audio_sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
//   static constexpr auto channels = 6;
   static constexpr auto channels = 3;
//   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = audio_clock;
   static constexpr auto buffer_size = 8;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
	  auto r = agc[channel](s) * gain;

      out[0] += r;
      //out[1] += r;
      if (channel == 0)
      {
         out[1] += agc[channel]._env_follow();
         op = agc[channel]._noise_gate();
      }
      //out[1] = -1;
   }

   template <typename Adc>
   void setup_channels(Adc& adc)
   {
//      adc.template enable_channels<0, 12, 13, 3, 10, 11>();
      adc.template enable_channels<0, 12, 13>();
//      adc.template enable_channels<3, 10, 11>();
   }

   inf::agc<audio_sps> agc[channels];
   inf::output_pin<inf::port::portg + 15> op;
};

inf::multi_channel_processor<inf::processor<infinity_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
void start()
{
   ctrl.start();
   proc.start();
   while (true)
      inf::delay_ms(500);
}

///////////////////////////////////////////////////////////////////////////////
// Interrupts
///////////////////////////////////////////////////////////////////////////////
INF_CONTROLLER_IRQ(2, ctrl)      // Controller Interrupts
INF_PROCESSOR_IRQ(1, 2, proc)    // Processor Interrupts

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
