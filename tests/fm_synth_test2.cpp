/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <q/synth.hpp>
#include <q/fx.hpp>
#include <inf/app.hpp>
#include <inf/dac.hpp>
#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/ssd1306.hpp>
#include <inf/rotary_encoder.hpp>

///////////////////////////////////////////////////////////////////////////////
// This test generates a sine wave modulated by another sine wave. The
// frequency, modulation amount, and modulation frequency factor can be
// modified using the rotary encoder. A reference sine wave is also
// generated to make it easy to sync the scope.
//
// Setup: Connect pin PA4 and A5 to an oscilloscope to see the generated
//        waveforms. Connect the rotary encoder quadrature A and B pins to
//        PC8 and PC9. The pins must be active low and hardware debounced.
//        Connect the ssd1306 oled I2C SCL to PB10 and SDA to PB3 to see
//        the actual value of the encoder go from 0 to 50 (initialized to 50).
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
namespace q = cycfi::q;
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
constexpr uint32_t sps = 40000;
constexpr float initial_modulator_gain = 0.2;
constexpr float initial_frequency = 110.0;
constexpr float initial_factor = 4.0;

q::one_pole_lp freq_lp{10.0f, sps};    // frequency param-change filter (10Hz)
q::one_pole_lp mod_lp{10.0f, sps};     // modulator param-change filter (10Hz)
q::one_pole_lp factor_lp{10.0f, sps};  // factor param-change filter (10Hz)

auto synth = q::fm(initial_frequency, initial_modulator_gain, initial_factor, sps);
auto ref_synth = q::sin(initial_frequency, sps);

///////////////////////////////////////////////////////////////////////////////
// Peripherals
inf::dac<0> dac1;
inf::dac<1> dac2;
inf::timer<3> tmr;
i2c_type i2c;
encoder_type enc;
mode_button_type mode_btn;

///////////////////////////////////////////////////////////////////////////////
inf::encoder_param<encoder_type>
   frequency_enc{enc, initial_frequency, 50, 1950, 0.1};

inf::encoder_param<encoder_type>
   modulation_enc{enc, initial_modulator_gain, 0, 1, 0.0001};

inf::encoder_param<encoder_type>
   factor_enc{enc, initial_factor, 0, 8, 0.001};

enum class mode_enum : char
{
   frequency,
   modulation,
   factor
};

mode_enum mode = mode_enum::frequency;

// Mode change
void set_mode()
{
   switch (mode)
   {
      default:
      case mode_enum::frequency:
         frequency_enc.deactivate();
         modulation_enc.activate();
         mode = mode_enum::modulation;
         break;
      case mode_enum::modulation:
         modulation_enc.deactivate();
         factor_enc.activate();
         mode = mode_enum::factor;
         break;
      case mode_enum::factor:
         factor_enc.deactivate();
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
      case mode_enum::frequency:    // Update the synth frequency
      {
         auto freq = freq_lp(frequency_enc());
         synth.freq(q::osc_freq(freq, sps));
         ref_synth.freq(q::osc_freq(freq, sps));
      }
      break;

      case mode_enum::modulation:   // Update the FM modulator gain
      {
         mod_lp(modulation_enc());
         auto mgain = q::fm_gain(mod_lp());
         synth.mgain(mgain);
      }
      break;

      case mode_enum::factor:       // Update the FM frequency factor
      {
         factor_lp(factor_enc());
         synth.mfactor(factor_lp());
      }
      break;
  }

   // We generate a 12 bit signal, but we do not want to saturate the
   // DAC output buffer (the buffer is not rail-to-rail), so we limit
   // the signal to 0.8.

   // Generate our FM synth
   uint16_t val1 = (0.8f * synth() * 2047) + 2048;
   dac1(val1);

   // Generate our Reference synth
   uint16_t val2 = (0.8f * ref_synth() * 2047) + 2048;
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
void display(oled_type& cnv, char const* str, int val, int frac)
{
   cnv.clear();
   cnv.draw_string(str, 15, 8, font::medium);

   if (val < 99999)
   {
      char out[8];
      inf::to_string(val, out, frac);
      cnv.draw_string(out, 70, 8, font::medium);
   }
   else
   {
      cnv.draw_string("over", 70, 8, font::medium);
   }

   cnv.refresh();
}

///////////////////////////////////////////////////////////////////////////////
// The main loop
void start()
{
   oled_type cnv{i2c};
   enc.start();
   tmr.start();
   mode_btn.start(rising_edge);  // call button_task on the rising edge

   factor_enc.activate();
   modulation_enc.activate();
   frequency_enc.activate();

   while (true)
   {
      switch (mode)
      {
         case mode_enum::frequency:
            display(cnv, "Freq", std::round(frequency_enc() * 10.0f), 1);
            break;
         case mode_enum::modulation:
            display(cnv, "Mod", std::round(modulation_enc() * 1000.0f), 1);
            break;
         case mode_enum::factor:
            display(cnv, "Factor", std::round(factor_enc() * 1000.0f), 3);
            break;
      }
      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
