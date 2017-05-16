/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/oled.hpp>
#include <inf/app.hpp>

///////////////////////////////////////////////////////////////////////////////
// Test the oled_SH1106 driver and canvas class. Just draw some stuff!!
//
// Setup: Connect the OLED to SPI plus some ports as follows:
//    PB3:  SCK (D0)
//    PB5:  MOSI (D1)
//    PB13: RST
//    PB14: DC
//    PB15: CS
//
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::output_pin;
using inf::delay_ms;

void start()
{
   inf::canvas<128, 64> cnv;

   using oled_SH1106 =
      inf::oled_SH1106<
         1,
         portb+3,    // SCK (D0)
         portb+5,    // MOSI (D1)
         portb+13,   // RST,
         portb+14,   // DC
         portb+15    // CS
      >;

   oled_SH1106 oled{cnv};

#if 0 // Test some rects and line

   cnv.fill_rect(10, 10, 50, 50);
   cnv.draw_rect(8, 8, 54, 54);
   cnv.draw_line(0, 0, 128, 64, inf::color::black);

#else // Test some text drawing

   cnv.draw_string("Hello, World!", 25, 0, inf::font::small);
   cnv.draw_string("Hello, World!", 10, 15, inf::font::medium);
   cnv.draw_string("cycfi", 25, 30, inf::font::large);

#endif

   oled.refresh();

   while (true)
      ;
}
