/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/app.hpp>

// The main start function entry point
void start();

namespace inf = cycfi::infinity;

namespace detail
{
   void system_clock_config();
}

int main()
{
   // Configure the system clock
   detail::system_clock_config();

   start();
   return 0;
}

namespace cycfi { namespace infinity
{
   // Our error handler
   void error_handler()
   {
      using namespace port;
      red_led_type red_led;
      red_led = on;
      while (true)
      {
         for (int i = 0; i < 3; ++i)
         {
            red_led = off;
            delay_ms(100);
            red_led = on;
            delay_ms(100);
         }
         delay_ms(2000);
      }
   }
}}
