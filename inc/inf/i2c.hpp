/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_I2C_HPP_AUGUST_16_2017)
#define CYCFI_INFINITY_I2C_HPP_AUGUST_16_2017

#include <inf/detail/i2c_impl.hpp>
#include <inf/detail/pin_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // i2c
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t id_, std::size_t sda_pin_, std::size_t scl_pin_>
   struct i2c_master
   {
      static constexpr std::size_t id = id_;
      static constexpr std::size_t sda_pin = sda_pin_;
      static constexpr std::size_t scl_pin = scl_pin_;

		i2c_master()
      {
         detail::i2c_config(
				id,
            detail::get_port<sda_pin/16>(),
            1 << (sda_pin % 16),
            detail::get_port<scl_pin/16>(),
            1 << (scl_pin % 16)
         );
      }

      void write(
			std::uint32_t addr, std::uint8_t const* data,
			std::size_t len, uint32_t timeout = 0xffffffff
		)
      {
         detail::i2c_write(id, addr, data, len, timeout);
      }
   };
}}

#endif
