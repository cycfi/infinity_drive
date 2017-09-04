/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PIN_IMPL_HPP_DECEMBER_20_2015)
#define CYCFI_INFINITY_PIN_IMPL_HPP_DECEMBER_20_2015

#include <cstddef>
#include <cstdint>
#include <stm32f4xx.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_it.h>

namespace cycfi { namespace infinity { namespace detail
{
   ////////////////////////////////////////////////////////////////////////////
   // The ports: We provide template functions for getting the memory mapped
   // ports given a constant N. That way, we can use generic programming.
   //
   // This implementation is for the STM32F4 series.
   ////////////////////////////////////////////////////////////////////////////

   template <std::size_t port>
   GPIO_TypeDef& get_port();

   template <std::size_t port>
   void enable_port_clock();

   template <std::size_t const bit>
   constexpr std::uint32_t exti_id()
   {
      switch (bit)
      {
         case 0:  return LL_SYSCFG_EXTI_LINE0;
         case 1:  return LL_SYSCFG_EXTI_LINE1;
         case 2:  return LL_SYSCFG_EXTI_LINE2;
         case 3:  return LL_SYSCFG_EXTI_LINE3;
         case 4:  return LL_SYSCFG_EXTI_LINE4;
         case 5:  return LL_SYSCFG_EXTI_LINE5;
         case 6:  return LL_SYSCFG_EXTI_LINE6;
         case 7:  return LL_SYSCFG_EXTI_LINE7;
         case 8:  return LL_SYSCFG_EXTI_LINE8;
         case 9:  return LL_SYSCFG_EXTI_LINE9;
         case 10: return LL_SYSCFG_EXTI_LINE10;
         case 11: return LL_SYSCFG_EXTI_LINE11;
         case 12: return LL_SYSCFG_EXTI_LINE12;
         case 13: return LL_SYSCFG_EXTI_LINE13;
         case 14: return LL_SYSCFG_EXTI_LINE14;
         case 15: return LL_SYSCFG_EXTI_LINE15;
      }
   }

   template <std::size_t const bit>
   constexpr std::uint32_t exti_src()
   {
      switch (bit)
      {
         case 0:  return LL_EXTI_LINE_0;
         case 1:  return LL_EXTI_LINE_1;
         case 2:  return LL_EXTI_LINE_2;
         case 3:  return LL_EXTI_LINE_3;
         case 4:  return LL_EXTI_LINE_4;
         case 5:  return LL_EXTI_LINE_5;
         case 6:  return LL_EXTI_LINE_6;
         case 7:  return LL_EXTI_LINE_7;
         case 8:  return LL_EXTI_LINE_8;
         case 9:  return LL_EXTI_LINE_9;
         case 10: return LL_EXTI_LINE_10;
         case 11: return LL_EXTI_LINE_11;
         case 12: return LL_EXTI_LINE_12;
         case 13: return LL_EXTI_LINE_13;
         case 14: return LL_EXTI_LINE_14;
         case 15: return LL_EXTI_LINE_15;
      }
   }

   template <std::size_t const bit>
   constexpr IRQn_Type exti_irq()
   {
      switch (bit)
      {
         case 0:  return EXTI0_IRQn;
         case 1:  return EXTI1_IRQn;
         case 2:  return EXTI2_IRQn;
         case 3:  return EXTI3_IRQn;
         case 4:  return EXTI4_IRQn;
         case 5:  return EXTI9_5_IRQn;
         case 6:  return EXTI9_5_IRQn;
         case 7:  return EXTI9_5_IRQn;
         case 8:  return EXTI9_5_IRQn;
         case 9:  return EXTI9_5_IRQn;
         case 10: return EXTI15_10_IRQn;
         case 11: return EXTI15_10_IRQn;
         case 12: return EXTI15_10_IRQn;
         case 13: return EXTI15_10_IRQn;
         case 14: return EXTI15_10_IRQn;
         case 15: return EXTI15_10_IRQn;
      }
   }

   template <std::size_t port>
   std::size_t exti_port();

#define INFINITY_IOPORT(N, PORT_NAME)                                          \
   template <>                                                                 \
   inline void enable_port_clock<N>()                                          \
   {                                                                           \
      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIO##PORT_NAME);           \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline GPIO_TypeDef& get_port<N>()                                          \
   {                                                                           \
      return *GPIO##PORT_NAME;                                                 \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   constexpr std::size_t exti_port<N>()                                        \
   {                                                                           \
      return LL_SYSCFG_EXTI_PORT##PORT_NAME;                                   \
   }                                                                           \
   /***/

#ifdef GPIOA
  INFINITY_IOPORT(0, A)
#endif
#ifdef GPIOB
  INFINITY_IOPORT(1, B)
#endif
#ifdef GPIOC
  INFINITY_IOPORT(2, C)
#endif
#ifdef GPIOD
  INFINITY_IOPORT(3, D)
#endif
#ifdef GPIOE
  INFINITY_IOPORT(4, E)
#endif
#ifdef GPIOF
  INFINITY_IOPORT(5, F)
#endif
#ifdef GPIOG
  INFINITY_IOPORT(6, G)
#endif
#ifdef GPIOH
  INFINITY_IOPORT(7, H)
#endif
#ifdef GPIOI
  INFINITY_IOPORT(8, I)
#endif

}}}

#endif
