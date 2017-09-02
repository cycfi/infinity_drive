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
   using namespace inf::monochrome;

   i2c_type 	i2c;
   oled_type 	cnv{ i2c };

   char status1[] = "01-02-03-04-05-06-";
   char status2[] = "07-08-09-10-11-12-";
   bool touch[12] = { false, false, true, true, false, false, true, true, false, false, true, true };

   while (true)
   {
      char* p = status1;
      for (int i = 0; i != 12; ++i)
      {
         auto n = i+1;
         if (n == 7)
            p = status2;
         if (touch[i])
         {
            *p++ = (n >= 10)? '1' : '0';
            *p++ = '0' + (n % 10);
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
      cnv.draw_string(status1, 10, 0, font::small);
      cnv.draw_string(status2, 10, 15, font::small);
      cnv.refresh();
      delay_ms(100);
   }
}
