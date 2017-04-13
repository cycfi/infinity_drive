/*=======================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <inf/support.hpp>
#include <inf/app.hpp>
#include "stm32f4xx_it.h"

// The main start function entry point xxx
void start();

int main()
{
   start();
   return 0;
}

namespace cycfi { namespace infinity
{
   // Our error handler
   void error_handler()
   {
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

extern "C"
{
   volatile uint32_t timer_delay;

   void SysTick_Handler(void)
   {
	  HAL_IncTick();
      if (timer_delay != 0u)
         timer_delay--;
   }
}
