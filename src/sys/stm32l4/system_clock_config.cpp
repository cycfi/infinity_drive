/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_utils.h"

namespace detail
{
   ////////////////////////////////////////////////////////////////////////////
   // System Clock Configuration for STM32L4
   //
   // The system Clock is configured as follows:
   //
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
      // MSI configuration and activation
      LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
      LL_RCC_MSI_Enable();
      while (LL_RCC_MSI_IsReady() != 1)
      {
      };

      // Main PLL configuration and activation
      LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
      LL_RCC_PLL_Enable();
      LL_RCC_PLL_EnableDomain_SYS();
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
      LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
      LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

      // Set systick to 1ms in using frequency set to 80MHz
      // This frequency can be calculated through LL RCC macro
      // ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6),
      //                               LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2)
      LL_Init1msTick(80000000);

      // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
      LL_SetSystemCoreClock(80000000);
   }
}
