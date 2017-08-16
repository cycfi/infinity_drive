/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SSD_1306_HPP_AUGUST_16_2017)
#define CYCFI_INFINITY_SSD_1306_HPP_AUGUST_16_2017

#include <inf/support.hpp>
#include <inf/canvas.hpp>
#include <cstdint>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // ssd1306 driver
   ////////////////////////////////////////////////////////////////////////////
   template <typename Port, typename Canvas, std::size_t timeout_ = 0xffffffff>
   struct ssd1306 : Canvas
   {
   public:

      using port = Port;
      using canvas = Canvas;

      static std::size_t const timeout = timeout_;
      static std::size_t const height = Canvas::height;
      static std::size_t const width = Canvas::width;
      static std::size_t const buffer_size = Canvas::size + 1;

      ssd1306();

      void refresh();

   private:

      void command(std::uint8_t cmd);
      void out(std::uint8_t data);
      void out(std::uint8_t const* data, std::size_t len);

      std::uint8_t   _buffer[buffer_size];
      Port           _out;
   };

   ////////////////////////////////////////////////////////////////////////////
   // ssd1306 implementation
   ////////////////////////////////////////////////////////////////////////////
   namespace ssd1306_constants
   {
      enum
      {
         set_contrast               = 0x81,
         display_all_on_resume      = 0xa4,
         display_all_on             = 0xa5,
         normal_display             = 0xa6,
         invert_display             = 0xa7,
         display_off                = 0xae,
         display_on                 = 0xaf,
         set_display_offset         = 0xd3,
         set_com_pins               = 0xda,
         set_vcom_detect            = 0xdb,
         set_display_clock_div      = 0xd5,
         set_pre_charge             = 0xd9,
         set_multiplex              = 0xa8,
         set_low_column             = 0x00,
         set_high_column            = 0x10,
         set_start_line             = 0x40,
         memory_mode                = 0x20,
         column_addr                = 0x21,
         page_addr                  = 0x22,
         com_scan_inc               = 0xc0,
         com_scan_dec               = 0xc8,
         seg_remap                  = 0xa0,
         charge_pump                = 0x8d,
         activate_scroll            = 0x2f,
         deactivate_scroll          = 0x2e,
         set_vert_scroll_area       = 0xa3,
         right_horiz_scroll         = 0x26,
         left_horiz_scroll          = 0x27,
         vert_right_horiz_scroll    = 0x29,
         vert_left_horiz_scroll     = 0x2a
      };
   }

   template <typename Port, typename Canvas, std::size_t timeout>
   inline void ssd1306<Port, Canvas, timeout>::command(std::uint8_t cmd)
   {
      uint8_t buf[2] = { 0, cmd };
      out(buf, 2);
   }

   template <typename Port, typename Canvas, std::size_t timeout>
   inline void ssd1306<Port, Canvas, timeout>::out(std::uint8_t const* data, std::size_t len)
   {
      this->_out.write(data, len, timeout);
   }

   template <typename Port, typename Canvas, std::size_t timeout>
   inline void ssd1306<Port, Canvas, timeout>::out(std::uint8_t data)
   {
      out(&data, 1);
   }

   template <typename Port, typename Canvas, std::size_t timeout>
   inline ssd1306<Port, Canvas, timeout>::ssd1306()
    : Canvas(&_buffer[1])
   {
      using namespace ssd1306_constants;

      // place the 0x40 prefix
      _buffer[0] = 0x40;

      command(display_off);

      // Wait 100mS for DC-DC to stabilise. This can probably be reduced
      delay_ms(100);

      command(set_display_clock_div);
      command(0x80);

      command(set_multiplex);
      command(height - 1);

      command(set_display_offset);
      command(0x0);                          // no offset
      command(set_start_line | 0x0);         // line #0
      command(charge_pump);
      command(0x14);
      command(memory_mode);
      command(0x00);                         // 0x0 act like ks0108
      command(seg_remap | 0x1);
      command(com_scan_dec);

      command(set_contrast);
      command(0xff);

      command(set_com_pins);
      if (width == 128 && height == 32)
         command(0x02);
      else if (width == 64 && height == 32)
         command(0x12);
      else if (width == 96 && height == 16)
         command(0x2);                       // ada x12

      command(set_pre_charge);
      command(0xf1);

      command(set_vcom_detect);
      command(0x40);
      command(display_all_on_resume);
      command(normal_display);

      command(deactivate_scroll);
      command(display_on);

      Canvas::clear();
      refresh();
   }

   template <typename Port, typename Canvas, std::size_t timeout>
   inline void ssd1306<Port, Canvas, timeout>::refresh()
   {
      using namespace ssd1306_constants;

      command(column_addr);
      command(0);                   // Column start address (0 = reset)
      command(width-1);             // Column end address (127 = reset)

      command(page_addr);
      command(0);                   // Page start address (0 = reset)

      if (height == 64)
          command(7);               // Page end address
      else if (height == 32)
          command(3);               // Page end address
      else if (height == 16)
          command(1);               // Page end address

      out(_buffer, buffer_size);
   }
}}

#endif
