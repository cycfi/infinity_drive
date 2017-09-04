/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_APP_HPP_JANUARY_1_2016)
#define CYCFI_INFINITY_APP_HPP_JANUARY_1_2016

#include <inf/pin.hpp>
#include <inf/detail/app_impl.hpp>

////////////////////////////////////////////////////////////////////////////
// The interrupt handler. By default, this does nothing
// Specialize this for specific interrupt handlers.
////////////////////////////////////////////////////////////////////////////
struct irq_not_handled {};

template <typename Key>
inline irq_not_handled irq(Key)
{
	return {};
}

////////////////////////////////////////////////////////////////////////////
// Interrupt handler keys
////////////////////////////////////////////////////////////////////////////

// External Interrupts
template <std::size_t id>
struct exti_task {};

// Timers
template <std::size_t id>
struct timer_task {};

// ADC
template <std::size_t id>
struct adc_conversion_half_complete {};

template <std::size_t id>
struct adc_conversion_complete {};

#endif
