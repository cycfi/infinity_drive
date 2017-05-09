/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/detail/adc_impl.hpp>

//#include "stm32l4xx_ll_rcc.h"
//#include "stm32l4xx_ll_tim.h"

namespace cycfi { namespace infinity { namespace detail
{
   // We use DMA1 Channels 1 to 3 for the ADC
   auto const dma = DMA1;
   //auto const dma_channel = LL_DMA_CHANNEL_1;
   //auto const dma_channel_irq = DMA1_Channel1_IRQn;
   


void Configure_ADC(void)
{
  /*## Configuration of GPIO used by ADC channels ############################*/

  /* Note: On this STM32 device, ADC1 channel 9 is mapped on GPIO pin PA.04 */

  /* Enable GPIO Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /* Configure GPIO in analog mode to be used as ADC input */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);

  /* Connect GPIO analog switch to ADC input */
  LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_4);

  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable ADC1 interruptions */
  NVIC_SetPriority(ADC1_2_IRQn, 0); /* ADC IRQ greater priority than DMA IRQ */
  NVIC_EnableIRQ(ADC1_2_IRQn);

  /*## Configuration of ADC ##################################################*/

  /*## Configuration of ADC hierarchical scope: common to several ADC ########*/

  /* Enable ADC clock (core clock) */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       All ADC instances of the ADC common group must be disabled.        */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE() == 0)
  {
    /* Note: Call of the functions below are commented because they are       */
    /*       useless in this example:                                         */
    /*       setting corresponding to default configuration from reset state. */

    /* Set ADC clock (conversion clock) common to several ADC instances */
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

    /* Set ADC measurement path to internal channels */
    // LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);


  /*## Configuration of ADC hierarchical scope: multimode ####################*/

    /* Set ADC multimode configuration */
    // LL_ADC_SetMultimode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_INDEPENDENT);

    /* Set ADC multimode DMA transfer */
    // LL_ADC_SetMultiDMATransfer(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_REG_DMA_EACH_ADC);

    /* Set ADC multimode: delay between 2 sampling phases */
    // LL_ADC_SetMultiTwoSamplingDelay(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE);

  }


  /*## Configuration of ADC hierarchical scope: ADC instance #################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Note: Call of the functions below are commented because they are       */
    /*       useless in this example:                                         */
    /*       setting corresponding to default configuration from reset state. */

