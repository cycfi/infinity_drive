/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/adc.hpp>
#include <inf/app.hpp>

namespace cycfi { namespace infinity { namespace detail
{
   void adc_dma_config(
      ADC_TypeDef& adc, DMA_Stream_TypeDef& dma, uint32_t dma_channel,
      volatile uint16_t values[], uint16_t size)
   {
      // Enable DMA clock
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

      // DMA configuration
      DMA_DeInit(&dma);
      DMA_InitTypeDef init;
      DMA_StructInit(&init);

      init.DMA_Channel = dma_channel;
      init.DMA_PeripheralBaseAddr = uint32_t(&adc.DR);
      init.DMA_Memory0BaseAddr = uint32_t(values);
      init.DMA_DIR = DMA_DIR_PeripheralToMemory;
      init.DMA_BufferSize = size;
      init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      init.DMA_MemoryInc = DMA_MemoryInc_Enable;
      init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
      init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
      init.DMA_Mode = DMA_Mode_Circular;
      init.DMA_Priority = DMA_Priority_High;

      init.DMA_FIFOMode = DMA_FIFOMode_Enable;
      init.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
      init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
      init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
      DMA_Init(&dma, &init);

      // Enable DMA
      DMA_Cmd(&dma, ENABLE);
   }

   void adc_config(
      ADC_TypeDef& adc, uint32_t adc_peripheral_id,
      uint32_t timer_trigger_id, uint16_t size)
   {
      // Enable ADC clock
      RCC_APB2PeriphClockCmd(adc_peripheral_id, ENABLE);

      static bool first_time = true;
      if (first_time) // do this only once
      {
         // ADC common configuration
         ADC_CommonInitTypeDef init;
         ADC_CommonStructInit(&init);
         init.ADC_Prescaler = ADC_Prescaler_Div2;
         init.ADC_Mode = ADC_Mode_Independent;
         init.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
         init.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
         ADC_CommonInit(&init);
         first_time = false;
      }

      {
         // ADC configuration
         ADC_InitTypeDef init;
         ADC_StructInit(&init);

         init.ADC_Resolution = ADC_Resolution_12b;
         init.ADC_ScanConvMode = ENABLE;
         init.ADC_ContinuousConvMode = DISABLE; // Conversions Triggered
         init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
         init.ADC_ExternalTrigConv = timer_trigger_id;
         init.ADC_DataAlign = ADC_DataAlign_Right;
         init.ADC_NbrOfConversion = size;
         ADC_Init(&adc, &init);
      }

      // Connect Timer 8 as clock source
      TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
   }
}}}
