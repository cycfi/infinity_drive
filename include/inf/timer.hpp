/*=======================================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015)
#define CYCFI_INFINITY_TIMER_HPP_DECEMBER_21_2015

#include "stm32f4xx.h"
#include <cstdint>
#include <type_traits>

#include <inf/pin.hpp>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   


//   ////////////////////////////////////////////////////////////////////////////
//	template <std::size_t port>
//	TIM_TypeDef& get_timer();
//
//   template <std::size_t port>
//   struct timer_info;
//
//#define INFINITY_TIMER(N, TIMER_NAME)                                          \
//   template <>                                                                 \
//   inline TIM_TypeDef& get_timer<N>()                                          \
//   {                                                                           \
//      return *TIMER_NAME;                                                      \
//   }                                                                           \
//   /***/
//
//#define INFINITY_TIMER_INFO(                                                   \
//   N, SYS_CLOCK_DIV, IRQ, PERIPH, PERIPH_FUNCTION, PERIPH_GROUP, TRIGGER)      \
//   template <>                                                                 \
//   struct timer_info<N>                                                        \
//   {                                                                           \
//      static constexpr int sys_clock_div = SYS_CLOCK_DIV;                      \
//      static constexpr int irq_id = IRQ;                                       \
//      static constexpr uint32_t peripheral_id = PERIPH;                        \
//      static constexpr int peripheral_group = PERIPH_GROUP;                    \
//      static constexpr uint32_t trigger_id = TRIGGER;                          \
//   };                                                                          \
//   /***/
//
//#if defined(TIM1)
// INFINITY_TIMER(1, TIM1)
// INFINITY_TIMER_INFO(1, 1, -1, RCC_APB2Periph_TIM1, GPIO_AF_TIM1, 2, -1)
//#endif
//
//#if defined(TIM2)
// INFINITY_TIMER(2, TIM2)
// INFINITY_TIMER_INFO(
//   2, 2, TIM2_IRQn, RCC_APB1Periph_TIM2, GPIO_AF_TIM2,
//   1, ADC_ExternalTrigConv_T2_TRGO)
//#endif
//
//#if defined(TIM3)
// INFINITY_TIMER(3, TIM3)
// INFINITY_TIMER_INFO(
//   3, 2, TIM3_IRQn, RCC_APB1Periph_TIM3, GPIO_AF_TIM3,
//   1, ADC_ExternalTrigConv_T3_TRGO)
//#endif
//
//#if defined(TIM4)
// INFINITY_TIMER(4, TIM4)
// INFINITY_TIMER_INFO(4, 2, TIM4_IRQn, RCC_APB1Periph_TIM4, GPIO_AF_TIM4, 1, -1)
//#endif
//
//#if defined(TIM5)
// INFINITY_TIMER(5, TIM5)
// INFINITY_TIMER_INFO(5, 2, TIM5_IRQn, RCC_APB1Periph_TIM5, GPIO_AF_TIM5, 1, -1)
//#endif
//
//#if defined(TIM6)
// INFINITY_TIMER(6, TIM6)
// INFINITY_TIMER_INFO(6, 2, -1, RCC_APB1Periph_TIM6, GPIO_AF_TIM6, 1, -1)
//#endif
//
//#if defined(TIM7)
// INFINITY_TIMER(7, TIM7)
// INFINITY_TIMER_INFO(7, 2, TIM7_IRQn, RCC_APB1Periph_TIM7, GPIO_AF_TIM7, 1, -1)
//#endif
//
//#if defined(TIM8)
// INFINITY_TIMER(8, TIM8)
// INFINITY_TIMER_INFO(
//   8, 2, -1, RCC_APB2Periph_TIM8, GPIO_AF_TIM8,
//   2, ADC_ExternalTrigConv_T8_TRGO)
//#endif
//
//#if defined(TIM9)
// INFINITY_TIMER(9, TIM9)
// INFINITY_TIMER_INFO(9, 1, -1, RCC_APB2Periph_TIM9, GPIO_AF_TIM9, 2, -1)
//#endif
//
//#if defined(TIM10)
// INFINITY_TIMER(10, TIM10)
// INFINITY_TIMER_INFO(10, 1, -1, RCC_APB2Periph_TIM10, GPIO_AF_TIM10, 2, -1)
//#endif
//
//#if defined(TIM11)
// INFINITY_TIMER(11, TIM11)
// INFINITY_TIMER_INFO(11, 1, -1, RCC_APB2Periph_TIM11, GPIO_AF_TIM11, 2, -1)
//#endif
//
//#if defined(TIM12)
// INFINITY_TIMER(12, TIM12)
// INFINITY_TIMER_INFO(12, 2, -1, RCC_APB1Periph_TIM12, GPIO_AF_TIM12, 1, -1)
//#endif
//
//#if defined(TIM13)
// INFINITY_TIMER(13, TIM13)
// INFINITY_TIMER_INFO(13, 2, -1, RCC_APB1Periph_TIM13, GPIO_AF_TIM13, 1, -1)
//#endif
//
//#if defined(TIM14)
// INFINITY_TIMER(14, TIM14)
// INFINITY_TIMER_INFO(14, 2, -1, RCC_APB1Periph_TIM14, GPIO_AF_TIM14, 1, -1)
//#endif
//
//   ////////////////////////////////////////////////////////////////////////////
//   enum class timer_output_polarity
//   {
//      high,
//      low
//   };
//
//   ////////////////////////////////////////////////////////////////////////////
//   // timer
//   ////////////////////////////////////////////////////////////////////////////
//   template <std::size_t N>
//   struct timer
//   {
//      static_assert(N >=1 && N <= 14, "Invalid Timer N");
//
//      timer(uint32_t clock_frequency, uint32_t period)
//      {
//         enable_peripheral(
//            int_<timer_info<N>::peripheral_id>(),
//            int_<timer_info<N>::peripheral_group>()
//         );
//
//         TIM_TimeBaseInitTypeDef init;
//         init.TIM_Period = period - 1;
//
//         uint32_t timer_clock = SystemCoreClock / timer_info<N>::sys_clock_div;
//         uint32_t prescaler_val = (timer_clock / clock_frequency) - 1;
//         if (prescaler_val > int_max<decltype(init.TIM_Prescaler)>())
//         {
//            // $$$ overflow error $$$
//         }
//
//         init.TIM_Prescaler = prescaler_val;
//         init.TIM_ClockDivision = TIM_CKD_DIV1;
//
//         init.TIM_CounterMode = TIM_CounterMode_Up;
//         TIM_TimeBaseInit(get_timer(), &init);
//      }
//
//      template <
//         std::size_t channel, std::size_t pin,
//         timer_output_polarity polarity = timer_output_polarity::high
//      >
//      void enable_channel()
//      {
//         timer_pin<pin> out;
//         enum
//         {
//            port = timer_pin<pin>::port,
//            pin = timer_pin<pin>::bit
//         };
//         GPIO_PinAFConfig(&get_port<port>(), pin, 0x02);
//
//         TIM_OCInitTypeDef init;
//         TIM_OCStructInit(&init);
//         init.TIM_OCMode = TIM_OCMode_PWM1;
//         init.TIM_OutputState = TIM_OutputState_Enable;
//         init.TIM_Pulse = period() / 2; // start at 50% pulse width
//         init.TIM_OCPolarity = polarity == timer_output_polarity::high ?
//                                 TIM_OCPolarity_High : TIM_OCPolarity_Low;
//
//         init_channel(&init, int_<channel>());
//      }
//
//      void period(uint32_t n)
//      {
//         get_timer()->ARR = n;
//      }
//
//      uint32_t period() const
//      {
//         return get_timer()->ARR;
//      }
//
//      template <std::size_t channel>
//      void width(uint32_t n)
//      {
//         width(n, int_<channel>());
//      }
//
//      template <std::size_t channel>
//      uint32_t width() const
//      {
//         return width(int_<channel>());
//      }
//
//      void enable_interrupt(std::size_t priority = 0)
//      {
//         // $$$ TODO: disable this or issue a static assert if timer
//         // does not allow interrupt (timer_info<N>::irq_id == -1) $$$
//
//         // Enable the timer global Interrupt
//         NVIC_InitTypeDef init;
//         init.NVIC_IRQChannel = timer_info<N>::irq_id;
//         init.NVIC_IRQChannelPreemptionPriority = priority;
//         init.NVIC_IRQChannelSubPriority = 0;
//         init.NVIC_IRQChannelCmd = ENABLE;
//         NVIC_Init(&init);
//
//         // enable the timer interrupt
//         get_timer()->DIER |= TIM_IT_Update;
//      }
//
//      void enable()
//      {
//         TIM_ARRPreloadConfig(get_timer(), ENABLE);
//         TIM_Cmd(get_timer(), ENABLE);
//      }
//
//   private:
//
//      template <std::size_t pin>
//      using timer_pin = output_pin<
//         pin,
//         port_output_speed::high_speed,
//         port_output_type::output_push_pull,
//         port_output_mode::alternate_function_mode>;
//
//      template <int id>
//      static void enable_peripheral(int_<id>, int_<1>)
//      {
//         RCC->APB1ENR |= id;
//      }
//
//      template <int id>
//      static void enable_peripheral(int_<id>, int_<2>)
//      {
//         RCC->APB2ENR |= id;
//      }
//
//      static TIM_TypeDef* get_timer()
//      {
//         return &infinity::get_timer<N>();
//      }
//
//      void init_channel(TIM_OCInitTypeDef* init, int_<0>)
//      {
//         static_assert(
//            N != 6 && N != 7,
//            "Invalid Channel"
//         );
//
//         TIM_OC1Init(get_timer(), init);
//         TIM_OC1PreloadConfig(get_timer(), TIM_OCPreload_Enable);
//      }
//
//      void init_channel(TIM_OCInitTypeDef* init, int_<1>)
//      {
//         static_assert(
//            N == 1 || N == 2 || N == 3 || N == 4 || N == 5 || N == 8 || N == 9 || N == 12,
//            "Invalid Channel"
//         );
//
//         TIM_OC2Init(get_timer(), init);
//         TIM_OC2PreloadConfig(get_timer(), TIM_OCPreload_Enable);
//      }
//
//      void init_channel(TIM_OCInitTypeDef* init, int_<2>)
//      {
//         static_assert(
//            N == 1 || N == 2 || N == 3 || N == 4 || N == 5 || N == 8,
//            "Invalid Channel"
//         );
//
//         TIM_OC3Init(get_timer(), init);
//         TIM_OC3PreloadConfig(get_timer(), TIM_OCPreload_Enable);
//      }
//
//      void init_channel(TIM_OCInitTypeDef* init, int_<3>)
//      {
//         static_assert(
//            N == 1 || N == 2 || N == 3 || N == 4 || N == 5 || N == 8,
//            "Invalid Channel"
//         );
//
//         TIM_OC4Init(get_timer(), init);
//         TIM_OC4PreloadConfig(get_timer(), TIM_OCPreload_Enable);
//      }
//
//      uint32_t width(int_<0>) const { return get_timer()->CCR1; }
//      uint32_t width(int_<1>) const { return get_timer()->CCR2; }
//      uint32_t width(int_<2>) const { return get_timer()->CCR3; }
//      uint32_t width(int_<3>) const { return get_timer()->CCR4; }
//
//      void width(uint32_t n, int_<0>) { get_timer()->CCR1 = n; }
//      void width(uint32_t n, int_<1>) { get_timer()->CCR2 = n; }
//      void width(uint32_t n, int_<2>) { get_timer()->CCR3 = n; }
//      void width(uint32_t n, int_<3>) { get_timer()->CCR4 = n; }
//   };
}}

#endif
