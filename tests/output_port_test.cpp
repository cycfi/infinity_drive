/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

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
using inf::delay_ms;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::main_led_type main_led;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(main_led.setup());

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   main_led = off;

   // toggle LED
   while (true)
   {
      delay_ms(1000);
      main_led = !main_led;
   }
}
