/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_IMPL_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_IMPL_HPP_DECEMBER_21_2015

#include "stm32l4xx.h"
#include <cstdint>

namespace cycfi { namespace infinity { namespace detail
{
   constexpr bool check_valid_timer(std::size_t N)
   {
      return (N >= 1 && N <= 8) || (N >= 15 && N <= 17);
   }

   template <std::size_t N>
   TIM_TypeDef& get_timer();

   template <std::size_t N>
   int timer_irqn();

   template <std::size_t N>
   uint32_t timer_periph_id();

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

#define INFINITY_TIMER_PERIPH_ID(N, ID)                                        \
   template <>                                                                 \
   constexpr inline uint32_t timer_periph_id<N>()                              \
   {                                                                           \
      return ID;                                                               \
   }                                                                           \
   /***/


#if defined(TIM1)
 INFINITY_TIMER(1, TIM1)
 INFINITY_TIMER_IRQN(1, TIM1_UP_TIM16_IRQn)
 INFINITY_TIMER_PERIPH_ID(1, LL_APB2_GRP1_PERIPH_TIM1)
#endif

#if defined(TIM2)
 INFINITY_TIMER(2, TIM2)
 INFINITY_TIMER_IRQN(2, TIM2_IRQn)
 INFINITY_TIMER_PERIPH_ID(2, LL_APB1_GRP1_PERIPH_TIM2)
#endif

#if defined(TIM3)
 INFINITY_TIMER(3, TIM3)
 INFINITY_TIMER_IRQN(3, TIM3_IRQn)
 INFINITY_TIMER_PERIPH_ID(3, LL_APB1_GRP1_PERIPH_TIM3)
#endif

#if defined(TIM4)
 INFINITY_TIMER(4, TIM4)
 INFINITY_TIMER_IRQN(4, TIM4_IRQn)
 INFINITY_TIMER_PERIPH_ID(4, LL_APB1_GRP1_PERIPH_TIM4)
#endif

#if defined(TIM5)
 INFINITY_TIMER(5, TIM5)
 INFINITY_TIMER_IRQN(5, TIM5_IRQn)
 INFINITY_TIMER_PERIPH_ID(5, LL_APB1_GRP1_PERIPH_TIM5)
#endif

#if defined(TIM6)
 INFINITY_TIMER(6, TIM6)
 INFINITY_TIMER_IRQN(6, -1)
 INFINITY_TIMER_PERIPH_ID(6, LL_APB1_GRP1_PERIPH_TIM6)
#endif

#if defined(TIM7)
 INFINITY_TIMER(7, TIM7)
 INFINITY_TIMER_IRQN(7, TIM7_IRQn)
 INFINITY_TIMER_PERIPH_ID(7, LL_APB1_GRP1_PERIPH_TIM7)
#endif

#if defined(TIM8)
 INFINITY_TIMER(8, TIM8)
 INFINITY_TIMER_IRQN(8, TIM8_IRQn)
 INFINITY_TIMER_PERIPH_ID(8, LL_APB2_GRP1_PERIPH_TIM8)
#endif

#if defined(TIM15)
 INFINITY_TIMER(15, TIM15)
 INFINITY_TIMER_IRQN(15, TIM1_BRK_TIM15_IRQn)
 INFINITY_TIMER_PERIPH_ID(15, LL_APB2_GRP1_PERIPH_TIM15)
#endif

#if defined(TIM16)
 INFINITY_TIMER(16, TIM16)
 INFINITY_TIMER_IRQN(16, TIM1_UP_TIM16_IRQn)
 INFINITY_TIMER_PERIPH_ID(16, LL_APB2_GRP1_PERIPH_TIM16)
#endif

#if defined(TIM17)
 INFINITY_TIMER(17, TIM17)
 INFINITY_TIMER_IRQN(17, TIM1_TRG_COM_TIM17_IRQn)
 INFINITY_TIMER_PERIPH_ID(17, LL_APB2_GRP1_PERIPH_TIM17)
#endif

}}}

#endif
