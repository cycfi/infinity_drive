/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/app.hpp>

// The main start function entry point
void start();

namespace inf = cycfi::infinity;

int main()
{
   // Configure the system clock
   inf::detail::system_clock_config();

   start();
   return 0;
}

namespace cycfi { namespace infinity
{
   // Our error handler
   void error_handler()
   {
      using namespace port;
      main_led_type main_led;
      main_led = on;
      while (true)
      {
         for (int i = 0; i < 3; ++i)
         {
            main_led = off;
            delay_ms(100);
            main_led = on;
            delay_ms(100);
         }
         delay_ms(2000);
      }
   }
}}
