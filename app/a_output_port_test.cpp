/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Simplest toggle the led test. This test uses simple delay to toggle
// the led at a rate of 1 per second. No setup required.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::output_pin;
using inf::delay_ms;

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
