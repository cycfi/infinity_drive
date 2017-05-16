/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

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
using inf::output_pin;

output_pin<porta + 5> red_led; // porta, pin 5

constexpr uint32_t base_freq = 10000;

void start()
{
   inf::timer<1> tmr(base_freq, 1); // 1Hz
   tmr.enable_interrupt();
   tmr.start();

   red_led = on;

   while (true)
      ;
}

void irq(timer_task<1>)
{
   red_led = !red_led;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
