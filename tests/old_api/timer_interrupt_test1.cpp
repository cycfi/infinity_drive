/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Toggle led test using timers and interrupts. This test uses a timer to
// toggle the led at a rate of 1 per second. No setup required.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;
using inf::main_led_type;

main_led_type main_led;
constexpr std::uint32_t base_freq = 10000;

void start()
{
   inf::timer<3> tmr(base_freq, 1); // 1Hz
   tmr.enable_interrupt();
   tmr.start();

   main_led = on;

   while (true)
      ;
}

void irq(timer_task<3>)
{
   main_led = !main_led;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
