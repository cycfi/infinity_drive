/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Simplest button test. We poll the main button (which is configured
// with a pull-up to vcc, hence normally 1). The main led is turned ON
// if the button is pushed, otherwise the main led is turned OFF.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::main_button_type;
using inf::main_led_type;
using inf::delay_ms;

void start()
{
   main_button_type btn;
   main_led_type main_led;
   main_led = off;

   // toggle LED
   while (true)
   {
      delay_ms(30);
      main_led = !btn;
   }
}
