/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_I2C_IMPL_HPP_AUGUST_16_2017)
#define CYCFI_INFINITY_I2C_IMPL_HPP_AUGUST_16_2017

#include "stm32f4xx_hal.h"

namespace cycfi { namespace infinity { namespace detail
{
   void i2c_config();
   void i2c_write(uint8_t const* data, uint32_t len, uint32_t timeout);
}}}

#endif
