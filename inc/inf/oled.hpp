/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_OLED_HPP_MAY_7_2015)
#define CYCFI_INFINITY_OLED_HPP_MAY_7_2015

#include <inf/spi.hpp>
#include <inf/pin.hpp>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // drawing canvas
   ////////////////////////////////////////////////////////////////////////////
   struct canvas
   {
      enum { width  = 128, height = 64 };

      void clear();

      std::uint8_t _buffer[(width * height) / 8];
   };

   ////////////////////////////////////////////////////////////////////////////
   // oled_SH1106 driver
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   struct oled_SH1106
   {
   public:

      oled_SH1106(canvas& canvas_)
       : _canvas(canvas_)
      {
         _spi.enable();
         reset();
      }

      void reset();
      void refresh();

   private:

      void command(std::uint8_t cmd);
      void print(std::uint8_t data);

      using spi = spi_master<spi_id, sck_pin, mosi_pin>;
      using pin = output_pin<rst_port>;

      canvas&  _canvas;
      spi      _spi;
      pin      _rst;
      pin      _dc;
      pin      _cs;
   };

   ////////////////////////////////////////////////////////////////////////////
   // oled_SH1106 implementation
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      enum display_commands : std::uint8_t
      {
         set_col_low    = 0x00,
         set_col_high   = 0x10,
         memory_mode    = 0x20,
         set_col_addr   = 0x21,
         set_page_addr  = 0x22,
         charge_pump    = 0x8D,
         set_mux_ratio  = 0xA8,
         display_on     = 0xAE,
         display_off    = 0xAF,
         page           = 0xB0,
         set_start_line = 0x40,
         scan_dir_incr  = 0xC0,
         scan_dir_decr  = 0xC8,
         segment_map    = 0xA0,
      };

      enum register_parameters : std::uint8_t
      {
         scan_dir_normal      = 0x00,
         scan_dir_reverse     = 0x08,
         scan_map_normal      = 0,
         scan_map_reverse     = 1,
         enable_charge_pump   = 0x14,
         horizontal_addr_mode = 0x00
      };

      enum constants
      {
         col_start   = 0x02,
         col_end     = 0x7F,
         page_start  = 0,
         page_end    = 7,
         ram_pages   = page_end - page_start + 1
      };
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::command(std::uint8_t cmd)
   {
      _dc = 0;
      _cs = 0;
      _spi.write(cmd);
      while (_spi.is_writing())
         ;
      _cs = 1;
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::print(std::uint8_t data)
   {
      _cs = 0;
      _spi.write(data);
      while (_spi.is_writing())
         ;
      _cs = 1;
   };

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::reset()
   {
      _rst = 0;
      delay_ms(1);
      _rst = 1;

      // Wait 100mS for DC-DC to stabilise. This can probably be reduced
      delay_ms(100);

      // Set MUX ratio
      command(set_mux_ratio);
      command(0x3F);

      // Enable charge pump
      command(charge_pump);
      command(enable_charge_pump);

      // Set memory addressing mode to horizontal addressing
      command(memory_mode);
      command(horizontal_addr_mode);

      // Clear the canvas
      _canvas.clear();

      // Print the canvas to clear the display RAM
      refresh();

      // Turn display on
      command(display_on | 1);

      // Wait 150ms for display to turn on
      delay_ms(150);
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::refresh()
   {
      auto* p = this->_buffer;
      for (auto row = 0; row != ram_pages; ++row)
      {
         command(page | row);
         command(set_col_low | col_start);   // Lower column address
         command(set_col_high | 0x0);        // Higher column address

         _dc = 1;
         for (auto col = 0; col != _canvas.width; ++col)
         {
            draw(*p++);
         }
      }
   }
}}

#endif
