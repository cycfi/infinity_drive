/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Interrupt button test. The main button, is configured with a pull-up
// to vcc (hence normally 1). The button is also configured to fire up an
// interrupt on the falling edge (when the button is pressed, it transitions
// from 1 to 0). An exti_task is setup to handle this interrupt. The task
// simply toggles the main LED.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

inf::main_led_type led;
inf::main_button_type btn;
constexpr auto btn_n = inf::main_button_type::bit;

void irq(exti_task<btn_n>)
{
   led = !led;
}

void start()
{
   btn.enable_interrupt(10);
   btn.start(falling);

   while (true)
   {
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>

