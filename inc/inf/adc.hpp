/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015)
#define CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015

#include <inf/detail/pin_impl.hpp>
#include <inf/timer.hpp>
#include <inf/support.hpp>
#include <inf/detail/adc_impl.hpp>
#include <algorithm>
#include <array>
#include <utility>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // adc
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t id_
    , std::size_t channels_
    , std::size_t buffer_size_ = 8
   >
   class adc
   {
   public:

      static_assert(detail::valid_adc(id_), "Invalid ADC id");

      using adc_type = adc;
      using sample_group_type = std::array<uint16_t, channels_>;
      using buffer_type = std::array<sample_group_type, buffer_size_> ;
      using buffer_iterator_type = typename buffer_type::const_iterator;

      static constexpr std::size_t id = id_;
      static constexpr std::size_t channels = channels_;
      static constexpr std::size_t buffer_size = buffer_size_;
      static constexpr std::size_t resolution = 4096;

      template <std::size_t tid>
      adc(timer<tid>)
      {
         static_assert(detail::valid_adc_timer(tid), "Invalid Timer for ADC");

         detail::system_clock_config();

         detail::adc_dma_config(
            get_adc(),
            detail::adc_info<id>::dma_stream,
            detail::adc_info<id>::dma_channel,
            detail::adc_info<id>::dma_irq_id,
            &_data[0][0], buffer_size * channels
         );

         detail::adc_config(
            get_adc(),
            detail::adc_timer_trigger_id<tid>(),
            detail::adc_info<id>::periph_id,
            channels
         );

         detail::activate_adc(get_adc());

         // Set timer the trigger output (TRGO)
         LL_TIM_SetTriggerOutput(&detail::get_timer<tid>(), LL_TIM_TRGO_UPDATE);

         // Clear the ADC buffer
         clear();
      }

      void clear()
      {
         for (auto& buff : _data)
            buff.fill(0);
      }

      template <std::size_t... channels>
      void enable_channels()
      {
         static_assert(sizeof...(channels) == channels_,
            "Invalid number of channnels");
         enable_all_channels<1>(std::index_sequence<channels...>());
      }

      void start()
      {
         detail::start_adc(get_adc());
      }

      void stop()
      {
         detail::stop_adc(get_adc());
      }

      constexpr std::size_t size() { return buffer_size; }
      constexpr std::size_t num_channels() { return channels; }

      buffer_iterator_type begin() const { return _data.begin(); }
      buffer_iterator_type middle() const { return _data.begin() + (buffer_size / 2); }
      buffer_iterator_type end() const { return _data.end(); }

      sample_group_type& operator[](std::size_t i) { return _data[i]; }
      sample_group_type const& operator[](std::size_t i) const { return _data[i]; }

   private:

      template <std::size_t channel, std::size_t rank>
      void enable_one_channel()
      {
         static_assert(detail::valid_adc_channel(channel), "Invalid ADC Channel");

         static constexpr std::size_t pin = detail::get_adc_pin<channel>(id);
         static constexpr uint16_t bit = pin % 16;
         static constexpr uint16_t port = pin / 16;
         static constexpr uint32_t mask = 1 << bit;

         auto* gpio = &detail::get_port<port>();

         // Enable GPIO peripheral clock
         detail::enable_port_clock<port>();

         // Configure GPIO in analog mode to be used as ADC input
         LL_GPIO_SetPinMode(gpio, mask, LL_GPIO_MODE_ANALOG);

         // Enable the ADC channel on the selected sequence rank.
         detail::enable_adc_channel(
            get_adc(), detail::adc_channel<channel>(), detail::adc_rank<rank>());
      }

      template <std::size_t rank>
      void enable_all_channels(std::index_sequence<>)
      {
         // end recursion
      }

      template <std::size_t rank, std::size_t channel, std::size_t... rest>
      void enable_all_channels(std::index_sequence<channel, rest...>)
      {
         enable_one_channel<channel, rank>();
         enable_all_channels<rank + 1>(std::index_sequence<rest...>{});
      }

      ADC_TypeDef* get_adc() const
      {
         return detail::adc_info<id>::adc;
      }

      buffer_type _data;
   };
}}

#endif
