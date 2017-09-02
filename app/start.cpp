/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>
#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/ssd1306.hpp>
#include <inf/mpr121.hpp>

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::output_pin;
using inf::delay_ms;

void start()
{
   using canvas_type = inf::mono_canvas<128, 32>;
   using i2c_type = inf::i2c_master<portb+10, portb+3>;
   using oled_type = inf::ssd1306<i2c_type, canvas_type>;
   using touch_pad_type = inf::mpr121<i2c_type>;
   using namespace inf::monochrome;

   i2c_type 	   i2c;
   oled_type 	   cnv{ i2c };
   touch_pad_type tpad{ i2c };

   char status1[] = "__-__-__-__-__-__-";
   char status2[] = "__-__-__-__-__-__-";

   while (true)
   {
      uint16_t touched = tpad.touched();
      char* p = status1;
      for (int i = 0; i != 12; ++i)
      {
         if (i == 6)
            p = status2;
         if (touched & (1 << i))
         {
            *p++ = (i >= 10)? '1' : '0';
            *p++ = '0' + (i % 10);
            p++;
         }
         else
         {
            *p++ = '_';
            *p++ = '_';
            p++;
         }
      }

      cnv.clear();
      cnv.draw_string(status1, 0, 0, font::small);
      cnv.draw_string(status2, 0, 15, font::small);
      cnv.refresh();
      delay_ms(10);
   }
}
