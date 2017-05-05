/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_MAY_1_2017)
#define CYCFI_INFINITY_ADC_HPP_MAY_1_2017

#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"

namespace cycfi { namespace infinity { namespace detail
{
   void adc_dma_config(
      ADC_TypeDef* adc_n,
      uint32_t dma_channel,
      IRQn_Type dma_channel_irq,
      uint16_t values[],
      uint16_t size
   );

   void adc_config(
      ADC_TypeDef* adc,
      uint32_t timer_trigger_id
   );

   void activate_adc(ADC_TypeDef* adc);

   constexpr bool valid_adc_timer(int N)
   {
      return N == 1 || N == 2 || N == 3 || N == 4 || N == 6 || N == 8 || N == 15;
   }

   constexpr bool valid_adc_channel(int N)
   {
      return N >= 1 && N <= 18;
   }

   ////////////////////////////////////////////////////////////////////////////
   // The ADCs: We provide template functions for getting the memory mapped
	//	adcs given a constant N. That way, we can use generic programming.
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   ADC_TypeDef* get_adc();

   template <std::size_t N>
   struct adc_info;

   template <std::size_t N>
   uint32_t adc_timer_trigger_id();

   template <std::size_t N>
   uint32_t adc_rank();

   template <std::size_t N>
   uint32_t adc_channel();

#define INFINITY_ADC(N)                                                        \
                                                                               \
   template <>                                                                 \
   inline ADC_TypeDef* get_adc<N>()                                            \
   {                                                                           \
      return ADC##N;                                                           \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   struct adc_info<N>                                                          \
   {                                                                           \
      static constexpr IRQn_Type dma_irq_id = DMA1_Channel##N##_IRQn;          \
      static constexpr uint32_t dma_channel = LL_DMA_CHANNEL_##N;              \
   };                                                                          \
   /***/

#define INFINITY_ADC_TIMER_TRIGGER(N)                                          \
   template <>                                                                 \
   inline uint32_t adc_timer_trigger_id<N>()                                   \
   {                                                                           \
      return LL_ADC_REG_TRIG_EXT_TIM##N##_TRGO;                                \
   }                                                                           \
   /***/

#define INFINITY_ADC_RANK(N)                                                   \
   template <>                                                                 \
   inline uint32_t adc_rank<N>()                                               \
   {                                                                           \
      return LL_ADC_REG_RANK_##N;                                              \
   }                                                                           \
   /***/

#define INFINITY_ADC_CHANNEL(N)                                                \
   template <>                                                                 \
   inline uint32_t adc_channel<N>()                                            \
   {                                                                           \
      return LL_ADC_CHANNEL_##N;                                               \
   }                                                                           \
   /***/

   INFINITY_ADC(1)
   INFINITY_ADC(2)
   INFINITY_ADC(3)

   INFINITY_ADC_TIMER_TRIGGER(1)
   INFINITY_ADC_TIMER_TRIGGER(2)
   INFINITY_ADC_TIMER_TRIGGER(3)
   INFINITY_ADC_TIMER_TRIGGER(4)
   INFINITY_ADC_TIMER_TRIGGER(6)
   INFINITY_ADC_TIMER_TRIGGER(8)
   INFINITY_ADC_TIMER_TRIGGER(15)

   INFINITY_ADC_RANK(1)
   INFINITY_ADC_RANK(2)
   INFINITY_ADC_RANK(3)
   INFINITY_ADC_RANK(4)
   INFINITY_ADC_RANK(5)
   INFINITY_ADC_RANK(6)
   INFINITY_ADC_RANK(7)
   INFINITY_ADC_RANK(8)
   INFINITY_ADC_RANK(9)
   INFINITY_ADC_RANK(10)
   INFINITY_ADC_RANK(11)
   INFINITY_ADC_RANK(12)
   INFINITY_ADC_RANK(13)
   INFINITY_ADC_RANK(14)
   INFINITY_ADC_RANK(15)
   INFINITY_ADC_RANK(16)

   INFINITY_ADC_CHANNEL(1)
   INFINITY_ADC_CHANNEL(2)
   INFINITY_ADC_CHANNEL(3)
   INFINITY_ADC_CHANNEL(4)
   INFINITY_ADC_CHANNEL(5)
   INFINITY_ADC_CHANNEL(6)
   INFINITY_ADC_CHANNEL(7)
   INFINITY_ADC_CHANNEL(8)
   INFINITY_ADC_CHANNEL(9)
   INFINITY_ADC_CHANNEL(10)
   INFINITY_ADC_CHANNEL(11)
   INFINITY_ADC_CHANNEL(12)
   INFINITY_ADC_CHANNEL(13)
   INFINITY_ADC_CHANNEL(14)
   INFINITY_ADC_CHANNEL(15)
   INFINITY_ADC_CHANNEL(16)
   INFINITY_ADC_CHANNEL(17)
   INFINITY_ADC_CHANNEL(18)

}}}

#endif
