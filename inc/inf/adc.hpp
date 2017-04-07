/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015)
#define CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015

#include "stm32f4xx.h"
#include <inf/pin.hpp>
#include <inf/timer.hpp>
#include <inf/app.hpp>
#include <array>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // implementation detail
   ////////////////////////////////////////////////////////////////////////////
   namespace detail
   {
      void adc_dma_config(
         ADC_TypeDef& adc, DMA_Stream_TypeDef& dma, uint32_t dma_channel,
         volatile uint16_t values[], uint16_t size);

      void adc_config(
         ADC_TypeDef& adc, uint32_t adc_peripheral_id,
         uint32_t timer_trigger_id, uint16_t size);
   }

   ////////////////////////////////////////////////////////////////////////////
   // The ADCs: We provide template functions for getting the memory mapped
	//	adcs given a constant N. That way, we can use generic programming.
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   ADC_TypeDef& get_adc();

   template <std::size_t N>
   DMA_Stream_TypeDef& get_adc_dma();

   template <std::size_t N>
   struct adc_info;

#define INFINITY_ADC(                                                          \
   N, ADC_NAME, PERIPHERAL_NAME, DMA_STREAM, DMA_IRQ, DMA_CHANNEL)             \
                                                                               \
   template <>                                                                 \
   inline ADC_TypeDef& get_adc<N>()                                            \
   {                                                                           \
      return *ADC_NAME;                                                        \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline DMA_Stream_TypeDef& get_adc_dma<N>()                                 \
   {                                                                           \
      return *DMA_STREAM;                                                      \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   struct adc_info<N>                                                          \
   {                                                                           \
      static constexpr uint32_t peripheral_id = PERIPHERAL_NAME;               \
      static constexpr uint32_t dma_irq_id = DMA_IRQ;                          \
      static constexpr uint32_t dma_channel = DMA_CHANNEL;                     \
   };                                                                          \
   /***/

INFINITY_ADC(
   1, ADC1, RCC_APB2Periph_ADC1, DMA2_Stream0, DMA2_Stream0_IRQn, DMA_Channel_0)

INFINITY_ADC(
   2, ADC2, RCC_APB2Periph_ADC2, DMA2_Stream2, DMA2_Stream2_IRQn, DMA_Channel_1)

INFINITY_ADC(
   3, ADC3, RCC_APB2Periph_ADC3, DMA2_Stream1, DMA2_Stream1_IRQn, DMA_Channel_2)

   ////////////////////////////////////////////////////////////////////////////
   // adc
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t id_, std::size_t channels_, std::size_t buffer_size_ = 8>
   struct adc
   {
      static constexpr std::size_t id = id_;
      static constexpr std::size_t channels = channels_;
      static constexpr std::size_t buffer_size = buffer_size_;
      static constexpr std::size_t capacity = buffer_size * channels;
      typedef adc adc_type;

      //static_assert(buffer_size >= 4, "Invalid buffer size");
      static_assert(id >=1 && id <= 3, "Invalid ADC id");

      typedef uint16_t sample_group_type[channels];
      typedef std::array<sample_group_type, buffer_size> buffer_type;
      typedef typename buffer_type::const_iterator buffer_iterator_type;

      template <std::size_t N>
      adc(timer<N>)
      {
         static_assert(N == 2 || N == 3 || N == 8, "Invalid Timer");

         detail::adc_dma_config(
            ref(), get_adc_dma<id>(), adc_info<id>::dma_channel,
            &data[0][0], capacity);

         detail::adc_config(
            ref(), adc_info<id>::peripheral_id,
            timer_info<N>::trigger_id, channels);
      }

      void enable()
      {
         // Enable the request after last transfer for DMA Circular mode
         ADC_DMARequestAfterLastTransferCmd(&ref(), ENABLE);

         // Enable ADC DMA
         ADC_DMACmd(&ref(), ENABLE);

         // Enable ADC1
         ADC_Cmd(&ref(), ENABLE);
      }

      template <std::size_t pin>
      using adc_pin = input_pin<pin, port_input_mode::analog_mode>;

      template <std::size_t channel, std::size_t pin, std::size_t rank>
      void enable_channel()
      {
         adc_pin<pin> adc_in;
         ADC_RegularChannelConfig(&ref(), channel, rank, ADC_SampleTime_3Cycles);
      }

      void enable_interrupt(std::size_t priority = 0)
      {
         // Enable DMA Stream Half / Transfer Complete interrupt
         DMA_ITConfig(&get_adc_dma<id>(), DMA_IT_TC | DMA_IT_HT, ENABLE);

         // Enable the timer global Interrupt
         NVIC_InitTypeDef init;
         init.NVIC_IRQChannel = adc_info<id>::dma_irq_id;
         init.NVIC_IRQChannelPreemptionPriority = priority;
         init.NVIC_IRQChannelSubPriority = 0;
         init.NVIC_IRQChannelCmd = ENABLE;
         NVIC_Init(&init);
      }

      constexpr std::size_t size() { return buffer_size; }
      constexpr std::size_t num_channels() { return channels; }

      buffer_iterator_type begin() const { return data.begin(); }
      buffer_iterator_type middle() const { return data.begin() + (buffer_size / 2); }
      buffer_iterator_type end() const { return data.end(); }

      sample_group_type& operator[](std::size_t i) { return data[i]; }
      sample_group_type const& operator[](std::size_t i) const { return data[i]; }

      ADC_TypeDef& ref() const
      {
         return get_adc<id>();
      }

      buffer_type data;
   };
}}

#endif
