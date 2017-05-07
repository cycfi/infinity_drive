/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SPI_HPP_MAY_4_2017)
#define CYCFI_INFINITY_SPI_HPP_MAY_4_2017

#include <inf/detail/spi_impl.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // spi
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t id_>
   struct spi_base
   {
      static constexpr std::size_t id = id_;

      void enable()
      {
         LL_SPI_Enable(detail::get_spi<id>());
      }

      void disable()
      {
         LL_SPI_Disable(detail::get_spi<id>());
      }

      bool write(std::uint8_t byte)
      {
         if (is_writing())
            return false;
         _write_byte = byte;
         detail::spi_write(id, &_write_byte, 1);
         return true;
      }

      bool write(std::uint8_t const* data, std::size_t len)
      {
         if (is_writing())
            return false;
         detail::spi_write(id, data, len);
         return true;
      }

      bool is_writing() const
      {
         return detail::spi_is_writing(id);
      }

      bool read(std::uint8_t* data, std::size_t len)
      {
         if (is_reading())
            return false;
         detail::spi_read(id, data, len);
         return true;
      }

      bool is_reading() const
      {
         return detail::spi_is_reading(id);
      }

   private:

      std::uint8_t _write_byte;
   };

   template <std::size_t id_, std::size_t sck_pin, int mosi_pin = -1, int miso_pin = -1>
   struct spi_master : spi_base<id_>
   {
      static constexpr std::size_t id = id_;

      spi_master()
      {
         bool const is_half_duplex = miso_pin == -1;
         detail::spi_pin_config<sck_pin, mosi_pin, miso_pin>();
         detail::spi_enable_clock<id>();

         detail::spi_config(
            id,
            detail::spi_id<id>(),
            detail::get_spi<id>(),
            true,             // master?
            is_half_duplex    // half duplex?
         );
      }
   };

   template <std::size_t id_, std::size_t sck_pin, int mosi_pin = -1, int miso_pin = -1>
   struct spi_slave : spi_base<id_>
   {
      static constexpr std::size_t id = id_;

      spi_slave()
      {
         bool const is_half_duplex = mosi_pin == -1;
         detail::spi_pin_config<sck_pin, mosi_pin, miso_pin>();
         detail::spi_enable_clock<id>();

         detail::spi_config(
            id,
            detail::spi_id<id>(),
            detail::get_spi<id>(),
            false,            // master?
            is_half_duplex    // half duplex?
         );
      }
   };
}}

#endif
