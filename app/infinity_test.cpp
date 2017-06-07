/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/control_acquisition.hpp>
#include <inf/fx.hpp>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;

///////////////////////////////////////////////////////////////////////////////
static constexpr auto control_sps = 1000;

float pot1 = 0.0f;
float pot2 = 0.0f;

struct infinity_controller
{
   static constexpr auto adc_id = 2;
   static constexpr auto timer_id = 3;
   static constexpr auto channels = 2;
   static constexpr auto sampling_rate = control_sps;
   static constexpr auto buffer_size = 1;

   template <typename T>
   void process(std::array<T, channels> const& in)
   {
      pot1 = in[0];
      pot2 = in[1];
   }

   template <typename Adc>
   void setup_channels(Adc& adc)
   {
      adc.template enable_channels<7, 8>();
   }
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
   static constexpr auto channels = 6;
   static constexpr auto sampling_rate = audio_clock;
   static constexpr auto buffer_size = 8;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      out[0] += blk1(s);
      out[1] += blk2(s);
   }

   template <typename Adc>
   void setup_channels(Adc& adc)
   {
      adc.template enable_channels<0, 12, 13, 3, 10, 11>();
   }

   inf::dc_block blk1, blk2;
};

inf::multi_channel_processor<inf::processor<infinity_processor>> proc;
inf::output_pin<inf::port::portc + 3> pin; // portc, pin 3

///////////////////////////////////////////////////////////////////////////////
void start()
{
   ctrl.start();
   proc.start();
   while (true)
      inf::delay_ms(500);
}

///////////////////////////////////////////////////////////////////////////////
inline void irq(adc_conversion_half_complete<2>)
{
   ctrl.irq_conversion_half_complete();
}

inline void irq(adc_conversion_complete<2>)
{
   ctrl.irq_conversion_complete();
}

///////////////////////////////////////////////////////////////////////////////
inline void irq(adc_conversion_half_complete<1>)
{
   pin = 1;
   proc.irq_conversion_half_complete();
   pin = 0;
}

inline void irq(adc_conversion_complete<1>)
{
   pin = 1;
   proc.irq_conversion_complete();
   pin = 0;
}

inline void irq(timer_task<2>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
