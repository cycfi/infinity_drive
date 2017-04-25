/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015)
#define CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015

#include <inf/timer.hpp>

///////////////////////////////////////////////////////////////////////////////
// Timer Interrupts
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#define TIMER_INTERRUPT_HANDLER(N, NAME)                                       \
   void NAME()                                                                 \
   {                                                                           \
      if (LL_TIM_IsActiveFlag_UPDATE(TIM##N) == 1)                             \
      {                                                                        \
         LL_TIM_ClearFlag_UPDATE(TIM##N);                                      \
      }                                                                        \
      irq(timer_task<N>{});                                                    \
   }                                                                           \
   /***/

#if defined(TIM1)
 TIMER_INTERRUPT_HANDLER(1, TIM1_UP_TIM16_IRQHandler)
#endif

#if defined(TIM2)
 TIMER_INTERRUPT_HANDLER(2, TIM2_IRQHandler)
#endif

#if defined(TIM3)
 TIMER_INTERRUPT_HANDLER(3, TIM3_IRQHandler)
#endif

#if defined(TIM4)
 TIMER_INTERRUPT_HANDLER(4, TIM4_IRQHandler)
#endif

#if defined(TIM5)
 TIMER_INTERRUPT_HANDLER(5, TIM5_IRQHandler)
#endif

#if defined(TIM7)
 TIMER_INTERRUPT_HANDLER(7, TIM7_IRQHandler)
#endif

#if defined(TIM8)
 TIMER_INTERRUPT_HANDLER(8, TIM8_UP_IRQHandler)
#endif

}

#endif
