/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;
using inf::on;
using inf::off;
using inf::portd;
using inf::output_pin;

output_pin<portd + 14> red_led;     // portd, pin 14
output_pin<portd + 15> blue_led;    // portd, pin 15

constexpr uint32_t base_freq = 10000;
constexpr uint32_t period = 10000;

void start()
{
   inf::timer<3> tmr(base_freq, period); // one second period
   tmr.enable_interrupt();
   tmr.enable();

   red_led = on;
   blue_led = off;

   while (true)
      ;
}

void irq(timer_task<3>)
{
   red_led = !red_led;
   blue_led = !blue_led;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>

