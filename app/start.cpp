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
#include <inf/rotary_encoder.hpp>
#include <cstring>

namespace inf = cycfi::infinity;
using namespace inf::monochrome;
using namespace inf::port;

using inf::delay_ms;
using canvas_type = inf::mono_canvas<128, 32>;
using i2c_type = inf::i2c_master<portb+10, portb+3>;
using oled_type = inf::ssd1306<i2c_type, canvas_type>;
using encoder_type = inf::rotary_encoder<portc+8, portc+9>;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
i2c_type i2c;
encoder_type enc;

///////////////////////////////////////////////////////////////////////////////
// Configuration.
auto config = inf::config(
   i2c.setup(),
   enc.setup()
);

void reverse(char s[])
{
   int i, j;
   char c;

   for (i = 0, j = std::strlen(s)-1; i<j; i++, j--) 
   {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
   }
}

void itoa(int n, char s[])
{
   int i, sign;

   if ((sign = n) < 0)  /* record sign */
       n = -n;          /* make n positive */
   i = 0;
   do {       /* generate digits in reverse order */
       s[i++] = n % 10 + '0';   /* get next digit */
   } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
       s[i++] = '-';
   s[i] = '\0';
   reverse(s);
}  

///////////////////////////////////////////////////////////////////////////////
// The main loop
float value = 0;

void start()
{
   oled_type cnv{i2c};
   enc.start();
      
   while (true)
   {
      char out[sizeof(int)*8+1];
      int value = enc() * 100.0f;
      itoa(value, out);

      cnv.clear();
      cnv.draw_string(out, 15, 15, font::medium);
      cnv.refresh();

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
