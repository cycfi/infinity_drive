/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_APP_IMPL_HPP_JANUARY_1_2016)
#define CYCFI_INFINITY_APP_IMPL_HPP_JANUARY_1_2016

#include <inf/pin.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Generic LED indicator (used for signaling errors)
   // (Port and Pin is board/application specific)
   ////////////////////////////////////////////////////////////////////////////

#ifdef STM32F429I_DISCO
    // STM32F429I-Discovery dev board.
   using main_led_type = output_pin<port::portg + 13>;
   using main_test_pin_type = output_pin<port::porta + 15>;
#endif

#ifdef NUCLEO_F446RE
# ifndef CYCFI_INFINITY_BOARD
   // Standard STM32F446 Nucleo-64 board
   using main_led_type = output_pin<port::porta + 5>;
# else
   // Modified STM32F446 Nucleo-64 board (avoids conflicts with the DAC)
   using main_led_type = output_pin<port::portc + 5>;
# endif

   using main_button_type = input_pin<port::portc + 13, port::pull_up>;
   using main_test_pin_type = output_pin<port::portc + 12>;

#endif


}}

#endif
