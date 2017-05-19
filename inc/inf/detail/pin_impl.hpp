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

#define INFINITY_IOPORT(N, PORT_NAME)                                          \
   template <>                                                                 \
   inline void enable_port_clock<N>()                                          \
   {                                                                           \
      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_##PORT_NAME);               \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline GPIO_TypeDef& get_port<N>()                                          \
   {                                                                           \
      return *PORT_NAME;                                                       \
   }                                                                           \
   /***/


#ifdef GPIOA
  INFINITY_IOPORT(0, GPIOA)
#endif
#ifdef GPIOB
  INFINITY_IOPORT(1, GPIOB)
#endif
#ifdef GPIOC
  INFINITY_IOPORT(2, GPIOC)
#endif
#ifdef GPIOD
  INFINITY_IOPORT(3, GPIOD)
#endif
#ifdef GPIOE
  INFINITY_IOPORT(4, GPIOE)
#endif
#ifdef GPIOF
  INFINITY_IOPORT(5, GPIOF)
#endif
#ifdef GPIOG
  INFINITY_IOPORT(6, GPIOG)
#endif
#ifdef GPIOH
  INFINITY_IOPORT(7, GPIOH)
#endif
#ifdef GPIOI
  INFINITY_IOPORT(8, GPIOI)
#endif

}}}

#endif
