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
   template <std::size_t scl_pin_, std::size_t sda_pin_>
   struct i2c_master
   {
      static_assert(detail::valid_scl_pin<scl_pin_>(), "Invalid SCL pin");
      static_assert(detail::valid_sda_pin<sda_pin_>(), "Invalid SDA pin");

      static constexpr std::size_t scl_pin = scl_pin_;
      static constexpr std::size_t sda_pin = sda_pin_;
      static constexpr std::size_t id = detail::scl_pin<scl_pin>::i2c_id;
      
      i2c_master()
      {
         static constexpr auto scl_port = scl_pin / 16;
         static constexpr auto sda_port = sda_pin / 16;
         
         // Enable GPIO peripheral clocks
         detail::enable_port_clock<scl_port>();
         detail::enable_port_clock<sda_port>();
         
         detail::i2c_config(
            id,
            detail::get_port<scl_port>(),
            1 << (scl_pin % 16),
            detail::get_port<sda_port>(),
            1 << (sda_pin % 16)
         );
      }

      void write(
         std::uint32_t addr, std::uint8_t const* data,
         std::size_t len, uint32_t timeout = 0xffffffff
      )
      {
         detail::i2c_write(id, addr, data, len, timeout);
      }

      void write(
         std::uint32_t addr, std::uint8_t* data,
         std::size_t len, uint32_t timeout = 0xffffffff
      )
      {
         detail::i2c_read(id, addr, data, len, timeout);
      }
   };
}}

#endif
