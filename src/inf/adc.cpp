/*=============================================================================
   Copyright � 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/detail/adc_impl.hpp>

namespace cycfi { namespace infinity { namespace detail
{
   // We use DMA1 Channels 1 to 3 for the ADC
   auto const dma = DMA1;
   //auto const dma_channel = LL_DMA_CHANNEL_1;
   //auto const dma_channel_irq = DMA1_Channel1_IRQn;

   void adc_dma_config(
      ADC_TypeDef* adc_n,
      uint32_t dma_channel,
      IRQn_Type dma_channel_irq,
      uint16_t values[],
      uint16_t size
   )
   {
      // Configuration of NVIC
      // Configure NVIC to enable DMA interruptions
      NVIC_SetPriority(dma_channel_irq, 1);  // DMA IRQ lower priority than ADC IRQ
      NVIC_EnableIRQ(dma_channel_irq);

      // Configuration of DMA
      // Enable the peripheral clock of DMA
      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

      // Configure the DMA transfer
      //    - DMA transfer in circular mode to match with ADC configuration:
      //      DMA unlimited requests.
      //    - DMA transfer from ADC without address increment.
      //    - DMA transfer to memory with address increment.
      //    - DMA transfer from ADC by half-word to match with ADC configuration:
      //      ADC resolution 12 bits.
      //    - DMA transfer to memory by half-word to match with ADC conversion data
      //      buffer variable type: half-word.

      LL_DMA_ConfigTransfer(
         dma,
         dma_channel,
         LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
         LL_DMA_MODE_CIRCULAR              |
         LL_DMA_PERIPH_NOINCREMENT         |
         LL_DMA_MEMORY_INCREMENT           |
         LL_DMA_PDATAALIGN_HALFWORD        |
         LL_DMA_MDATAALIGN_HALFWORD        |
         LL_DMA_PRIORITY_HIGH
      );

      // Select ADC as DMA transfer request
      LL_DMA_SetPeriphRequest(dma, dma_channel, LL_DMA_REQUEST_0);

      // Set DMA transfer addresses of source and destination
      LL_DMA_ConfigAddresses(
         dma,
         dma_channel,
         LL_ADC_DMA_GetRegAddr(adc_n, LL_ADC_DMA_REG_REGULAR_DATA),
         reinterpret_cast<uint32_t>(values),
         LL_DMA_DIRECTION_PERIPH_TO_MEMORY
      );

      // Set DMA transfer size
      LL_DMA_SetDataLength(dma, dma_channel, size);

      // Enable DMA transfer interruption: transfer complete
      LL_DMA_EnableIT_TC(dma, dma_channel);

      // Enable DMA transfer interruption: transfer error
      LL_DMA_EnableIT_TE(dma, dma_channel);

      // Activation of DMA
      // Enable the DMA transfer
      LL_DMA_EnableChannel(dma, dma_channel);
   }

   // adc_peripheral_id, e.g. LL_AHB2_GRP1_PERIPH_GPIOA
   // port, e.g. GPIOA
   // mask, e.g. LL_GPIO_PIN_4
   // adc_irq_id, e.g. ADC1_2_IRQn

   void adc_config(
      ADC_TypeDef* adc,
      uint32_t adc_peripheral_id,
      GPIO_TypeDef* port, uint32_t mask,
      IRQn_Type adc_irq_id
   )
   {
      // Enable GPIO Clock
      LL_AHB2_GRP1_EnableClock(adc_peripheral_id);

      // Configure GPIO in analog mode to be used as ADC input
      LL_GPIO_SetPinMode(port, mask, LL_GPIO_MODE_ANALOG);

      // Connect GPIO analog switch to ADC input
      LL_GPIO_EnablePinAnalogControl(port, mask);

      // Configuration of NVIC
      // Configure NVIC to enable ADC1 interruptions
      NVIC_SetPriority(adc_irq_id, 0); // ADC IRQ greater priority than DMA IRQ
      NVIC_EnableIRQ(adc_irq_id);

      // Enable ADC clock (core clock)
      LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

      // Set ADC clock (conversion clock) common to several ADC instances
      LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(adc), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

      // ADC must be disabled at this point
      if (LL_ADC_IsEnabled(adc) != 0)
         error_handler();

      // Set ADC data resolution
      LL_ADC_SetResolution(adc, LL_ADC_RESOLUTION_12B);

      // Set ADC conversion data alignment
      LL_ADC_SetResolution(adc, LL_ADC_DATA_ALIGN_RIGHT);

      // Set ADC low power mode
      // LL_ADC_SetLowPowerMode(adc, LL_ADC_LP_MODE_NONE);

      // Set ADC selected offset number: channel and offset level
      // LL_ADC_SetOffset(adc, LL_ADC_OFFSET_1, LL_ADC_CHANNEL_9, 0x000);

      // ADC conversion should not be ongoing at this point
      if (LL_ADC_REG_IsConversionOngoing(adc) != 0)
         error_handler();

      // Set ADC group regular trigger source
      LL_ADC_REG_SetTriggerSource(adc, LL_ADC_REG_TRIG_SOFTWARE);

      // Set ADC group regular trigger polarity
      // LL_ADC_REG_SetTriggerEdge(adc, LL_ADC_REG_TRIG_EXT_RISING);

      // Set ADC group regular continuous mode
      LL_ADC_REG_SetContinuousMode(adc, LL_ADC_REG_CONV_CONTINUOUS);

      // Set ADC group regular conversion data transfer */
      LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

      // Set ADC group regular overrun behavior
      LL_ADC_REG_SetOverrun(adc, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

      // Set ADC group regular sequencer length and scan direction
      LL_ADC_REG_SetSequencerLength(adc, LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS);

      // Set ADC group regular sequencer discontinuous mode
      // LL_ADC_REG_SetSequencerDiscont(adc, LL_ADC_REG_SEQ_DISCONT_DISABLE);

      // Set ADC group regular sequence: channel on the selected sequence rank.
      LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_9);
      LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_VREFINT);
      LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_TEMPSENSOR);

      LL_ADC_SetChannelSamplingTime(adc, LL_ADC_CHANNEL_9, LL_ADC_SAMPLINGTIME_47CYCLES_5);
      LL_ADC_SetChannelSamplingTime(adc, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_247CYCLES_5);
      LL_ADC_SetChannelSamplingTime(adc, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_247CYCLES_5);

      // Configuration of ADC interruptions
      // Enable interruption ADC group regular end of sequence conversions
      LL_ADC_EnableIT_EOS(adc);

      // Enable interruption ADC group regular overrun
      LL_ADC_EnableIT_OVR(adc);
   }

   void activate_adc(ADC_TypeDef* adc)
   {
      // ADC must be disabled at this point
      if (LL_ADC_IsEnabled(adc) != 0)
         error_handler();

      // Disable ADC deep power down (enabled by default after reset state)
      LL_ADC_DisableDeepPowerDown(adc);

      // Enable ADC internal voltage regulator
      LL_ADC_EnableInternalRegulator(adc);

      // Delay for ADC internal voltage regulator stabilization.
      // Compute number of CPU cycles to wait for, from delay in us.
      //
      // Note: Variable divided by 2 to compensate partially
      //       CPU processing cycles (depends on compilation optimization).
      //
      // Note: If system core clock frequency is below 200kHz, wait time
      //       is only a few CPU processing cycles.

      auto wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
      while (wait_loop_index != 0)
         wait_loop_index--;

      // Run ADC self calibration
      uint32_t const adc_calibration_timout_ms = 1;
      auto timeout = adc_calibration_timout_ms;

      LL_ADC_StartCalibration(adc, LL_ADC_SINGLE_ENDED);
      while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
      {
         // Check Systick counter flag to decrement the time-out value
         if (LL_SYSTICK_IsActiveCounterFlag())
         {
            if (timeout-- == 0)
               error_handler();
         }
      }

      // Delay between ADC end of calibration and ADC enable.
      // Note: Variable divided by 2 to compensate partially
      //       CPU processing cycles (depends on compilation optimization).

      auto const adc_delay_calib_enable_cpu_cycles = LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32;
      wait_loop_index = adc_delay_calib_enable_cpu_cycles >> 1;
      while (wait_loop_index != 0)
         wait_loop_index--;

      // Enable ADC
      LL_ADC_Enable(adc);

      // Poll for ADC ready to convert
      uint32_t const adc_enable_timeout_ms = 1;
      timeout = adc_enable_timeout_ms;

      while (LL_ADC_IsActiveFlag_ADRDY(adc) == 0)
      {
         // Check Systick counter flag to decrement the time-out value
         if (LL_SYSTICK_IsActiveCounterFlag())
         {
            if (timeout-- == 0)
               error_handler();
         }
      }
   }
}}}
