/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016)
#define CYCFI_INFINITY_DAC_HPP_FEBRUARY_6_2016

#include "stm32f4xx.h"

namespace cycfi { namespace infinity
{
   // This is a barebones DAC converter, for now only used for testing
   struct dac
   {
      dac()
      {
         // Enable clocks for port A and DAC
         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

         // Set up PA.5 as DAC channel 2 output
         GPIO_InitTypeDef GPIO_InitStructure;
         GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
         GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
         GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
         GPIO_Init(GPIOA, &GPIO_InitStructure);

         /* DAC channel 2 Configuration */
         DAC_InitTypeDef DAC_InitStructure;
         DAC_StructInit(&DAC_InitStructure);

         DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
         DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
         DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
         DAC_Init(DAC_Channel_2, &DAC_InitStructure);

         /* Enable DAC Channel 2 */
         DAC_Cmd(DAC_Channel_2, ENABLE);
      }

      void operator()(uint16_t val)
      {
         DAC_SetChannel2Data(DAC_Align_12b_R, val);
      }
   };
}}

#endif