    /* Set ADC data resolution */
    // LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    /* Set ADC conversion data alignment */
    // LL_ADC_SetResolution(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    /* Set ADC low power mode */
    // LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    /* Set ADC selected offset number: channel and offset level */
    // LL_ADC_SetOffset(ADC1, LL_ADC_OFFSET_1, LL_ADC_CHANNEL_9, 0x000);

  }


  /*## Configuration of ADC hierarchical scope: ADC group regular ############*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled or enabled without conversion on going        */
  /*       on group regular.                                                  */
  if ((LL_ADC_IsEnabled(ADC1) == 0)               ||
      (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
  {
    /* Set ADC group regular trigger source */
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM2_TRGO);

    /* Set ADC group regular trigger polarity */
    // LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

    /* Set ADC group regular continuous mode */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    /* Set ADC group regular conversion data transfer */
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    /* Set ADC group regular overrun behavior */
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* Set ADC group regular sequencer */
    /* Note: On this STM32 serie, ADC group regular sequencer is              */
    /*       fully configurable: sequencer length and each rank               */
    /*       affectation to a channel are configurable.                       */
    /*       Refer to description of function                                 */
    /*       "LL_ADC_REG_SetSequencerLength()".                               */

    /* Set ADC group regular sequencer length and scan direction */
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);

    /* Set ADC group regular sequencer discontinuous mode */
    // LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

    /* Set ADC group regular sequence: channel on the selected sequence rank. */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_9);
  }


  /*## Configuration of ADC hierarchical scope: ADC group injected ###########*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled or enabled without conversion on going        */
  /*       on group injected.                                                 */
  if ((LL_ADC_IsEnabled(ADC1) == 0)               ||
      (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )
  {
    /* Note: Call of the functions below are commented because they are       */
    /*       useless in this example:                                         */
    /*       setting corresponding to default configuration from reset state. */

    /* Set ADC group injected trigger source */
    // LL_ADC_INJ_SetTriggerSource(ADC1, LL_ADC_INJ_TRIG_SOFTWARE);

    /* Set ADC group injected trigger polarity */
    // LL_ADC_INJ_SetTriggerEdge(ADC1, LL_ADC_INJ_TRIG_EXT_RISING);

    /* Set ADC group injected conversion trigger  */
    // LL_ADC_INJ_SetTrigAuto(ADC1, LL_ADC_INJ_TRIG_INDEPENDENT);

    /* Set ADC group injected contexts queue mode */
    /* Note: If ADC group injected contexts queue are enabled, configure      */
    /*       contexts using function "LL_ADC_INJ_ConfigQueueContext()".       */
    // LL_ADC_INJ_SetQueueMode(ADC1, LL_ADC_INJ_QUEUE_DISABLE);

    /* Set ADC group injected sequencer */
    /* Note: On this STM32 serie, ADC group injected sequencer is             */
    /*       fully configurable: sequencer length and each rank               */
    /*       affectation to a channel are configurable.                       */
    /*       Refer to description of function                                 */
    /*       "LL_ADC_INJ_SetSequencerLength()".                               */

    /* Set ADC group injected sequencer length and scan direction */
    // LL_ADC_INJ_SetSequencerLength(ADC1, LL_ADC_INJ_SEQ_SCAN_DISABLE);

    /* Set ADC group injected sequencer discontinuous mode */
    // LL_ADC_INJ_SetSequencerDiscont(ADC1, LL_ADC_INJ_SEQ_DISCONT_DISABLE);

    /* Set ADC group injected sequence: channel on the selected sequence rank. */
    // LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_9);
  }


  /*## Configuration of ADC hierarchical scope: channels #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled or enabled without conversion on going        */
  /*       on either groups regular or injected.                              */
  if ((LL_ADC_IsEnabled(ADC1) == 0)                    ||
      ((LL_ADC_REG_IsConversionOngoing(ADC1) == 0) &&
       (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )   )
  {
    /* Set ADC channels sampling time */
    /* Note: Considering interruption occurring after each number of          */
    /*       "ADC_CONVERTED_DATA_BUFFER_SIZE" ADC conversions                 */
    /*       (IT from DMA transfer complete),                                 */
    /*       select sampling time and ADC clock with sufficient               */
    /*       duration to not create an overhead situation in IRQHandler.      */
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_9, LL_ADC_SAMPLINGTIME_47CYCLES_5);

    /* Set mode single-ended or differential input of the selected            */
    /* ADC channel.                                                           */
    // LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_9, LL_ADC_SINGLE_ENDED);
  }


  /*## Configuration of ADC transversal scope: analog watchdog ###############*/

  /* Set ADC analog watchdog channels to be monitored */
  // LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_DISABLE);

  /* Set ADC analog watchdog thresholds */
  // LL_ADC_ConfigAnalogWDThresholds(ADC1, LL_ADC_AWD1, 0xFFF, 0x000);


  /*## Configuration of ADC transversal scope: oversampling ##################*/

  /* Set ADC oversampling scope */
  // LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

  /* Set ADC oversampling parameters */
  // LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_2, LL_ADC_OVS_SHIFT_NONE);


  /*## Configuration of ADC interruptions ####################################*/
  /* Enable interruption ADC group regular overrun */
  LL_ADC_EnableIT_OVR(ADC1);

  /* Note: in this example, ADC group regular end of conversions              */
  /*       (number of ADC conversions defined by DMA buffer size)             */
  /*       are notified by DMA transfer interruptions).                       */

}
   
