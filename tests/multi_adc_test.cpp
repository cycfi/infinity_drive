/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/fx.hpp>

///////////////////////////////////////////////////////////////////////////////
// Multichannel ADC test. Tests the multichannel ADC.
//
// Setup: Connect an input signals to channels 0, 12, 13, 3, 10, and 11.
// PA0, PC2, PC3, PA3, PC0, and PC1. Connect pin PA4 and PA5 to an
// oscilloscope to see the mixed waveform.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;

static constexpr auto clock = 64000;
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

inf::multi_channel_processor<inf::processor<my_processor>> proc;
inf::output_pin<inf::port::portc + 3> pin; // portc, pin 3

void start()
{
   proc.start();
   while (true)
      inf::delay_ms(500);
}

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

void irq(timer_task<2>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
