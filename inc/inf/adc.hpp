/*=============================================================================
   Copyright � 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015)
#define CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015

#include "stm32l4xx.h"
#include <inf/detail/pin_impl.hpp>
#include <inf/timer.hpp>
//#include <inf/app.hpp>
#include <inf/detail/adc_impl.hpp>
#include <array>

namespace cycfi { namespace infinity
{
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
      using adc_type = adc;

      static_assert(id >=1 && id <= 3, "Invalid ADC id");

      typedef uint16_t sample_group_type[channels];
      typedef std::array<sample_group_type, buffer_size> buffer_type;
      typedef typename buffer_type::const_iterator buffer_iterator_type;

      template <std::size_t N>
      adc(timer<N>)
      {
         static_assert(detail::valid_adc_timer(N), "Invalid Timer for ADC");

         detail::adc_dma_config(
            get_adc(),
            detail::adc_info<id>::dma_channel,
            detail::adc_info<id>::dma_irq_id,
            &data[0][0], capacity
         );

         detail::adc_config(get_adc(), detail::adc_timer_trigger_id<N>());
      }

      template <std::size_t channel, std::size_t pin, std::size_t rank>
      void enable_channel()
      {
         static_assert(detail::valid_adc_channel(channel), "Invalid ADC Channel");

         static constexpr uint16_t bit = pin % 16;
         static constexpr uint16_t port = pin / 16;
         static constexpr uint32_t mask = 1 << bit;
         auto* gpio = &detail::get_port<port>();

         // Enable GPIO peripheral clock
         detail::enable_port_clock<port>();

         // Configure GPIO in analog mode to be used as ADC input
         LL_GPIO_SetPinMode(gpio, mask, LL_GPIO_MODE_ANALOG);

         // Connect GPIO analog switch to ADC input
         LL_GPIO_EnablePinAnalogControl(gpio, mask);

         // Set ADC group regular sequence: channel on the selected sequence rank.
         auto const adc_channel = detail::adc_channel<channel>();
         LL_ADC_REG_SetSequencerRanks(get_adc(), detail::adc_rank<rank>(), adc_channel);
         LL_ADC_SetChannelSamplingTime(get_adc(), adc_channel, LL_ADC_SAMPLINGTIME_2CYCLES_5);
      }

      void start()
      {
         detail::activate_adc(get_adc());
      }

      //void enable_interrupt(std::size_t priority = 0)
      //{
      //}

      constexpr std::size_t size() { return buffer_size; }
      constexpr std::size_t num_channels() { return channels; }

      buffer_iterator_type begin() const { return data.begin(); }
      buffer_iterator_type middle() const { return data.begin() + (buffer_size / 2); }
      buffer_iterator_type end() const { return data.end(); }

      sample_group_type& operator[](std::size_t i) { return data[i]; }
      sample_group_type const& operator[](std::size_t i) const { return data[i]; }

      ADC_TypeDef* get_adc() const
      {
         return detail::get_adc<id>();
      }

      buffer_type data;
   };
}}

#endif
