/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016)
#define CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016

#include <inf/config.hpp>
#include <inf/pin.hpp>

#if defined(STM32F4)
# include <stm32f4xx.h>
# include <stm32f4xx_ll_dac.h>
#endif

#include <algorithm>

#if !defined(DAC)
#error "Error: Target device does not have a DAC."
#endif

namespace cycfi { namespace infinity
{
   template <std::size_t Channel>
   class dac
   {
      static constexpr uint16_t dac_gpio =
         (Channel == 0) ? LL_GPIO_PIN_4 : LL_GPIO_PIN_5;
      static constexpr std::uint32_t dac_channel =
         (Channel == 0) ? LL_DAC_CHANNEL_1 : LL_DAC_CHANNEL_2;

      using dac_peripheral_id = io_pin_id<
         port::porta+(Channel == 0) ? 4 : 5>
      ;

   public:

      static_assert(Channel >=0 && Channel <= 1, "Invalid DAC Channel");

      void init(uint16_t init_val = 2048)
      {
         detail::system_clock_config();

         // Enable GPIO Clock
         LL_AHB2_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

         // Configure GPIO in analog mode to be used as DAC output
         LL_GPIO_SetPinMode(GPIOA, dac_gpio, LL_GPIO_MODE_ANALOG);

         // Enable DAC clock
         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

         // Select trigger source
         LL_DAC_SetTriggerSource(DAC1, dac_channel, LL_DAC_TRIG_SOFTWARE);

         // Disable the output for the selected DAC channel
         LL_DAC_SetOutputBuffer(DAC1, dac_channel, LL_DAC_OUTPUT_BUFFER_ENABLE);

         // Disable DAC channel DMA request
         LL_DAC_DisableDMAReq(DAC1, dac_channel);

         // Set the data to be loaded in the data holding register
         LL_DAC_ConvertData12RightAligned(DAC1, dac_channel, 0x000);

         // Enable interruption DAC channel1 underrun
         LL_DAC_EnableIT_DMAUDR1(DAC1);

         // Start the DAC
         start();
         (*this)(init_val);
      }

      auto setup(uint16_t init_val = 2048)
      {
         init(init_val);
         return [](auto base)
         {
            return make_basic_config<dac_peripheral_id>(base);
         };
      }

      void start()
      {
         // Start the DAC
         // Enable DAC channel
         LL_DAC_Enable(DAC1, dac_channel);

         // Delay for DAC channel voltage settling time from DAC channel startup.
         // Compute number of CPU cycles to wait for, from delay in us.
         //
         // Note: Variable divided by 2 to compensate partially CPU processing cycles
         //       (depends on compilation optimization).
         // Note: If system core clock frequency is below 200kHz, wait time
         //       is only a few CPU processing cycles.

         auto delay = (LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US
               * (SystemCoreClock / (100000 * 2))) / 10;

         while (delay != 0)
            delay--;

         LL_DAC_EnableTrigger(DAC1, dac_channel);
      }

      void stop()
      {
         // Stop the DAC
         LL_DAC_Disable(DAC1, dac_channel);
      }

      void operator()(uint16_t val)
      {
         // Write a new value
         val = std::min<uint16_t>(val, 4095);

         // Set the data to be loaded in the data holding register
         LL_DAC_ConvertData12RightAligned(DAC1, dac_channel, val);

         // Trig DAC conversion by software
         LL_DAC_TrigSWConversion(DAC1, dac_channel);
      }
   };
}}

#endif
