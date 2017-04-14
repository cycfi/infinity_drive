/*=============================================================================
   Copyright � 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015

#include "stm32f4xx.h"
#include <cstdint>
#include <type_traits>

#include <inf/pin.hpp>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   namespace detail
   {
      constexpr int sys_clock_div(int timer)
      {
         return (timer == 1 || timer == 9 || timer == 10 || timer == 11) ? 1 : 2;
      }

      constexpr TIM_TypeDef* get_timer(int timer)
      {
         switch (timer)
         {
            case 1:  return TIM1;
            case 2:  return TIM2;
            case 3:  return TIM3;
            case 4:  return TIM4;
            case 5:  return TIM5;
            case 6:  return TIM6;
            case 7:  return TIM7;
            case 8:  return TIM8;
            case 9:  return TIM9;
            case 10: return TIM10;
            case 11: return TIM11;
            case 12: return TIM12;
            case 13: return TIM13;
            case 14: return TIM14;
         }
         return 0;
      }

      constexpr int get_timer_irq(int timer)
      {
         switch (timer)
         {
            case 2:  return TIM2_IRQn;
            case 3:  return TIM3_IRQn;
            case 4:  return TIM4_IRQn;
            case 5:  return TIM5_IRQn;
            case 6:  return -1;
            case 7:  return TIM7_IRQn;
            default: return -1;
         }
         return 0;
      }

      inline void enable_timer_clock(int timer)
      {
         // TIMx Peripheral clock enable
         switch (timer)
         {
            case 1:  __HAL_RCC_TIM1_CLK_ENABLE();  break;
            case 2:  __HAL_RCC_TIM2_CLK_ENABLE();  break;
            case 3:  __HAL_RCC_TIM3_CLK_ENABLE();  break;
            case 4:  __HAL_RCC_TIM4_CLK_ENABLE();  break;
            case 5:  __HAL_RCC_TIM5_CLK_ENABLE();  break;
            case 6:  __HAL_RCC_TIM6_CLK_ENABLE();  break;
            case 7:  __HAL_RCC_TIM7_CLK_ENABLE();  break;
            case 8:  __HAL_RCC_TIM8_CLK_ENABLE();  break;
            case 9:  __HAL_RCC_TIM9_CLK_ENABLE();  break;
            case 10: __HAL_RCC_TIM10_CLK_ENABLE(); break;
            case 11: __HAL_RCC_TIM11_CLK_ENABLE(); break;
            case 12: __HAL_RCC_TIM12_CLK_ENABLE(); break;
            case 13: __HAL_RCC_TIM13_CLK_ENABLE(); break;
            case 14: __HAL_RCC_TIM14_CLK_ENABLE(); break;
         }
      }

      inline void enable_interrupt(int timer, std::size_t priority)
      {
         auto id = get_timer_irq(timer);
         if (id == -1)
            error_handler();
         HAL_NVIC_SetPriority(IRQn_Type(id), priority, 0);
         HAL_NVIC_EnableIRQ(IRQn_Type(id));
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // timer
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   struct timer
   {
      static_assert(N >=1 && N <= 14, "Invalid Timer N");

      timer(uint32_t clock_frequency, uint32_t period)
      {
         h.Instance = detail::get_timer(N);

         h.Init.Period = period - 1;

         uint32_t timer_clock = SystemCoreClock / detail::sys_clock_div(N);
         uint32_t prescaler_val = (timer_clock / clock_frequency) - 1;
         if (prescaler_val > int_max<decltype(h.Init.Prescaler)>())
            error_handler();  // Overflow error

         h.Init.Prescaler = prescaler_val;
         h.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
         h.Init.CounterMode = TIM_COUNTERMODE_UP;

         detail::enable_timer_clock(N);
         if (HAL_TIM_Base_Init(&h) != HAL_OK)
           error_handler();   // Initialization Error
      }

      void enable_interrupt(std::size_t priority = 0)
      {
         detail::enable_interrupt(N, priority);
      }

      void start()
      {
         if (HAL_TIM_Base_Start_IT(&h) != HAL_OK)
            error_handler();
      }

      void stop()
      {
         if (HAL_TIM_Base_Stop_IT(&h) != HAL_OK)
            error_handler();
      }

   private:

      TIM_HandleTypeDef h;
   };
}}

#endif
