/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/adc.hpp>
#include <inf/dac.hpp>
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// ADC and DAC test. We read the signal from ADC channel 0 (pin PA0)
// and send it out through the DAC (pin PA4). The ADC is buffered. The DAC
// output will be delayed by 1024 samples (the buffer size). With a sampling
// rate of 32kHz, the delay will be 16ms. This delay is computed as:
//
//    ((1/32000) * 1024) / 2
//
// We divide by two because the ADC calls our processing routine twice.
// First when it finishes sampling half the buffer size:
//
//    adc_conversion_half_complete
//
// and then again when it concludes the entire conversion for the entire
// buffer:
//
//    adc_conversion_complete
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pin PA4 to an oscilloscope to see the waveform.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::delay_ms;

constexpr int sampling_rate = 32000;
constexpr std::uint32_t adc_clock_rate = 2000000;
constexpr unsigned buffer_size = 1024;
using adc_type = inf::adc<1, 1, buffer_size>;

// The main clock
inf::timer<2> clock(adc_clock_rate, sampling_rate);

// The ADC
adc_type adc(clock);
adc_type::buffer_iterator_type out = adc.begin();

// The DAC
inf::dac<0> dac;

void start()
{
   // channel 0, porta pin 0
   adc.enable_channel<0, 1>();

   adc.start();
   clock.start();
   clock.enable_interrupt();

   while (true)
   {
      delay_ms(500);
   }
}

inline void irq(adc_conversion_half_complete<1>)
{
   out = adc.begin();
}

inline void irq(adc_conversion_complete<1>)
{
   out = adc.middle();
}

void irq(timer_task<2>)
{
   dac((*out++)[0]);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
