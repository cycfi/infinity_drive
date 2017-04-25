/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015

#include "stm32l4xx.h"
#include <cstdint>
#include <type_traits>

#include <inf/pin.hpp>
#include <inf/support.hpp>
#include <inf/detail/timer_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // timer
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   struct timer
   {
      timer(uint32_t clock_frequency, uint32_t period)
      {
         static_assert(detail::check_valid_timer(N), "Invalid Timer N");

          // Enable the timer peripheral clock
         LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

         // Set the pre-scaler value
         LL_TIM_SetPrescaler(get_timer(), __LL_TIM_CALC_PSC(SystemCoreClock, clock_frequency));

         // Set the auto-reload value to have an initial update event frequency of
         // clock_frequency/period
         auto freq = clock_frequency/period;
         auto autoreload = __LL_TIM_CALC_ARR(
            SystemCoreClock, LL_TIM_GetPrescaler(get_timer()), freq);

         LL_TIM_SetAutoReload(get_timer(), autoreload);
      }

      void enable_interrupt(std::size_t priority = 0)
      {
         // Enable the update interrupt
         LL_TIM_EnableIT_UPDATE(get_timer());

         // Configure the NVIC to handle timer update interrupt
         NVIC_SetPriority(get_irqn(), priority);
         NVIC_EnableIRQ(get_irqn());
      }

      void start()
      {
         // Enable counter
         LL_TIM_EnableCounter(get_timer());

         // Force update generation
         LL_TIM_GenerateEvent_UPDATE(get_timer());
      }

      void stop()
      {
         LL_TIM_DisableCounter(get_timer());
      }

   private:

      static constexpr TIM_TypeDef* get_timer()
      {
         return &detail::get_timer<N>();
      }

      static constexpr IRQn_Type get_irqn()
      {
         static_assert(detail::timer_irqn<N>() != -1, "Timer has no interrupt capability.");
         return IRQn_Type(detail::timer_irqn<N>());
      }

//      static constexpr int sys_clock_div()
//      {
//         return (N == 1 || N == 9 || N == 10 || N == 11) ? 1 : 2;
//      }
//
//      static constexpr int timer_irq()
//      {
//         switch (N)
//         {
//            case 2:  return TIM2_IRQn;
//            case 3:  return TIM3_IRQn;
//            case 4:  return TIM4_IRQn;
//            case 5:  return TIM5_IRQn;
//            case 7:  return TIM7_IRQn;
//            default: return -1;
//         }
//         return 0;
//      }
//
//      inline void enable_timer_clock()
//      {
//         // TIMx Peripheral clock enable
//         switch (N)
//         {
////            case 1:  __HAL_RCC_TIM1_CLK_ENABLE();  break;
////            case 2:  __HAL_RCC_TIM2_CLK_ENABLE();  break;
////            case 3:  __HAL_RCC_TIM3_CLK_ENABLE();  break;
////            case 4:  __HAL_RCC_TIM4_CLK_ENABLE();  break;
////            case 5:  __HAL_RCC_TIM5_CLK_ENABLE();  break;
////            case 6:  __HAL_RCC_TIM6_CLK_ENABLE();  break;
////            case 7:  __HAL_RCC_TIM7_CLK_ENABLE();  break;
////            case 8:  __HAL_RCC_TIM8_CLK_ENABLE();  break;
////            case 9:  __HAL_RCC_TIM9_CLK_ENABLE();  break;
////            case 10: __HAL_RCC_TIM10_CLK_ENABLE(); break;
////            case 11: __HAL_RCC_TIM11_CLK_ENABLE(); break;
////            case 12: __HAL_RCC_TIM12_CLK_ENABLE(); break;
////            case 13: __HAL_RCC_TIM13_CLK_ENABLE(); break;
////            case 14: __HAL_RCC_TIM14_CLK_ENABLE(); break;
//         }
//      }

      //TIM_HandleTypeDef h;
   };
}}

#endif
