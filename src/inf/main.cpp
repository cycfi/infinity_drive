#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_it.h"
#include "system_stm32f4xx.h"
#include "misc.h"

// The main start function entry point
void start();

int main()
{
   // Use SysTick as reference for the delay loops.
   // SysTick for every 1-ms
   constexpr uint32_t FREQUENCY_HZ = 1000u;
   SysTick_Config(SystemCoreClock / FREQUENCY_HZ);

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

   start();
   return 0;
}

extern "C"
{
   volatile uint32_t timer_delay;

   void SysTick_Handler(void)
   {
      if (timer_delay != 0u)
         timer_delay--;
   }
}
