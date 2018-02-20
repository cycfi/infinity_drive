/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Toggle led port using timers and interrupts. This test uses a timer to
// toggle pin PC3 at a rate of 100kHz. The clock frequency is set to 200kHz,
// but since the toggling of the pin happens to halve this frequency, you'll
// see a frequency of 100kHz.
//
// Setup connect an oscilloscope probe to pin PC3
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;
using inf::output_pin;

output_pin<porta + 0> pin; // porta, pin 0

constexpr std::uint32_t base_freq = 1000000;
constexpr std::uint32_t frequency = 200000;
inf::timer<3> tmr(base_freq, frequency);

void start()
{
   tmr.enable_interrupt();
   tmr.start();

   while (true)
      ;
}

void irq(timer_task<3>)
{
   pin = !pin;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
