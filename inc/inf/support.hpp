/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SUPPORT_HPP_DECEMBER_20_2015)
#define CYCFI_INFINITY_SUPPORT_HPP_DECEMBER_20_2015

#include <type_traits>
#include <cstdint>
#include <system_stm32f4xx.h>
#include <stm32f4xx_ll_utils.h>

extern "C"
{
   extern volatile uint32_t timer_delay; // maintained by SysTick_Handler
}

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic metaprogramming utils
   ////////////////////////////////////////////////////////////////////////////
   template <bool val>
   using bool_ = std::integral_constant<bool, val>;
   
   template <int i>
   using int_ = std::integral_constant<int, i>;

   template <std::size_t i>
   using uint_ = std::integral_constant<std::size_t, i>;

   template <int8_t i>
   using int8_ = std::integral_constant<int8_t, i>;

   template <uint8_t i>
   using uint8_ = std::integral_constant<uint8_t, i>;

   template <int16_t i>
   using int16_ = std::integral_constant<int16_t, i>;

   template <uint16_t i>
   using uint16_ = std::integral_constant<uint16_t, i>;

   template <int32_t i>
   using int32_ = std::integral_constant<int32_t, i>;

   template <uint32_t i>
   using uint32_ = std::integral_constant<uint32_t, i>;

   template <int64_t i>
   using int64_ = std::integral_constant<int64_t, i>;

   template <uint64_t i>
   using uint64_ = std::integral_constant<uint64_t, i>;

   ////////////////////////////////////////////////////////////////////////////
   // Clock initialization
   ////////////////////////////////////////////////////////////////////////////
   namespace detail
   {
      void system_clock_config();
   }

   ////////////////////////////////////////////////////////////////////////////
   // The MCU clock speed
   ////////////////////////////////////////////////////////////////////////////
   uint32_t const clock_speed = SystemCoreClock;

   ////////////////////////////////////////////////////////////////////////////
   // delay_ms function
	////////////////////////////////////////////////////////////////////////////
   inline void delay_ms(uint32_t ms)
   {
	   LL_mDelay(ms);
   }

   ////////////////////////////////////////////////////////////////////////////
   // error_handler
   ////////////////////////////////////////////////////////////////////////////
   void error_handler();

   ////////////////////////////////////////////////////////////////////////////
   // Some constants
	////////////////////////////////////////////////////////////////////////////
   template <typename T>
   struct int_traits;

   template <>
   struct int_traits<uint32_t>
   {
      static constexpr uint32_t max = 4294967295;
      static constexpr uint32_t min = 0;
   };

   template <>
   struct int_traits<int32_t>
   {
      static constexpr int32_t max = 2147483647;
      static constexpr int32_t min = -2147483648;
   };

   template <>
   struct int_traits<uint16_t>
   {
      static constexpr uint16_t max = 65535;
      static constexpr uint16_t min = 0;
   };

   template <>
   struct int_traits<int16_t>
   {
      static constexpr int16_t max = 32767;
      static constexpr int16_t min = -32768;
   };

   template <>
   struct int_traits<uint8_t>
   {
      static constexpr uint8_t max = 255;
      static constexpr uint8_t min = 0;
   };

   template <>
   struct int_traits<int8_t>
   {
      static constexpr uint8_t max = 127;
      static constexpr uint8_t min = -128;
   };

   template <typename T>
   constexpr T int_max()
   {
      return int_traits<T>::max;
   }

   template <typename T>
   constexpr T int_min()
   {
      return int_traits<T>::min;
   }

   ////////////////////////////////////////////////////////////////////////////
   // fixed point utilities
   ////////////////////////////////////////////////////////////////////////////

   // 16.16 bit fixed point one (1.0 representation)
   constexpr int32_t fxp_one = 65536;

   constexpr int32_t fxp(double n)
   {
      return n * fxp_one;
   }

   constexpr int32_t fxp(int32_t n)
   {
      return n << 16;
   }

   ////////////////////////////////////////////////////////////////////////////
   // integer and binary functions
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   constexpr T smallest_pow2(T n, T m = 1)
   {
      return (m < n)? smallest_pow2(n, m << 1) : m;
   }
   
   template <typename T>
   constexpr bool is_pow2(T n)
   {   
      return (n & (n - 1)) == 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic math functions and constants
   ////////////////////////////////////////////////////////////////////////////

   // fast exp approximation (from http://www.musicdsp.org/)
   constexpr float exp(float x)
   {
      return 
         (362880.f + x * 
            (362880.f + x * 
               (181440.f + x * 
                  (60480.f + x * 
                     (15120.f + x * 
                        (3024.f + x * 
                           (504.f + x * 
                              (72.f + x * 
                                 (9.f + x)
         )))))))) * 2.75573192e-6f;
   }

   // pi
   constexpr float pi = 3.1415926535897f;
   constexpr float _2pi = pi * 2.0f;

   ////////////////////////////////////////////////////////////////////////////
   // linear interpolation: Interpolates a value linearly between y1 and y2
   // given mu. If mu is 0, the result is y1. If mu is 1, then the result is
   // y2. 
   ////////////////////////////////////////////////////////////////////////////
   constexpr float linear_interpolate(float y1, float y2, float mu)
   {
      return y1 + mu * (y2 - y1);
   }
   
   ////////////////////////////////////////////////////////////////////////////
   // Fast reciprocal. See http://tinyurl.com/lgmnsyg. We want to avoid
   // multiplication. So instead of 1.0f/val, we use this function which
   // is a inaccurate, but fast substitute. It works by negating the exponent
   // which is assumed to be IEEE754.
   ////////////////////////////////////////////////////////////////////////////
   inline float inverse(float val)
   {
      auto x = reinterpret_cast<std::int32_t&>(val);
      x = 0x7EF311C2 - x;
      return reinterpret_cast<float&>(x);
   }
}}

#endif
