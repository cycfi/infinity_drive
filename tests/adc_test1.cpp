/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>
#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/adc.hpp>
#include <inf/ssd1306.hpp>
#include <inf/fx.hpp>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Test the adc. 
// 
// Setup: Connect a 3.3v voltage divider using a potentiometer to pin   
//        PA4. Connect the ssd1306 oled I2C SCL to PB10 and SDA to 
//        PB3 and see the ADC value change when you move the pot.
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::monochrome;
using namespace inf::port;

using inf::delay_ms;
using canvas_type = inf::mono_canvas<128, 32>;
using i2c_type = inf::i2c_master<portb+10, portb+3>;
using oled_type = inf::ssd1306<i2c_type, canvas_type>;

constexpr int sampling_rate = 16000;

///////////////////////////////////////////////////////////////////////////////
// Peripherals
i2c_type i2c;
inf::timer<2> clock;
inf::adc<1, 1> adc;

///////////////////////////////////////////////////////////////////////////////
// ADC conversion complete task
volatile int32_t adc_val = 0;
inf::one_pole_lp lp{10.0f, sampling_rate}; // 10Hz low pass filter

inline void conversion_complete()
{
   for (int i = 0; i < 8; ++i)
      adc_val = lp(adc[i][0]);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   i2c.setup(),
   clock.setup(2000000, sampling_rate),
   adc.setup(clock, conversion_complete),
   adc.enable_channels<4>()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop.
void start()
{
   oled_type cnv{i2c};
   adc.start();
   clock.start();

   while (true)
   {
      char out[sizeof(int)*8+1];
      inf::itoa(adc_val, out);

      cnv.clear();
      cnv.draw_string(out, 15, 15, font::medium);
      cnv.refresh();

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
