/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_I2C_HPP_AUGUST_16_2017)
#define CYCFI_INFINITY_I2C_HPP_AUGUST_16_2017

#include <inf/detail/i2c_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // i2c
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t id_, std::size_t sda_pin, std::size_t scl_pin>
   struct i2c_master
   {
      i2c_master()
      {
         detail::i2c_config();
      }

      void write(std::uint8_t const* data, std::size_t len, uint32_t timeout)
      {
         detail::i2c_write(data, len, timeout);
      }
   };
}}

#endif
