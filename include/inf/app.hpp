/*=======================================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(CYCFI_INFINITY_APP_HPP_JANUARY_1_2016)
#define CYCFI_INFINITY_APP_HPP_JANUARY_1_2016

#include <inf/pin.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Generic LED indicator (used for signaling errors)
   // (Port and Pin is application specific)
   ////////////////////////////////////////////////////////////////////////////
   using red_led_type = output_pin<portd + 14>;
}}

////////////////////////////////////////////////////////////////////////////
// The interrupt handler. By default, this does nothing
// Specialize this for specific interrupt handlers.
////////////////////////////////////////////////////////////////////////////
template <typename Key>
inline void irq(Key)
{
}

////////////////////////////////////////////////////////////////////////////
// Interrupt handler keys
////////////////////////////////////////////////////////////////////////////

// Timers
template <std::size_t N>
struct timer_task {};

// ADC
struct adc_conversion_half_complete {};
struct adc_conversion_complete {};

#endif
