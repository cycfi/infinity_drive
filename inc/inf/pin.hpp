/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PIN_HPP_DECEMBER_20_2015)
#define CYCFI_INFINITY_PIN_HPP_DECEMBER_20_2015

#include <inf/detail/pin_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Constants
   ////////////////////////////////////////////////////////////////////////////

   namespace port
   {
      constexpr uint16_t porta = 0 * 16;
      constexpr uint16_t portb = 1 * 16;
      constexpr uint16_t portc = 2 * 16;
      constexpr uint16_t portd = 3 * 16;
      constexpr uint16_t porte = 4 * 16;
      constexpr uint16_t portf = 5 * 16;
      constexpr uint16_t portg = 6 * 16;
      constexpr uint16_t porth = 7 * 16;
      constexpr uint16_t porti = 8 * 16;
   }

   enum class port_output_speed
   {
      low_speed = LL_GPIO_SPEED_FREQ_LOW,             // 2MHz low speed
      mid_speed = LL_GPIO_SPEED_FREQ_MEDIUM,          // 12.5 MHz to 50 MHz medium speed
      high_speed = LL_GPIO_SPEED_FREQ_HIGH,           // 25 MHz to 100 MHz high speed
      very_high_speed = LL_GPIO_SPEED_FREQ_VERY_HIGH  // 50 MHz to 200 MHz very high speed
   };

   enum class port_output_type
   {
      push_pull = LL_GPIO_OUTPUT_PUSHPULL,
      open_drain = LL_GPIO_OUTPUT_OPENDRAIN
   };
   
   namespace port
   {
      auto constexpr low_speed = port_output_speed::low_speed;
      auto constexpr mid_speed = port_output_speed::mid_speed;
      auto constexpr high_speed = port_output_speed::high_speed;
      auto constexpr very_high_speed = port_output_speed::very_high_speed;
      
      auto constexpr push_pull = port_output_type::push_pull;
      auto constexpr open_drain = port_output_type::open_drain;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   struct inverse_pin
   {
      operator bool() const
      {
         return pin.off();
      }

      T operator!() const
      {
         return pin;
      }

      T pin;
   };

   struct on_type
   {
      operator bool() const
      {
         return true;
      }

      bool operator!() const
      {
         return false;
      }
   };

   struct off_type
   {
      operator bool() const
      {
         return false;
      }

      bool operator!() const
      {
         return true;
      }
   };

   namespace port
   {
      constexpr on_type on = {};
      constexpr off_type off = {};
   }

   ////////////////////////////////////////////////////////////////////////////
   // output_pin
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t N,
      port_output_speed speed = port::high_speed,
      port_output_type type = port::push_pull
   >
   struct output_pin
   {
      static constexpr size_t    n = N;
      static constexpr uint16_t  bit = N % 16;
      static constexpr uint16_t  port = N / 16;
      static constexpr uint32_t  mask = 1 << bit;

      // there are only 9 ports
      static_assert(port < 9, "Invalid port");

      typedef output_pin self_type;
      typedef inverse_pin<output_pin> inverse_type;

      void setup()
      {
         // Enable GPIO peripheral clock
         detail::enable_port_clock<port>();

         // Configure output mode
         LL_GPIO_SetPinMode(&gpio(), mask, LL_GPIO_MODE_OUTPUT);

         // Configure output push pull or open drain
         LL_GPIO_SetPinOutputType(&gpio(), mask, uint32_t(type));

         // Configure output speed
         LL_GPIO_SetPinSpeed(&gpio(), mask, uint32_t(speed));

         // Configure pull-up/down resistor
         LL_GPIO_SetPinPull(&gpio(), mask, LL_GPIO_PULL_NO);
      }

      output_pin()
      {
         setup();
      }

      output_pin(on_type)
      {
         setup();
         ref() |= mask;
      }

      output_pin(off_type)
      {
         setup();
         ref() &= ~mask;
      }

      output_pin(output_pin const&)
      {
      }

      GPIO_TypeDef& gpio() const
      {
         return detail::get_port<port>();
      }

      volatile uint32_t& ref() const
      {
         return gpio().ODR;
      }

      bool state() const
      {
         return (ref() & mask) != 0;
      }

      inverse_type operator!() const
      {
         return { *this };
      }

      output_pin& operator=(bool val)
      {
         ref() ^= (-uint16_t(val) ^ ref()) & mask;
         return *this;
      }

      output_pin& operator=(self_type)
      {
         return *this;
      }

      output_pin& operator=(inverse_type)
      {
         ref() ^= mask;
         return *this;
      }

      output_pin& operator=(on_type)
      {
         ref() |= mask;
         return *this;
      }

      output_pin& operator=(off_type)
      {
         ref() &= ~mask;
         return *this;
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   // input_pin
   ////////////////////////////////////////////////////////////////////////////
   enum class port_input_type
   {
      normal = LL_GPIO_PULL_NO,
      pull_up = LL_GPIO_PULL_UP,
      pull_down = LL_GPIO_PULL_DOWN
   };

   enum class port_edge
   {
      rising, 
      falling
   };
   
   namespace port
   {
      auto constexpr pull_up = port_input_type::pull_up;
      auto constexpr pull_down = port_input_type::pull_down;
      auto constexpr rising = port_edge::rising;
      auto constexpr falling = port_edge::falling;
   }

   template <std::size_t N, port_input_type type = port_input_type::normal>
   struct input_pin
   {
      static constexpr size_t    n = N;
      static constexpr uint16_t  bit = N % 16;
      static constexpr uint16_t  port = N / 16;
      static constexpr uint32_t  mask = 1 << bit;

      // there are only 8 ports
      static_assert(port < 8, "Invalid port");

      typedef input_pin self_type;

      input_pin()
      {
         // Enable GPIO peripheral clock
    	  detail::enable_port_clock<port>();

         // Configure input mode
         LL_GPIO_SetPinMode(&gpio(), mask, LL_GPIO_MODE_INPUT);

         // Configure pull-up/down resistor
         LL_GPIO_SetPinPull(&gpio(), mask, uint32_t(type));
      }

      void enable_interrupt(std::size_t priority = 0)
      {
         // Connect External Line to the GPIO
         LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                  
         LL_SYSCFG_SetEXTISource(detail::exti_port<port>(), detail::exti_id<bit>()); 
         
         // Configure NVIC to handle external interrupt
         NVIC_SetPriority(detail::exti_irq<bit>(), priority);
         NVIC_EnableIRQ(detail::exti_irq<bit>());
      }

      void start(port_edge edge)
      {
         LL_EXTI_EnableIT_0_31(detail::exti_src<bit>());
         if (edge == port_edge::rising)
            LL_EXTI_EnableRisingTrig_0_31(detail::exti_src<bit>());
         else
            LL_EXTI_EnableFallingTrig_0_31(detail::exti_src<bit>());
      }

      void stop(port_edge edge)
      {
         LL_EXTI_DisableIT_0_31(detail::exti_src<port>());
         if (edge == port_edge::rising)
            LL_EXTI_DisableRisingTrig_0_31(detail::exti_src<port>());
         else
            LL_EXTI_DisableFallingTrig_0_31(detail::exti_src<port>());
      }

      GPIO_TypeDef& gpio() const
      {
         return detail::get_port<port>();
      }

      auto& ref() const
      {
         return gpio().IDR;
      }

      bool state() const
      {
         return (ref() & mask) != 0;
      }

      operator bool() const
      {
         return state();
      }

      bool operator!() const
      {
         return !state();
      }
   };
}}

#endif
