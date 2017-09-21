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
//    conversion_half_complete
//
// and then again when it concludes the entire conversion for the entire 
// buffer:
//
//    conversion_complete
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::delay_ms;

constexpr int sampling_rate = 32000;
constexpr uint32_t adc_clock_rate = 2000000;
constexpr unsigned buffer_size = 1024;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
using adc_type = inf::adc<1, 1, buffer_size>;

// The main clock
inf::timer<2> clock;

// The ADC
adc_type adc;
adc_type::buffer_iterator_type out = adc.begin();

// The DAC
inf::dac<0> dac;

///////////////////////////////////////////////////////////////////////////////
// Callbacks
inline void conversion_half_complete()
{
   out = adc.begin();
}

inline void conversion_complete()
{
   out = adc.middle();
}

void timer_task()
{
   dac((*out++)[0]);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   clock.setup(adc_clock_rate, sampling_rate, timer_task),
   adc.setup(clock, conversion_half_complete, conversion_complete),
   adc.enable_channels<0>(),
   dac.setup()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   adc.start();
   clock.start();

   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
