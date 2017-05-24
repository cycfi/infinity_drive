/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include "period_trigger.hpp"

///////////////////////////////////////////////////////////////////////////////
// Period Trigger test. Generates square pulses that correspond the period
// of a waveform.
//
// Setup: Connect an input signal (e.g. signal gen) to pin PC0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
static constexpr auto sps_div = 4;

struct my_processor
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = 32000;
   static constexpr auto buffer_size = 1024;

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      out[0] = _pt(s);
      out[1] = s;
   }
   
   template <typename Adc>
   void setup_channels(Adc& adc)
   {
      // channel 0, porta pin 0
      adc.template enable_channel<0, 1>();
   }

   inf::period_trigger<32000 / sps_div> _pt;
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
