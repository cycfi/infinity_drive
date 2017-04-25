/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015

#include <inf/pin.hpp>
#include <inf/support.hpp>
#include <inf/detail/timer_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // timer
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t N>
   struct timer
   {
      timer(uint32_t clock_frequency, uint32_t period)
      {
         static_assert(detail::check_valid_timer(N), "Invalid Timer N");

          // Enable the timer peripheral clock
         LL_APB2_GRP1_EnableClock(get_periph_id());

         // Set the pre-scaler value
         LL_TIM_SetPrescaler(get_timer(), __LL_TIM_CALC_PSC(SystemCoreClock, clock_frequency));

         // Set the auto-reload value to have an initial update event frequency
         // of clock_frequency/period
         auto freq = clock_frequency/period;
         auto autoreload = __LL_TIM_CALC_ARR(
            SystemCoreClock, LL_TIM_GetPrescaler(get_timer()), freq);

         LL_TIM_SetAutoReload(get_timer(), autoreload);
      }

      void enable_interrupt(std::size_t priority = 0)
      {
         // Enable the update interrupt
         LL_TIM_EnableIT_UPDATE(get_timer());

         // Configure the NVIC to handle timer update interrupt
         NVIC_SetPriority(get_irqn(), priority);
         NVIC_EnableIRQ(get_irqn());
      }

      void start()
      {
         // Enable counter
         LL_TIM_EnableCounter(get_timer());

         // Force update generation
         LL_TIM_GenerateEvent_UPDATE(get_timer());
      }

      void stop()
      {
         LL_TIM_DisableCounter(get_timer());
      }

   private:

      static constexpr TIM_TypeDef* get_timer()
      {
         return &detail::get_timer<N>();
      }

      static constexpr IRQn_Type get_irqn()
      {
         static_assert(detail::timer_irqn<N>() != -1, "Timer has no interrupt capability.");
         return IRQn_Type(detail::timer_irqn<N>());
      }

      static constexpr uint32_t get_periph_id()
      {
         return detail::timer_periph_id<N>();
      }
   };
}}

#endif
