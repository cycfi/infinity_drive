/*=======================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <inf/support.hpp>
#include <inf/app.hpp>
#include "stm32f4xx_it.h"

// The main start function entry point xxx
void start();

namespace inf = cycfi::infinity;

namespace
{
   static void system_clock_config();
}

int main()
{
   // STM32F4xx HAL library initialization:
   //    - Configure the Flash prefetch, instruction and Data caches
   //    - Configure the Systick to generate an interrupt each 1 msec
   //    - Set NVIC Group Priority to 4
   //    - Global MSP (MCU Support Package) initialization
   if (HAL_Init() != HAL_OK)
      inf::error_handler();

   // Configure the system clock to 168 MHz
   system_clock_config();

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

namespace
{
   ////////////////////////////////////////////////////////////////////////////
   //  The system Clock is configured as follows :
   //  System Clock source            = PLL (HSE)
   //  SYSCLK(Hz)                     = 168000000
   //  HCLK(Hz)                       = 168000000
   //  AHB Prescaler                  = 1
   //  APB1 Prescaler                 = 4
   //  APB2 Prescaler                 = 2
   //  HSE Frequency(Hz)              = 8000000
   //  PLL_M                          = 8
   //  PLL_N                          = 336
   //  PLL_P                          = 2
   //  PLL_Q                          = 7
   //  VDD(V)                         = 3.3
   //  Main regulator output voltage  = Scale1 mode
   //  Flash Latency(WS)              = 5

   void system_clock_config()
   {
      RCC_ClkInitTypeDef RCC_ClkInitStruct;
      RCC_OscInitTypeDef RCC_OscInitStruct;

      // Enable Power Control clock
      __HAL_RCC_PWR_CLK_ENABLE();

      // The voltage scaling allows optimizing the power consumption when the device is
      // clocked below the maximum system frequency, to update the voltage scaling value
      // regarding system frequency refer to product datasheet.
      __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

      // Enable HSE Oscillator and activate PLL with HSE as source
      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
      RCC_OscInitStruct.PLL.PLLM = 8;
      RCC_OscInitStruct.PLL.PLLN = 336;
      RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
      RCC_OscInitStruct.PLL.PLLQ = 7;
      HAL_RCC_OscConfig(&RCC_OscInitStruct);

      // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
      // clocks dividers
      RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
      HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

      // STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported
      if (HAL_GetREVID() == 0x1001)
      {
         // Enable the Flash prefetch
         __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
      }
   }
}

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
