/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_MAY_1_2017)
#define CYCFI_INFINITY_ADC_HPP_MAY_1_2017

#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_adc.h>
#include <stm32l4xx_ll_dma.h>
#include <stm32l4xx_ll_gpio.h>

#include <inf/pin.hpp>
#include <type_traits>

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
   
   // Check if id is a valid adc.
   constexpr bool valid_adc(std::size_t id)
   {
      return id >=1 && id <= 3;
   }

   // Check if id is a valid timer for the adc.
   constexpr bool valid_adc_timer(std::size_t id)
   {
      return id == 1 || id == 2 || id == 3 || id == 4 || id == 6 || id == 8 || id == 15;
   }

   // Check if channel is a valid adc channel.
   constexpr bool valid_adc_channel(std::size_t channel)
   {
      return channel >= 1 && channel <= 18;
   }
   
   // Check if pin is a valid input port for adc, given id and channel.
   // Defaults to returning false, with specializations for each valid
   // configuration.
   template <std::size_t channel>
   constexpr bool valid_adc_pin(std::size_t id, std::size_t pin)
   {
      return false;
   }

   template <>
   constexpr bool valid_adc_pin<1>(std::size_t id, std::size_t pin)
   {
      return pin == port::portc + 0;
   }
   
   template <>
   constexpr bool valid_adc_pin<2>(std::size_t id, std::size_t pin)
   {
      return pin == port::portc + 1;
   }
   
   template <>
   constexpr bool valid_adc_pin<3>(std::size_t id, std::size_t pin)
   {
      return pin == port::portc + 2;
   }
   
   template <>
   constexpr bool valid_adc_pin<4>(std::size_t id, std::size_t pin)
   {
      return pin == port::portc + 3;
   }
   
   template <>
   constexpr bool valid_adc_pin<5>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 0;
   }
   
   template <>
   constexpr bool valid_adc_pin<6>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 1;
   }
   
   template <>
   constexpr bool valid_adc_pin<7>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 2;
   }
      
   template <>
   constexpr bool valid_adc_pin<8>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 3;
   }
      
   template <>
   constexpr bool valid_adc_pin<9>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 4;
   }
      
   template <>
   constexpr bool valid_adc_pin<10>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 5;
   }      
   
   template <>
   constexpr bool valid_adc_pin<11>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 6;
   }
   
   template <>
   constexpr bool valid_adc_pin<12>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::porta + 7;
   }
   
   template <>
   constexpr bool valid_adc_pin<13>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::portc + 4;
   }
   
   template <>
   constexpr bool valid_adc_pin<14>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::portc + 5;
   }
   
   template <>
   constexpr bool valid_adc_pin<15>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::portb + 0;
   }
   
   template <>
   constexpr bool valid_adc_pin<16>(std::size_t id, std::size_t pin)
   {
      return (id == 1 || id == 2) && pin == port::portb + 1;
   }
   ////////////////////////////////////////////////////////////////////////////
   // The ADCs: We provide template functions for getting the memory mapped
   // adcs given a constant id. That way, we can use generic programming.
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t id>
   ADC_TypeDef* get_adc();

   template <std::size_t id>
   struct adc_info;

   template <std::size_t id>
   uint32_t adc_timer_trigger_id();

   template <std::size_t id>
   uint32_t adc_rank();

   template <std::size_t id>
   uint32_t adc_channel();

#define INFINITY_ADC(id)                                                       \
                                                                               \
   template <>                                                                 \
   inline ADC_TypeDef* get_adc<id>()                                           \
   {                                                                           \
      return ADC##id;                                                          \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   struct adc_info<id>                                                         \
   {                                                                           \
      static constexpr IRQn_Type dma_irq_id = DMA1_Channel##id##_IRQn;         \
      static constexpr uint32_t dma_channel = LL_DMA_CHANNEL_##id;             \
   };                                                                          \
   /***/

#define INFINITY_ADC_TIMER_TRIGGER(id)                                         \
   template <>                                                                 \
   inline uint32_t adc_timer_trigger_id<id>()                                  \
   {                                                                           \
      return LL_ADC_REG_TRIG_EXT_TIM##id##_TRGO;                               \
   }                                                                           \
   /***/

#define INFINITY_ADC_RANK(id)                                                  \
   template <>                                                                 \
   inline uint32_t adc_rank<id>()                                              \
   {                                                                           \
      return LL_ADC_REG_RANK_##id;                                             \
   }                                                                           \
   /***/

#define INFINITY_ADC_CHANNEL(id)                                               \
   template <>                                                                 \
   inline uint32_t adc_channel<id>()                                           \
   {                                                                           \
      return LL_ADC_CHANNEL_##id;                                              \
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
