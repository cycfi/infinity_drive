/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>
#include <inf/config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Toggle led test using timers and interrupts. This test uses a timer to
// toggle the led at a rate of 1 per second. No setup required.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;
using inf::main_led_type;

main_led_type main_led;
constexpr uint32_t base_freq = 10000;
inf::timer<3> tmr(base_freq, 1); // 1Hz

void timer_task()
{
   main_led = !main_led;
}

auto config = inf::config(
   tmr(timer_task)
);

void start()
{
   tmr.start();
   main_led = on;
   while (true)
      ;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
