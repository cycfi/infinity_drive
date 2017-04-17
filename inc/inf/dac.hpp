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
   template <std::size_t Channel>
   class dac
   {
   public:

      static_assert(Channel >=0 && Channel <= 1, "Invalid DAC Channel");

      dac(uint16_t init_val = 2048)
      {
         // Enable peripherals and GPIO Clocks
         __HAL_RCC_DAC_CLK_ENABLE();
         __HAL_RCC_GPIOA_CLK_ENABLE();

         // Configure peripheral GPIO
         GPIO_InitTypeDef init;
         init.Pin = dac_gpio();
         init.Mode = GPIO_MODE_ANALOG;
         init.Pull = GPIO_NOPULL;
         HAL_GPIO_Init(GPIOA, &init);

         // Channel settings
         DAC_ChannelConfTypeDef ch_conf;
         ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;
         ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

         // Initialize DAC
         h.Instance = DAC;
         HAL_DAC_Init(&h);

         // Configure DAC channel
         HAL_DAC_ConfigChannel(&h, &ch_conf, dac_channel());

         // Start the DAC
         start();
         (*this)(init_val);
      }

      void start()
      {
         // Start the DAC
         HAL_DAC_Start(&h, dac_channel());
      }

      void stop()
      {
         // Stop the DAC
         HAL_DAC_Stop(&h, dac_channel());
      }

      void operator()(uint16_t val)
      {
         // Write a new value
         val = std::min<uint16_t>(val, 4095);
         HAL_DAC_SetValue(&h, dac_channel(), DAC_ALIGN_12B_R, val);
      }

   private:

      static constexpr uint16_t dac_gpio()
      {
         return (Channel == 0) ? GPIO_PIN_4 : GPIO_PIN_5;
      }

      static constexpr uint32_t dac_channel()
      {
         return (Channel == 0) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
      }

      DAC_HandleTypeDef h;
   };
}}

#endif
