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
using inf::input_pin;
using inf::port_input_type;
using inf::delay_ms;
using inf::porta;
using inf::portc;
using inf::on;
using inf::off;

auto const pull_up = port_input_type::pull_up;

void start()
{
   output_pin<porta + 5> red_led;         // porta, pin 5
   input_pin<portc + 13, pull_up> button; // portc, pin 13
   red_led = off;

   // toggle LED 1s or 100ms frequency depending on button state
   while (true)
   {
      if (button)
         delay_ms(1000);
      else
         delay_ms(100);
      red_led = !red_led;
   }
}
