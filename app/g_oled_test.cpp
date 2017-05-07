/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/spi.hpp>
#include <inf/app.hpp>
#include <algorithm>

namespace inf = cycfi::infinity;

using inf::output_pin;
using inf::delay_ms;
using inf::portb;
using inf::on;
using inf::off;
using inf::spi_master;

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
      ram_pages   = page_end - page_start + 1,

      width       = 128,
      height       = 64
   };

   std::uint8_t image_buffer[(width * height) / 8];
}

void clear()
{
   for (auto& b : image_buffer)
      b = 0;
}

void start()
{
   clear();

   output_pin<portb+13> rst;
   output_pin<portb+14> dc;
   output_pin<portb+15> cs;

   // SPI master: SCK: portb+3, MOSI: portb+5
   spi_master<1, portb+3, portb+5> spi{};

   auto command = [&](std::uint8_t cmd)
   {
      dc = 0;
      cs = 0;
      spi.write(cmd);
      while (spi.is_writing())
         ;
      cs = 1;
   };

   auto reset = [&]()
   {
      rst = 0;
      delay_ms(1);
      rst = 1;

      // Wait 100mS for DC-DC to stabilise. This can probably be reduced
      delay_ms(100);

      command(set_mux_ratio); //Set MUX ratio
      command(0x3F);

      command(charge_pump); //Enable charge pump
      command(enable_charge_pump);

      // Set memory addressing mode to horizontal addressing
      command(memory_mode);
      command(horizontal_addr_mode);

      // Clear the display buffer
      // ClearBuffer();

      // Output the display buffer to clear the display RAM
      // Refresh();

      // Turn display on
      command(display_on | 1);

      // Wait 150mS for display to turn on
      delay_ms(150);
   };

   auto draw = [&](std::uint8_t data)
   {
      cs = 0;
      spi.write(data);
      while (spi.is_writing())
         ;
      cs = 1;
   };

   spi.enable();
   reset();

//	command(set_col_addr);
//	command(col_start);
//	command(col_end);
//
//   /* set graphics ram start and end pages */
//	command(set_page_addr);
//	command(page_start);
//	command(page_end);

   image_buffer[1] = 0xff;
   image_buffer[3] = 0xff;
   image_buffer[5] = 0xff;
   image_buffer[7] = 0xff;

   auto* p = image_buffer;
   for (auto row = 0; row < ram_pages; row++)
   {
      command(page | row);
      command(set_col_low | col_start); /* Lower column address */
      command(set_col_high | 0x0); /* Higher column address */

      dc = 1;
      for (auto col = 0; col < width; col++)
      {
         draw(*p++);
      }
   }


   //while (true)
   //{
   //   draw(0);
   //   //draw(0);
   //}
}
