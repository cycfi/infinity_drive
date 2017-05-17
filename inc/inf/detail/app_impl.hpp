/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

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
   // This implementation is for the STM32F429I-Discovery dev board.
   ////////////////////////////////////////////////////////////////////////////
   using main_led_type = output_pin<port::portg + 13>;
}}

#endif
