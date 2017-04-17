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
   inline ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT)
   {
     ITStatus bitstatus = RESET;
     uint16_t itstatus = 0x0, itenable = 0x0;
     itstatus = TIMx->SR & TIM_IT;

     itenable = TIMx->DIER & TIM_IT;
     if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
     {
       bitstatus = SET;
     }
     else
     {
       bitstatus = RESET;
     }
     return bitstatus;
   }

   inline void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT)
   {
     /* Clear the IT pending Bit */
     TIMx->SR = (uint16_t)~TIM_IT;
   }

#define TIMER_INTERRUPT_HANDLER(N)                                             \
   void TIM##N##_IRQHandler(void)                                              \
   {                                                                           \
      if (TIM_GetITStatus(TIM##N, TIM_IT_UPDATE) != RESET)                     \
      {                                                                        \
         TIM_ClearITPendingBit(TIM##N, TIM_IT_UPDATE);                         \
         irq(timer_task<N>{});                                                 \
      }                                                                        \
   }                                                                           \
   /***/

    //if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
    //{
    //  __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
    //  HAL_TIM_PeriodElapsedCallback(htim);
    //}

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

   //void DMA2_Stream0_IRQHandler()
   //{
   //   // Test on DMA Stream Half Transfer interrupt
   //   if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
   //   {
   //      // Clear DMA Stream Half Transfer interrupt pending bit
   //      DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
   //
   //      // Call the app irq task (ping)
   //      irq(adc_conversion_half_complete{});
   //   }
   //
   //   // Test on DMA Transfer Complete interrupt
   //   if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
   //   {
   //      // Clear DMA Transfer Complete interrupt pending bit
   //      DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
   //
   //      // Call the app irq task (pong)
   //      irq(adc_conversion_complete{});
   //   }
   //}
}

#endif
