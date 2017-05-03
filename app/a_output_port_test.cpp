/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>

#include <inf/app.hpp>

namespace inf = cycfi::infinity;

using inf::clock_speed;
using inf::output_pin;
using inf::delay_ms;
using inf::porta;
using inf::on;
using inf::off;

void start()
{
   output_pin<porta + 5> red_led; // porta, pin 5
   red_led = off;

   // toggle LED
   while (true)
   {
      delay_ms(1000);
      red_led = !red_led;
   }
}
