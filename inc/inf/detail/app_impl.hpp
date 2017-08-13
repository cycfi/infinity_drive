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
   // (Port and Pin is application specific)
   //
   ////////////////////////////////////////////////////////////////////////////

#ifdef STM32F429I_DISCO
    // STM32F429I-Discovery dev board.
   using main_led_type = output_pin<port::portg + 13>;
#endif

#ifdef NUCLEO_F446RE
   // STM32F446 Nucleo-64 board
   using main_led_type = output_pin<port::porta + 5>;
#endif
}}

#endif
