/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include "diag/Trace.h"

namespace inf = cycfi::infinity;

using inf::clock_speed;
using inf::output_pin;
using inf::delay_ms;
using inf::portd;
using inf::on;
using inf::off;

void start()
{
   output_pin<portd + 14> red_led;     // portd, pin 14
   output_pin<portd + 15> blue_led;    // portd, pin 15
   uint32_t seconds = 0;

   blue_led = on;
   red_led = off;

   trace_puts("Hello Infinity!");
   trace_printf("System clock: %u Hz\n", clock_speed);

   // toggle LEDs
   while (true)
   {
      delay_ms(1000);
      blue_led = !blue_led;
      red_led = !red_led;

      ++seconds;
      trace_printf("Second %u\n", seconds);
   }
}
