/*=======================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <inf/support.hpp>
#include <inf/app.hpp>
#include "sys/stm32l4xx_it.h"

// The main start function entry point xxx
void start();

namespace inf = cycfi::infinity;

namespace
{
   static void system_clock_config();
}

int main()
{
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
   // System Clock Configuration
   // The system Clock is configured as follows :
   //    System Clock source            = PLL (MSI)
   //    SYSCLK(Hz)                     = 80000000
   //    HCLK(Hz)                       = 80000000
   //    AHB Prescaler                  = 1
   //    APB1 Prescaler                 = 1
   //    APB2 Prescaler                 = 1
   //    MSI Frequency(Hz)              = 4000000
   //    PLL_M                          = 1
   //    PLL_N                          = 40
   //    PLL_R                          = 2
   //    Flash Latency(WS)              = 4

   void system_clock_config()
   {
      /* MSI configuration and activation */
      LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
      LL_RCC_MSI_Enable();
      while(LL_RCC_MSI_IsReady() != 1)
      {
      };

      /* Main PLL configuration and activation */
      LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
      LL_RCC_PLL_Enable();
      LL_RCC_PLL_EnableDomain_SYS();
      while(LL_RCC_PLL_IsReady() != 1)
      {
      };

      /* Sysclk activation on the main PLL */
      LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
      LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
      while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
      {
      };

      /* Set APB1 & APB2 prescaler*/
      LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
      LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

      /* Set systick to 1ms in using frequency set to 80MHz */
      /* This frequency can be calculated through LL RCC macro */
      /* ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6),
                                      LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2)*/
      LL_Init1msTick(80000000);

      /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
      LL_SetSystemCoreClock(80000000);
   }
}


