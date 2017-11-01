/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/dac.hpp>
#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/ssd1306.hpp>
#include <inf/rotary_encoder.hpp>

///////////////////////////////////////////////////////////////////////////////
// This test generates an 440Hz sine wave FM modulated by a 110Hz sine wave.
// The modulation amount is controlled by a rotary encoder.
//
// Setup: Connect pin PA4 to an oscilloscope to see the generated waveform.
//        Connect the rotary encoder quadrature A and B pins to PC8 and
//        PC9. The pins must be active low and hardware debounced. Connect
//        the ssd1306 oled I2C SCL to PB10 and SDA to PB3 to see the
//        actual value of the encoder go from 0 to 50 (initialized to 50).
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using namespace inf::port;
using namespace inf::monochrome;

using inf::delay_ms;
using canvas_type = inf::mono_canvas<128, 32>;
using i2c_type = inf::i2c_master<portb+10, portb+3>;
using oled_type = inf::ssd1306<i2c_type, canvas_type>;
using encoder_type = inf::rotary_encoder<portc+8, portc+9, 24 /*steps*/>;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 100000;
constexpr float initial_modulator_gain = 0.2;

inf::fm synth(440.0, initial_modulator_gain, 110.0, sps);

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac;
inf::timer<3> tmr;
i2c_type i2c;
encoder_type enc;

///////////////////////////////////////////////////////////////////////////////
// Our timer task
void timer_task()
{
   // We generate a 12 bit signal, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signal to 0.9.
   uint16_t val = (0.9f * synth() * 2047) + 2048;
   dac(val);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac.setup(),
   tmr.setup(tmr_freq, sps, timer_task),  // calls timer_task every 100kHz
   i2c.setup(),
   enc.setup()
);

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   oled_type cnv{i2c};
   enc(initial_modulator_gain);
   enc.start();
   tmr.start();

   while (true)
   {
      char out[sizeof(int)*8+1];
      int value = enc() * 100.0f;
      inf::itoa(value, out);

      cnv.clear();
      cnv.draw_string(out, 15, 15, font::medium);
      cnv.refresh();

      // Update the synth modulator gain
      synth.modulator_gain(enc());

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
