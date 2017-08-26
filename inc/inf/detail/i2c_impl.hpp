/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_I2C_IMPL_HPP_AUGUST_16_2017)
#define CYCFI_INFINITY_I2C_IMPL_HPP_AUGUST_16_2017

#include <stm32f4xx_hal.h>
#include <cstdint>

namespace cycfi { namespace infinity { namespace detail
{
   void i2c_config(
	  std::size_t id,
      GPIO_TypeDef& scl_gpio, std::uint32_t scl_pin_mask,
      GPIO_TypeDef& sda_gpio, std::uint32_t sda_pin_mask
   );

   void i2c_write(
		std::size_t id, std::uint32_t addr,
		uint8_t const* data, uint32_t len, uint32_t timeout);
}}}

#endif
