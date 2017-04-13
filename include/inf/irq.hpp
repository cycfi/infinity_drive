/*=======================================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015)
#define CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015

#include <inf/timer.hpp>

///////////////////////////////////////////////////////////////////////////////
// Timer Interrupts
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#define TIMER_INTERRUPT_HANDLER(N)                                             \
   void TIM##N##_IRQHandler(void)                                              \
   {                                                                           \
      if (TIM_GetITStatus(TIM##N, TIM_IT_Update) != RESET)                     \
      {                                                                        \
         TIM_ClearITPendingBit(TIM##N, TIM_IT_Update);                         \
         irq(timer_task<N>{});                                                 \
      }                                                                        \
   }                                                                           \
   /***/

#if defined(TIM2)
 TIMER_INTERRUPT_HANDLER(2)
#endif

#if defined(TIM3)
 TIMER_INTERRUPT_HANDLER(3)
#endif

#if defined(TIM4)
 TIMER_INTERRUPT_HANDLER(4)
#endif

#if defined(TIM5)
 TIMER_INTERRUPT_HANDLER(5)
#endif

#if defined(TIM6)
 TIMER_INTERRUPT_HANDLER(6)
#endif

#if defined(TIM7)
 TIMER_INTERRUPT_HANDLER(7)
#endif

#if defined(TIM8)
 TIMER_INTERRUPT_HANDLER(8)
#endif

#if defined(TIM9)
 TIMER_INTERRUPT_HANDLER(9)
#endif

#if defined(TIM10)
 TIMER_INTERRUPT_HANDLER(10)
#endif

#if defined(TIM11)
 TIMER_INTERRUPT_HANDLER(11)
#endif

///////////////////////////////////////////////////////////////////////////////
// ADC Interrupt
///////////////////////////////////////////////////////////////////////////////

   void DMA2_Stream0_IRQHandler()
   {
      // Test on DMA Stream Half Transfer interrupt
      if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
      {
         // Clear DMA Stream Half Transfer interrupt pending bit
         DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);

         // Call the app irq task (ping)
         irq(adc_conversion_half_complete{});
      }

      // Test on DMA Transfer Complete interrupt
      if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
      {
         // Clear DMA Transfer Complete interrupt pending bit
         DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);

         // Call the app irq task (pong)
         irq(adc_conversion_complete{});
      }
   }
}

#endif
