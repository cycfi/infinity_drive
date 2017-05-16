/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Input pin test. The speed of the blinking led depends on state of the
// dev-board button. Normally the button is on due to the pullup, hence
// the led blinks at 1hz. When the button is pressed, the led blinks at
// 10hz. No setup required.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::output_pin;
using inf::input_pin;
using inf::delay_ms;

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
