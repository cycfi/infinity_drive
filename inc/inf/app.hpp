/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_APP_HPP_JANUARY_1_2016)
#define CYCFI_INFINITY_APP_HPP_JANUARY_1_2016

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
