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
using mode_button_type = inf::input_pin<portc + 10, pull_up>;

///////////////////////////////////////////////////////////////////////////////
// Our synthesizer
constexpr uint32_t sps = 20000;
constexpr float initial_modulator_gain = 0.0;
constexpr float initial_frequency = 110.0;

inf::fm synth(440.0, initial_modulator_gain, initial_frequency, sps);
inf::sin ref_synth(initial_frequency, sps);

inf::one_pole_lp freq_lp{10.0f, sps};  // frequency param-change filter (10Hz)
inf::one_pole_lp mod_lp{10.0f, sps};   // modulator param-change filter (10Hz)
inf::one_pole_lp phase_lp{10.0f, sps}; // phase param-change filter (10Hz)

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac1;
inf::dac<1> dac2;
inf::timer<3> tmr;
i2c_type i2c;
encoder_type enc;
mode_button_type mode_btn;

///////////////////////////////////////////////////////////////////////////////
inf::encoder_param<encoder_type> frequency_enc{enc, initial_frequency, 50, 1950, 0.1};
inf::encoder_param<encoder_type> modulation_enc{enc, initial_modulator_gain, 0, 1, 0.0001};
inf::encoder_param<encoder_type> phase_enc{enc, 0, -180, 360, 0.1};

enum class mode_enum : char
{
   frequency,
   modulation,
   phase
};

// mode_enum operator++(mode_enum& mode)
// {

// }

mode_enum mode = mode_enum::frequency;

// Mode change
void set_mode()
{
   switch (mode)
   {
      case mode_enum::frequency:
         frequency_enc.deactivate();
         modulation_enc.activate();
         mode = mode_enum::modulation;
         break;
      case mode_enum::modulation:
         modulation_enc.deactivate();
         phase_enc.activate();
         mode = mode_enum::phase;
         break;
      case mode_enum::phase:
         phase_enc.deactivate();
         frequency_enc.activate();
         mode = mode_enum::frequency;
         break;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Generate the DAC out
void generate()
{
   switch (mode)
   {
      case mode_enum::frequency:
         freq_lp(frequency_enc());
         synth.freq(freq_lp(), sps);
         ref_synth.freq(freq_lp(), sps);
         break;
      case mode_enum::modulation:
         // Update the synth modulator gain
         synth.modulator_gain(mod_lp(modulation_enc()));
         break;
      case mode_enum::phase:
         phase_lp(phase_enc());
         break;
   }

   // We generate a 12 bit signal, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signal to 0.9.

   // Generate our FM synth
   uint16_t val1 = (0.9f * synth() * 2047) + 2048;
   dac1(val1);

   // Generate our Reference synth
   uint16_t val2 = (0.9f * ref_synth() * 2047) + 2048;
   dac2(val2);
}

///////////////////////////////////////////////////////////////////////////////
// Configuration
constexpr uint32_t tmr_freq = 80000000;

auto config = inf::config(
   dac1.setup(),
   dac2.setup(),
   tmr.setup(tmr_freq, sps, generate), // calls generate every 20kHz
   i2c.setup(),
   enc.setup(),
   mode_btn.setup(set_mode, 10)
);

///////////////////////////////////////////////////////////////////////////////
// Display
void display(oled_type& cnv, char const* str, int val)
{
   cnv.clear();
   cnv.draw_string(str, 15, 8, font::medium);

   if (val < 99999)
   {
      char out[8];
      inf::to_string(val, out, 1);
      cnv.draw_string(out, 65, 8, font::medium);
   }
   else
   {
      cnv.draw_string("over", 65, 8, font::medium);
   }

   cnv.refresh();
}

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   oled_type cnv{i2c};
   enc(initial_modulator_gain);
   enc.start();
   tmr.start();
   mode_btn.start(rising_edge);  // call button_task on the rising edge

   phase_enc.activate();
   modulation_enc.activate();
   frequency_enc.activate();

   freq_lp.y = initial_frequency;
   mod_lp.y = initial_modulator_gain;

   while (true)
   {
      switch (mode)
      {
         case mode_enum::frequency:
            display(cnv, "Freq", std::round(freq_lp() * 10.0f));
            break;
         case mode_enum::modulation:
            display(cnv, "Mod", std::round(mod_lp() * 1000.0f));
            break;
         case mode_enum::phase:
            display(cnv, "Phase", std::round(phase_lp() * 10.0f));
            break;
      }
      delay_ms(100);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