void Activate_ADC(void)
{
  #define ADC_CALIBRATION_TIMEOUT_MS       ((uint32_t)   1)
  #define ADC_ENABLE_TIMEOUT_MS            ((uint32_t)   1)
  #define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)

  __IO uint32_t wait_loop_index = 0;
  #if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; /* Variable used for timeout management */
  #endif /* USE_TIMEOUT */

  /*## Operation on ADC hierarchical scope: ADC instance #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Disable ADC deep power down (enabled by default after reset state) */
    LL_ADC_DisableDeepPowerDown(ADC1);

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);

    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);

    /* Poll for ADC effectively calibrated */
    #if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
    #endif /* USE_TIMEOUT */

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
    #if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {
        /* Time-out occurred. Set LED to blinking mode */
        LED_Blinking(LED_BLINK_ERROR);
        }
      }
    #endif /* USE_TIMEOUT */
    }

    /* Delay between ADC end of calibration and ADC enable.                   */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* Poll for ADC ready to convert */
    #if (USE_TIMEOUT == 1)
    Timeout = ADC_ENABLE_TIMEOUT_MS;
    #endif /* USE_TIMEOUT */

    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
    #if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {
        /* Time-out occurred. Set LED to blinking mode */
        LED_Blinking(LED_BLINK_ERROR);
        }
      }
    #endif /* USE_TIMEOUT */
    }

    /* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
    /*       status afterwards.                                               */
    /*       This flag should be cleared at ADC Deactivation, before a new    */
    /*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
  }

  /*## Operation on ADC hierarchical scope: ADC group regular ################*/
  /* Note: No operation on ADC group regular performed here.                  */
  /*       ADC group regular conversions to be performed after this function  */
  /*       using function:                                                    */
  /*       "LL_ADC_REG_StartConversion();"                                    */

  /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
  /* Note: No operation on ADC group injected performed here.                 */
  /*       ADC group injected conversions to be performed after this function */
  /*       using function:                                                    */
  /*       "LL_ADC_INJ_StartConversion();"                                    */

}


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

      // Enable DMA transfer interruption: half transfer
      LL_DMA_EnableIT_HT(dma, dma_channel);

      // Enable DMA transfer interruption: transfer error
      LL_DMA_EnableIT_TE(dma, dma_channel);

      // Activation of DMA
      // Enable the DMA transfer
      LL_DMA_EnableChannel(dma, dma_channel);
   }

   // port, e.g. GPIOA
   // mask, e.g. LL_GPIO_PIN_4
   // timer_trigger_id, e.g. LL_ADC_REG_TRIG_EXT_TIM2_TRGO

   void adc_config(
      ADC_TypeDef* adc,
      uint32_t timer_trigger_id
   )
   {
      Configure_ADC();
   
   
//   
//      // Configuration of NVIC
//      // Configure NVIC to enable ADC1 interruptions
//      NVIC_SetPriority(ADC1_2_IRQn, 0); // ADC IRQ greater priority than DMA IRQ
//      NVIC_EnableIRQ(ADC1_2_IRQn);
//
//      // Enable ADC clock (core clock)
//      LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
//
//      // ADC common instance must not be enabled at this point
//      if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE() != 0)
//         error_handler();
//
//      // Set ADC clock (conversion clock) common to several ADC instances
//      LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(adc), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
//
//      // ADC must not be enabled at this point
//      if (LL_ADC_IsEnabled(adc) != 0)
//         error_handler();
//
//      // Set ADC data resolution
//      LL_ADC_SetResolution(adc, LL_ADC_RESOLUTION_12B);
//
//      // Set ADC conversion data alignment
//      LL_ADC_SetResolution(adc, LL_ADC_DATA_ALIGN_RIGHT);
//
//      // Set ADC low power mode
//      // LL_ADC_SetLowPowerMode(adc, LL_ADC_LP_MODE_NONE);
//
//      // Set ADC selected offset number: channel and offset level
//      // LL_ADC_SetOffset(adc, LL_ADC_OFFSET_1, LL_ADC_CHANNEL_9, 0x000);
//
//      // ADC conversion should not be ongoing at this point
//      if (LL_ADC_REG_IsConversionOngoing(adc) != 0)
//         error_handler();
//
//      // Set ADC group regular trigger source
//      LL_ADC_REG_SetTriggerSource(adc, timer_trigger_id);
//
//      // Set ADC group regular trigger polarity
//      // LL_ADC_REG_SetTriggerEdge(adc, LL_ADC_REG_TRIG_EXT_RISING);
//
//      // Set ADC group regular continuous mode
//      LL_ADC_REG_SetContinuousMode(adc, LL_ADC_REG_CONV_SINGLE);
//
//      // Set ADC group regular conversion data transfer */
//      LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
//
//      // Set ADC group regular overrun behavior
//      LL_ADC_REG_SetOverrun(adc, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
//
//      // Set ADC group regular sequencer length and scan direction
//      LL_ADC_REG_SetSequencerLength(adc, LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS);
//
//      // Set ADC group regular sequencer discontinuous mode
//      // LL_ADC_REG_SetSequencerDiscont(adc, LL_ADC_REG_SEQ_DISCONT_DISABLE);
//
//      // Configuration of ADC interruptions
//      // Enable interruption ADC group regular end of sequence conversions
//      LL_ADC_EnableIT_EOS(adc);
//
//      // Enable interruption ADC group regular overrun
//      LL_ADC_EnableIT_OVR(adc);
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
