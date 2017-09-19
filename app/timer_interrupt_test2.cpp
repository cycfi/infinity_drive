/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/config.hpp>

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

///////////////////////////////////////////////////////////////////////////////
// Peripherals
output_pin<portc + 3> pin; // portc, pin 3
inf::timer<3> tmr;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   pin = !pin;
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t base_freq = 1000000;
constexpr uint32_t frequency = 200000;

auto config = inf::config(
   pin.setup(),
   tmr.setup(base_freq, frequency, timer_task) // calls timer_task every 200kHz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();
   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
