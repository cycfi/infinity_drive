/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015)
#define CYCFI_INFINITY_ADC_HPP_DECEMBER_31_2015

#include "stm32l4xx.h"
//#include <inf/pin.hpp>
#include <inf/timer.hpp>
//#include <inf/app.hpp>
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
      typedef adc adc_type;

      //static_assert(buffer_size >= 4, "Invalid buffer size");
      static_assert(id >=1 && id <= 3, "Invalid ADC id");

      typedef uint16_t sample_group_type[channels];
      typedef std::array<sample_group_type, buffer_size> buffer_type;
      typedef typename buffer_type::const_iterator buffer_iterator_type;

      template <std::size_t N>
      adc(timer<N>)
      {
      }

      void enable()
      {
      }

//      template <std::size_t pin>
//      using adc_pin = input_pin<pin, port_input_mode::analog_mode>;

      template <std::size_t channel, std::size_t pin, std::size_t rank>
      void enable_channel()
      {
      }

      void enable_interrupt(std::size_t priority = 0)
      {
      }

      constexpr std::size_t size() { return buffer_size; }
      constexpr std::size_t num_channels() { return channels; }

      buffer_iterator_type begin() const { return data.begin(); }
      buffer_iterator_type middle() const { return data.begin() + (buffer_size / 2); }
      buffer_iterator_type end() const { return data.end(); }

      sample_group_type& operator[](std::size_t i) { return data[i]; }
      sample_group_type const& operator[](std::size_t i) const { return data[i]; }

      //ADC_TypeDef& ref() const
      //{
      //   return get_adc<id>();
      //}

      buffer_type data;
   };
}}

#endif
