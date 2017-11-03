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

///////////////////////////////////////////////////////////////////////////////
// Rotary encoder test.
//
// Setup: Connect the rotary encoder quadrature A and B pins to PC8 and
//        PC9. The pins must be active low and hardware debounced. Connect
//        the ssd1306 oled I2C SCL to PB10 and SDA to PB3 to see the
//        actual value of the encoder go from 0 to 50 (initialized to 50).
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::monochrome;
using namespace inf::port;

using inf::delay_ms;
using canvas_type = inf::mono_canvas<128, 32>;
using i2c_type = inf::i2c_master<portb+10, portb+3>;
using oled_type = inf::ssd1306<i2c_type, canvas_type>;
using encoder_type = inf::rotary_encoder<portc+8, portc+9, 24 /*steps*/>;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
i2c_type i2c;
encoder_type enc{0.0001};

///////////////////////////////////////////////////////////////////////////////
// Configuration.
auto config = inf::config(
   i2c.setup(),
   enc.setup()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   oled_type cnv{i2c};
   enc(0.5);
   enc.start();

   while (true)
   {
      char out[8];
      int value = enc() * 10000.0f;
      inf::to_string(value, out, 2);

      cnv.clear();
      cnv.draw_string(out, 15, 15, font::medium);
      cnv.refresh();

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
