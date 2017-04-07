/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_ERROR_HPP_FEBRUARY_10_2016)
#define CYCFI_INFINITY_ERROR_HPP_FEBRUARY_10_2016

//#include <inf/pin.hpp>
#include <inf/support.hpp>
#include "diag/Trace.h"

namespace cycfi { namespace infinity
{
   void hard_error(char const* error)
   {
      trace_puts(error);

      //output_pin<portd + 14> red_led;     // portd, pin 14
      //while (true)
      //{
      //   delay_ms(100);
      //   red_led = !red_led;
      //   delay_ms(300);
      //   red_led = !red_led;
      //}
   }

#if defined(DEBUG)
# define INF_ASSERT(cond, error) if (!(cond)) hard_error((error))
#else
# define INF_ASSERT(cond, error)
#endif

}}

#endif
