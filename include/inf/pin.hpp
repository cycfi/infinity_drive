/*=======================================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(CYCFI_INFINITY_PIN_HPP_DECEMBER_20_2015)
#define CYCFI_INFINITY_PIN_HPP_DECEMBER_20_2015

#include <cstddef>
#include <cstdint>
#include "stm32f4xx.h"

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // The ports: We provide template functions for getting the memory mapped
	//	ports given a constant N. That way, we can use generic programming.
   ////////////////////////////////////////////////////////////////////////////

	template <std::size_t port>
	GPIO_TypeDef& get_port();

#define INFINITY_IOPORT(N, PORT_NAME)                                          \
   template <>                                                                 \
   inline GPIO_TypeDef& get_port<N>()                                          \
   {                                                                           \
      return *PORT_NAME;                                                       \
   }                                                                           \
   /***/

INFINITY_IOPORT(0, GPIOA)
INFINITY_IOPORT(1, GPIOB)
INFINITY_IOPORT(2, GPIOC)
INFINITY_IOPORT(3, GPIOD)
INFINITY_IOPORT(4, GPIOE)
INFINITY_IOPORT(5, GPIOF)
INFINITY_IOPORT(6, GPIOG)
INFINITY_IOPORT(7, GPIOH)
INFINITY_IOPORT(8, GPIOI)

   ////////////////////////////////////////////////////////////////////////////
   // Constants
   ////////////////////////////////////////////////////////////////////////////

   constexpr uint16_t porta = 0 * 16;
   constexpr uint16_t portb = 1 * 16;
   constexpr uint16_t portc = 2 * 16;
   constexpr uint16_t portd = 3 * 16;
   constexpr uint16_t porte = 4 * 16;
   constexpr uint16_t portf = 5 * 16;
   constexpr uint16_t portg = 6 * 16;
   constexpr uint16_t porth = 7 * 16;
   constexpr uint16_t porti = 8 * 16;

   enum class port_output_speed
   {
      low_speed = 0,       // 2MHz low speed
      mid_speed = 1,       // 12.5 MHz to 50 MHz medium speed
      high_speed = 2,      // 25 MHz to 100 MHz high speed
      very_high_speed = 3  // 50 MHz to 200 MHz very high speed
   };

   enum class port_output_type
   {
      output_push_pull = true,
      output_open_drain = false
   };

   enum class port_output_mode
   {
      // allows the GPIO pin to be used as an output pin
      output_mode = 1,

      // allow the GPIO pins to be used by peripherals such as the UART, SPI e.t.c.
      alternate_function_mode = 2
   };

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

   static on_type const on = {};
   static off_type const off = {};

   ////////////////////////////////////////////////////////////////////////////
   // output_pin
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t N,
      port_output_speed speed = port_output_speed::very_high_speed,
      port_output_type push_pull = port_output_type::output_push_pull,
      port_output_mode mode = port_output_mode::output_mode
   >
   struct output_pin
   {
      static constexpr uint16_t bit = N % 16;
      static constexpr uint16_t port = N / 16;
      static constexpr uint32_t mask = 1 << bit;
      static constexpr uint32_t mask2 = 0x3 << (bit * 2);

      // there are only 9 ports
      static_assert(port < 9, "Invalid port");

      typedef output_pin self_type;
      typedef inverse_pin<output_pin> inverse_type;

      void setup()
      {
         // Enable GPIO peripheral clock
         RCC->AHB1ENR |= 1 << port;

         // Configure output mode
         gpio().MODER |=
            (gpio().MODER & mask2) | (uint32_t(mode) << (bit * 2));

         // Configure output push pull or open drain
         if (bool(push_pull))
            gpio().OTYPER &= ~mask;
         else
            gpio().OTYPER |= mask;

         // Configure output speed
         gpio().OSPEEDR |=
            (gpio().OSPEEDR & mask2) | (uint32_t(speed) << (bit * 2));

         // Configure pull-up/down resistor
         gpio().PUPDR &= ~mask2;
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

      volatile GPIO_TypeDef& gpio() const
      {
         return get_port<port>();
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
   enum class port_input_mode
   {
      // allows the GPIO pin to be used as an input pin
      input_mode = 0,

      // allow the GPIO pins to be used by peripherals such as the UART, SPI e.t.c.
      alternate_function_mode = 2,

      // allows the GPIO pin to be used as an Analog input pin
      analog_mode = 3
   };

   enum class port_input_type
   {
      input_normal = true,
      input_pull_down = false
   };

   template <std::size_t N,
      port_input_mode mode = port_input_mode::input_mode,
      port_input_type pull_down = port_input_type::input_normal
   >
   struct input_pin
   {
      static constexpr uint16_t bit = N % 16;
      static constexpr uint16_t port = N / 16;
      static constexpr uint32_t mask = 1 << bit;
      static constexpr uint32_t mask2 = 0x3 << (bit * 2);

      // there are only 8 ports
      static_assert(port < 8, "Invalid port");

      typedef input_pin self_type;

      input_pin()
      {
         // Enable the AHB peripheral clock
         RCC->AHB1ENR |= 1 << port;

         // Configure input mode
         gpio().MODER |=
            (gpio().MODER & mask2) | (uint32_t(mode) << (bit * 2));

         // Configure pull-up/down resistor
         if (pull_down == port_input_type::input_normal)
            gpio().PUPDR &= ~mask2;
         else
            gpio().PUPDR |= mask2;
      }

      volatile GPIO_TypeDef& gpio() const
      {
         return get_port<port>();
      }

      volatile uint16_t& ref() const
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
