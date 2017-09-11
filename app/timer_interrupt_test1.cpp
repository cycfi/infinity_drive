/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Toggle led test using timers and interrupts. This test uses a timer to
// toggle the led at a rate of 1 per second. No setup required.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::main_led_type main_led;
inf::timer<3> tmr;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   main_led = !main_led;
}

///////////////////////////////////////////////////////////////////////////////
// Configuration.
constexpr uint32_t base_freq = 10000;

auto config = inf::config(
   main_led.setup(),
   tmr.setup(base_freq, 1, timer_task) // calls timer_task every 1Hz
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   tmr.start();
   main_led = on;
   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
