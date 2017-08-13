/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_pwr.h>

namespace cycfi { namespace infinity { namespace detail
{
   ////////////////////////////////////////////////////////////////////////////
   // System Clock Configuration for STM32F4 series.
   //
   //    The system Clock is configured as follows : 
   //       System Clock source            = PLL (HSE)
   //       SYSCLK(Hz)                     = 180000000
   //       HCLK(Hz)                       = 180000000
   //       AHB Prescaler                  = 1
   //       APB1 Prescaler                 = 4
   //       APB2 Prescaler                 = 2
   //       HSE Frequency(Hz)              = 8000000
   //       PLL_M                          = 8
   //       PLL_N                          = 360
   //       PLL_P                          = 2
   //       VDD(V)                         = 3.3
   //       Main regulator output voltage  = Scale1 mode
   //       Flash Latency(WS)              = 5
   //
   ////////////////////////////////////////////////////////////////////////////

   void system_clock_config()
   {
      static bool init = false;
      if (!init)
      {
         init = true;         

        // Enable HSE oscillator
        LL_RCC_HSE_Enable();
        while (LL_RCC_HSE_IsReady() != 1)
        {
        };

        // Set FLASH latency
        LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

        // Enable PWR clock
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

        // Activation OverDrive Mode
        LL_PWR_EnableOverDriveMode();
        while (LL_PWR_IsActiveFlag_OD() != 1)
        {
        };

        // Activation OverDrive Switching
        LL_PWR_EnableOverDriveSwitching();
        while (LL_PWR_IsActiveFlag_ODSW() != 1)
        {
        };

        // Main PLL configuration and activation
        LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 360, LL_RCC_PLLP_DIV_2);
        LL_RCC_PLL_Enable();
        while (LL_RCC_PLL_IsReady() != 1)
        {
        };

        // Sysclk activation on the main PLL
        LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
        {
        };

        // Set APB1 & APB2 prescaler
        LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
        LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

        // Set systick to 1ms
        SysTick_Config(180000000 / 1000);

        // Update CMSIS variable (which can be updated also through 
        // SystemCoreClockUpdate function) */
        SystemCoreClock = 180000000;
      }
   }
}}}
