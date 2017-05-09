/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_OLED_HPP_MAY_7_2015)
#define CYCFI_INFINITY_OLED_HPP_MAY_7_2015

#include <inf/spi.hpp>
#include <inf/pin.hpp>
#include <inf/support.hpp>
#include <inf/canvas.hpp>

namespace cycfi { namespace infinity
{
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

      oled_SH1106(canvas<128, 64>& canvas_)
       : _canvas(canvas_)
      {
         _spi.enable();
         reset();
      }

      void reset();
      void on();
      void off();
      void refresh();
      void contrast(std::uint8_t val);

   private:

      void command(std::uint8_t cmd);
      void out(std::uint8_t data);

      using canvas_ = canvas<128, 64>;
      using spi = spi_master<spi_id, sck_pin, mosi_pin>;
      using rst_pin = output_pin<rst_port>;
      using dc_pin = output_pin<dc_port>;
      using cs_pin = output_pin<cs_port>;

      canvas_& _canvas;
      spi      _spi;
      rst_pin  _rst;
      dc_pin   _dc;
      cs_pin   _cs;
   };

   ////////////////////////////////////////////////////////////////////////////
   // oled_SH1106 implementation
   ////////////////////////////////////////////////////////////////////////////
   namespace display_commands
   {
      enum
      {
         set_col_low       = 0x00,
         set_col_high      = 0x10,
         memory_mode       = 0x20,
         set_col_addr      = 0x21,
         set_page_addr     = 0x22,
         charge_pump       = 0x8D,
         set_mux_ratio     = 0xA8,
         display_off       = 0xAE,
         display_on        = 0xAF,
         page              = 0xB0,
         set_start_line    = 0x40,
         scan_dir_incr     = 0xC0,
         scan_dir_decr     = 0xC8,
         segment_map_incr  = 0xA0,
         segment_map_decr  = 0xA1,
         set_contrast      = 0x81,
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
      ::out(std::uint8_t data)
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
      using namespace display_commands;

      enum
      {
         enable_charge_val   = 0x14,
         horizontal_addr_mode = 0x00
      };

      _rst = 0;
      delay_ms(1);
      _rst = 1;

      // Turn display off
      off();

      // Wait 100mS for DC-DC to stabilise. This can probably be reduced
      delay_ms(100);

      // Set MUX ratio
      command(set_mux_ratio);
      command(0x3F);

      // Enable charge pump
      command(charge_pump);
      command(enable_charge_val);

      command(memory_mode);
      command(0x00);

      // Clear the canvas
      _canvas.clear();

      // Print the canvas to clear the display RAM
      refresh();

      // Set default contrast
      contrast(0x8F);

      // Flip display horizontally and vertically
      command(scan_dir_decr);
      command(segment_map_decr);

      // Turn display on
      on();

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
      using namespace display_commands;

      enum
      {
         col_start   = 0x02,
         col_end     = 0x7F,
         page_start  = 0,
         page_end    = 7,
         ram_pages   = page_end - page_start + 1
      };

      auto* p = _canvas.begin();
      for (auto row = 0; row != ram_pages; ++row)
      {
         command(page | row);
         command(set_col_low | col_start);   // Lower column address
         command(set_col_high | 0x0);        // Higher column address

         _dc = 1;
         for (auto col = 0; col != _canvas.width; ++col)
            out(*p++);
      }
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::contrast(std::uint8_t val)
   {
      command(display_commands::set_contrast);
      command(val);
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::on()
   {
      command(display_commands::display_on);
   }

   template <
      std::size_t spi_id, std::size_t sck_pin, std::size_t mosi_pin,
      std::size_t rst_port, std::size_t dc_port, std::size_t cs_port
   >
   inline void oled_SH1106<spi_id, sck_pin, mosi_pin, rst_port, dc_port, cs_port>
      ::off()
   {
      command(display_commands::display_off);
   }
}}

#endif
