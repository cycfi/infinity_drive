/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015)
#define CYCFI_INFINITY_IRQ_HPP_DECEMBER_22_2015

#include <inf/timer.hpp>
#include <type_traits>

#if defined(STM32F4)
# include <stm32f4xx_ll_dma.h>
# include <stm32f4xx_ll_adc.h>
#else
# error "MCU not supported"
#endif

///////////////////////////////////////////////////////////////////////////////
// Timer Interrupts
///////////////////////////////////////////////////////////////////////////////

namespace cycfi { namespace infinity { namespace detail
{
   template <std::size_t N>
   void handle_timer_interrupt()
   {
      auto& timer = cycfi::infinity::detail::get_timer<N>();

      // Check whether update interrupt is pending
      if (LL_TIM_IsActiveFlag_UPDATE(&timer) == 1)
      {
         // Clear the update interrupt flag
         LL_TIM_ClearFlag_UPDATE(&timer);
      }
      // call timer_task
      irq(timer_task<N>{});
   }

   template <std::size_t N>
   void handle_adc_interrupt()
   {
      // Check whether DMA transfer complete caused the DMA interruption
      if (LL_DMA_IsActiveFlag_TC0(DMA2) == 1)
      {
         // Clear flag DMA transfer complete
         LL_DMA_ClearFlag_TC0(DMA2);

         // call adc_conversion_complete
         irq(adc_conversion_complete<N>{});
      }

      // Check whether DMA half transfer caused the DMA interruption
      if (LL_DMA_IsActiveFlag_HT0(DMA2) == 1)
      {
         // Clear flag DMA half transfer
         LL_DMA_ClearFlag_HT0(DMA2);

         // call adc_conversion_half_complete
         irq(adc_conversion_half_complete<N>{});
      }

      // Check whether DMA transfer error caused the DMA interruption
      if (LL_DMA_IsActiveFlag_TE0(DMA2) == 1)
      {
         // Clear flag DMA transfer error
         LL_DMA_ClearFlag_TE0(DMA2);

         // call error_handler
         cycfi::infinity::error_handler();
      }
   }
}}}

extern "C"
{

#define TIMER_INTERRUPT_HANDLER(N, NAME)                                       \
   void NAME()                                                                 \
   {                                                                           \
      using cycfi::infinity::detail::handle_timer_interrupt;                   \
      handle_timer_interrupt<N>();                                             \
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

   void DMA2_Stream0_IRQHandler(void)
   {
      using cycfi::infinity::detail::handle_adc_interrupt;
      handle_adc_interrupt<1>();
   }

   void DMA2_Stream1_IRQHandler(void)
   {
      using cycfi::infinity::detail::handle_adc_interrupt;
      handle_adc_interrupt<1>();
   }

   void DMA2_Stream2_IRQHandler(void)
   {
      using cycfi::infinity::detail::handle_adc_interrupt;
      handle_adc_interrupt<2>();
   }

   void DMA2_Stream3_IRQHandler(void)
   {
      using cycfi::infinity::detail::handle_adc_interrupt;
      handle_adc_interrupt<3>();
   }

   void ADC_IRQHandler(void)
   {
      // Check whether ADC group regular overrun caused the ADC interruption

      if (LL_ADC_IsActiveFlag_OVR(ADC1) != 0)
      {
         // Clear flag ADC group regular overrun
         LL_ADC_ClearFlag_OVR(ADC1);

         // call error_handler
         using cycfi::infinity::detail::handle_adc_interrupt;
         handle_adc_interrupt<1>();
         cycfi::infinity::error_handler();
      }

      if (LL_ADC_IsActiveFlag_OVR(ADC2) != 0)
      {
         // Clear flag ADC group regular overrun
         LL_ADC_ClearFlag_OVR(ADC2);

         // call error_handler
         cycfi::infinity::error_handler();
      }

      if (LL_ADC_IsActiveFlag_OVR(ADC3) != 0)
      {
         // Clear flag ADC group regular overrun
         LL_ADC_ClearFlag_OVR(ADC3);

         // call error_handler
         cycfi::infinity::error_handler();
      }
   }
}

#endif
