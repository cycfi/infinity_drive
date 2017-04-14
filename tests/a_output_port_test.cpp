/*=======================================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>

#include <inf/app.hpp>

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

   blue_led = on;
   red_led = off;

   // toggle LEDs
   while (true)
   {
      delay_ms(1000);
      blue_led = !blue_led;
      red_led = !red_led;
   }
}
