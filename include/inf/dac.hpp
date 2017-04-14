/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016)
#define CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016

#include "stm32f4xx.h"
#include <algorithm>

#if !defined(DAC)
#error "Error: Target device does not have a DAC."
#endif

namespace cycfi { namespace infinity
{
   namespace detail
   {
      constexpr uint16_t dac_gpio(std::size_t ch)
      {
         return (ch == 0) ? GPIO_PIN_4 : GPIO_PIN_5;
      }

      constexpr uint32_t dac_channel(std::size_t ch)
      {
         return (ch == 0) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
      }

      constexpr uint16_t dac_channel_enable(std::size_t ch)
      {
         return (ch == 0) ? DAC_CR_EN1 : DAC_CR_EN2;
      }

      inline auto& dac_port(std::size_t ch)
      {
         return (ch == 0) ? DAC->DHR12R1 : DAC->DHR12R2;
      }
   }

   template <std::size_t Channel>
   class dac
   {
   public:

      static_assert(Channel >=0 && Channel <= 1, "Invalid DAC Channel");

      dac()
      {
         // Enable peripherals and GPIO Clocks
         __HAL_RCC_DAC_CLK_ENABLE();
         __HAL_RCC_GPIOA_CLK_ENABLE();

         // Configure peripheral GPIO
         GPIO_InitTypeDef init;
         init.Pin = detail::dac_gpio(Channel);
         init.Mode = GPIO_MODE_ANALOG;
         init.Pull = GPIO_NOPULL;
         HAL_GPIO_Init(GPIOA, &init);

         // Set DAC options
         h.Instance = DAC;

         // Set channel settings
         DAC_ChannelConfTypeDef ch_conf;
         ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;
         ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

         // Initialize DAC
         HAL_DAC_Init(&h);

         // Initialize and enable DAC
         HAL_DAC_ConfigChannel(&h, &ch_conf, detail::dac_channel(Channel));
         DAC->CR |= detail::dac_channel_enable(Channel);
      }

      void operator()(uint16_t val)
      {
         val = std::max<uint16_t>(val, 4095);
         detail::dac_port(Channel) = val;
         HAL_DAC_SetValue(&h, detail::dac_channel(Channel), DAC_ALIGN_12B_R, val);
      }

   private:

      DAC_HandleTypeDef h;
   };
}}

#endif
