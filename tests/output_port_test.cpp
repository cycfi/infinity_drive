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

using inf::main_led_type;
using inf::delay_ms;

void start()
{
   main_led_type main_led;
   main_led = off;

   // toggle LED
   while (true)
   {
      delay_ms(1000);
      main_led = !main_led;
   }
}
