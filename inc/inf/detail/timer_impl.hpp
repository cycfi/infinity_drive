/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_IMPL_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_IMPL_HPP_DECEMBER_21_2015

#include "stm32l4xx.h"

namespace cycfi { namespace infinity { namespace detail
{
   constexpr bool check_valid_timer(std::size_t N)
   {
      return (N >= 1 && N <= 17);
   }

   template <std::size_t N>
   TIM_TypeDef& get_timer();

   template <std::size_t N>
   int timer_irqn();

#define INFINITY_TIMER(N, TIMER_NAME)                                          \
   template <>                                                                 \
   constexpr inline TIM_TypeDef& get_timer<N>()                                \
   {                                                                           \
      return *TIMER_NAME;                                                      \
   }                                                                           \
   /***/

#define INFINITY_TIMER_IRQN(N, IRQN)                                           \
   template <>                                                                 \
   constexpr inline int timer_irqn<N>()                                        \
   {                                                                           \
      return IRQN;                                                             \
   }                                                                           \
   /***/


#if defined(TIM1)
 INFINITY_TIMER(1, TIM1)
 INFINITY_TIMER_IRQN(1, TIM1_UP_TIM16_IRQn)
#endif

#if defined(TIM2)
 INFINITY_TIMER(2, TIM2)
 INFINITY_TIMER_IRQN(2, TIM2_IRQn)
#endif

#if defined(TIM3)
 INFINITY_TIMER(3, TIM3)
 INFINITY_TIMER_IRQN(3, TIM3_IRQn)
#endif

#if defined(TIM4)
 INFINITY_TIMER(4, TIM4)
 INFINITY_TIMER_IRQN(4, TIM4_IRQn)
#endif

#if defined(TIM5)
 INFINITY_TIMER(5, TIM5)
 INFINITY_TIMER_IRQN(5, TIM5_IRQn)
#endif

#if defined(TIM6)
 INFINITY_TIMER(6, TIM6)
 INFINITY_TIMER_IRQN(6, -1)
#endif

#if defined(TIM7)
 INFINITY_TIMER(7, TIM7)
 INFINITY_TIMER_IRQN(7, TIM7_IRQn)
#endif

#if defined(TIM8)
 INFINITY_TIMER(8, TIM8)
 INFINITY_TIMER_IRQN(8, TIM8_IRQn)
#endif

#if defined(TIM9)
 INFINITY_TIMER(9, TIM9)
 INFINITY_TIMER_IRQN(9, -1)
#endif

#if defined(TIM10)
 INFINITY_TIMER(10, TIM10)
 INFINITY_TIMER_IRQN(10, -1)
#endif

#if defined(TIM11)
 INFINITY_TIMER(11, TIM11)
 INFINITY_TIMER_IRQN(11, -1)
#endif

#if defined(TIM12)
 INFINITY_TIMER(12, TIM12)
 INFINITY_TIMER_IRQN(12, -1)
#endif

#if defined(TIM13)
 INFINITY_TIMER(13, TIM13)
 INFINITY_TIMER_IRQN(13, -1)
#endif

#if defined(TIM14)
 INFINITY_TIMER(14, TIM14)
 INFINITY_TIMER_IRQN(14, -1)
#endif

#if defined(TIM15)
 INFINITY_TIMER(15, TIM15)
 INFINITY_TIMER_IRQN(15, TIM1_BRK_TIM15_IRQn)
#endif

#if defined(TIM16)
 INFINITY_TIMER(16, TIM16)
 INFINITY_TIMER_IRQN(16, TIM1_UP_TIM16_IRQn)
#endif

#if defined(TIM17)
 INFINITY_TIMER(17, TIM17)
 INFINITY_TIMER_IRQN(17, TIM1_TRG_COM_TIM17_IRQn)
#endif

}}}

#endif
