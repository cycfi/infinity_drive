/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SPI_HPP_MAY_4_2017)
#define CYCFI_INFINITY_SPI_HPP_MAY_4_2017

//#include <inf/detail/pin_impl.hpp>
//#include <inf/timer.hpp>
#include <inf/detail/spi_impl.hpp>
//#include <array>

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
   };

   template <std::size_t id_>
   struct spi_master : spi_base<id_>
   {
      static constexpr std::size_t id = id_;

      template <std::size_t sck_pin, int mosi_pin, int miso_pin>
      spi_master()
      {
         detail::spi_pin_config<sck_pin, mosi_pin, miso_pin>();

         detail::spi_config(
            detail::spi_id<id>(),
            detail::spi_periph_id<id>(),
            detail::get_spi<id>(),
            true              // master
         );
      }
   };

   template <std::size_t id_>
   struct spi_slave : spi_base<id_>
   {
      static constexpr std::size_t id = id_;

      template <std::size_t sck_pin, int mosi_pin, int miso_pin>
      spi_slave()
      {
         detail::spi_pin_config<sck_pin, mosi_pin, miso_pin>();

         detail::spi_config(
            detail::spi_id<id>(),
            detail::spi_periph_id<id>(),
            detail::get_spi<id>(),
            false              // master
         );
      }
   };
}}

#endif
