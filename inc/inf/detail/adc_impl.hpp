/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015)
#define CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015

#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"

namespace cycfi { namespace infinity { namespace detail
{
   ////////////////////////////////////////////////////////////////////////////
   // The ADCs: We provide template functions for getting the memory mapped
	//	adcs given a constant N. That way, we can use generic programming.
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   ADC_TypeDef& get_adc();

   template <std::size_t N>
   struct adc_info;

#define INFINITY_ADC(N)                                                        \
                                                                               \
   template <>                                                                 \
   inline ADC_TypeDef& get_adc<N>()                                            \
   {                                                                           \
      return *ADC##N;                                                          \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   struct adc_info<N>                                                          \
   {                                                                           \
      static constexpr IRQn_Type dma_irq_id = DMA1_Channel##N##_IRQn;          \
      static constexpr uint32_t dma_channel = LL_DMA_CHANNEL_##N;              \
   };                                                                          \
   /***/

   INFINITY_ADC(1)
   INFINITY_ADC(2)
   INFINITY_ADC(3)

}}}

#endif
