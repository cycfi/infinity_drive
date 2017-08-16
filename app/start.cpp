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

namespace inf = cycfi::infinity;
using namespace inf::port;

using inf::output_pin;
using inf::delay_ms;

void start()
{
   using canvas_type = inf::mono_canvas<128, 32>;
   using i2c_type = inf::i2c_master<3, porta+8, portc+9>;
   using oled_type = inf::ssd1306<i2c_type, canvas_type>;
   using namespace inf::monochrome;

   i2c_type 	i2c;
   oled_type 	cnv{ i2c };

#if 0 // Test some rects and line

   cnv.fill_rect(10, 10, 50, 50);
   cnv.draw_rect(8, 8, 54, 54);
   cnv.draw_line(0, 0, 128, 32, color::black);

#else // Test some text drawing

   cnv.draw_string("Hello, World!", 25, 0, font::small);
   cnv.draw_string("Hello, World!", 15, 15, font::medium);
   cnv.refresh();
   delay_ms(1000);

   for (int y = -30; y <= 0; ++y)
   {
      cnv.clear();
      cnv.draw_string("cycfi", 30, y, font::large);
      cnv.refresh();
      delay_ms(50);
   }

   cnv.bright(false);
   for (int v = 255; v >= 0; --v)
   {
      cnv.brightness(v);
      delay_ms(1);
   }

   cnv.bright();
   delay_ms(1000);

#endif

   while (true)
      ;
}
