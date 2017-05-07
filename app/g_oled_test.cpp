/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/oled.hpp>
#include <inf/app.hpp>
#include <algorithm>

namespace inf = cycfi::infinity;

using inf::output_pin;
using inf::delay_ms;
using inf::portb;
using inf::on;
using inf::off;

void start()
{
   inf::canvas<128, 64> cnv;

   using oled_SH1106 =
      inf::oled_SH1106<
         1,
         portb+3,    // SCK
         portb+5,    // MOSI
         portb+13,   // Reset,
         portb+14,   // DC
         portb+15    // CS
      >;

   oled_SH1106 oled{cnv};

   cnv._buffer[1] = 0xff;
   cnv._buffer[3] = 0xff;
   cnv._buffer[5] = 0xff;
   cnv._buffer[7] = 0xff;

   oled.refresh();

   //for (int i = 255; i > 0; --i)
   //{
   //   delay_ms(10);
   //   oled.contrast(i);
   //}
   //oled.off();

   while (true)
      ;
}


