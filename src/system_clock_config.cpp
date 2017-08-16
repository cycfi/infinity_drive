/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_hal.h>

namespace cycfi { namespace infinity { namespace detail
{
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler */
}

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

      // Reset of all peripherals, Initializes the Flash interface and the Systick.
      HAL_Init();

      RCC_OscInitTypeDef RCC_OscInitStruct;
      RCC_ClkInitTypeDef RCC_ClkInitStruct;

         /**Configure the main internal regulator output voltage
         */
       __HAL_RCC_PWR_CLK_ENABLE();

       __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

         /**Initializes the CPU, AHB and APB busses clocks
         */
       RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
       RCC_OscInitStruct.HSIState = RCC_HSI_ON;
       RCC_OscInitStruct.HSICalibrationValue = 16;
       RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
       RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
       RCC_OscInitStruct.PLL.PLLM = 8;
       RCC_OscInitStruct.PLL.PLLN = 50;
       RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
       RCC_OscInitStruct.PLL.PLLQ = 7;
       if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
       {
         Error_Handler();
       }

         /**Initializes the CPU, AHB and APB busses clocks
         */
       RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
       RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
       RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
       RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
       RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

       if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
       {
         Error_Handler();
       }

         /**Configure the Systick interrupt time
         */
       HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

         /**Configure the Systick
         */
       HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

       /* SysTick_IRQn interrupt configuration */
       HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);











        //// Reset of all peripherals, Initializes the Flash interface and the Systick.
        //HAL_Init();
        //
        //// Enable HSE oscillator
        //LL_RCC_HSE_Enable();
        //while (LL_RCC_HSE_IsReady() != 1)
        //{
        //};
        //
        //// Set FLASH latency
        //LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
        //
        //// Enable PWR clock
        //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
        //
        //// Activation OverDrive Mode
        //LL_PWR_EnableOverDriveMode();
        //while (LL_PWR_IsActiveFlag_OD() != 1)
        //{
        //};
        //
        //// Activation OverDrive Switching
        //LL_PWR_EnableOverDriveSwitching();
        //while (LL_PWR_IsActiveFlag_ODSW() != 1)
        //{
        //};
        //
        //// Main PLL configuration and activation
        //LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 360, LL_RCC_PLLP_DIV_2);
        //LL_RCC_PLL_Enable();
        //while (LL_RCC_PLL_IsReady() != 1)
        //{
        //};
        //
        //// Sysclk activation on the main PLL
        //LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
        //LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        //while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
        //{
        //};
        //
        //// Set APB1 & APB2 prescaler
        //LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
        //LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
        //
        //// Set systick to 1ms
        //SysTick_Config(180000000 / 1000);
        //
        //// Update CMSIS variable (which can be updated also through
        //// SystemCoreClockUpdate function) */
        //SystemCoreClock = 180000000;
      }
   }
}}}
