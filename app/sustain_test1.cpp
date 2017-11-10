/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/multi_processor.hpp>
#include <inf/app.hpp>
#include <inf/support.hpp>
#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/ssd1306.hpp>
#include <inf/rotary_encoder.hpp>
#include <inf/pid.hpp>
#include <q/synth.hpp>
#include "freq_locked_synth.hpp"

///////////////////////////////////////////////////////////////////////////////
// Frequency-Locked Synthesizer test
//
// The freq_locked_synth looks at the input audio and extracts the
// fundamental frequency and phase from the waveform and uses these
// information to set the frequency and phase of a sine-wave synthesiser.
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pins PA4 and PA5 to an oscilloscope to see the input and output waveforms.
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
// Peripherals
i2c_type i2c;
encoder_type enc;
mode_button_type mode_btn;

///////////////////////////////////////////////////////////////////////////////
inf::encoder_param<encoder_type>
   level_enc{enc, 0.5, 0, 1, 0.001};

inf::encoder_param<encoder_type>
   modulation_enc{enc, 0, 0, 1, 0.001};

inf::encoder_param<encoder_type>
   factor_enc{enc, 0, 0, 8, 0.001};

enum class mode_enum : char
{
   level,
   modulation,
   factor
};

mode_enum mode = mode_enum::level;

// Mode change
void set_mode()
{
   switch (mode)
   {
      default:
      case mode_enum::level:
         level_enc.deactivate();
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
         level_enc.activate();
         mode = mode_enum::level;
         break;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Our multi-processor
static constexpr auto clock = 80000;
static constexpr auto sps_div = 4;
static constexpr auto sps = clock / sps_div;

struct my_processor
{
   static constexpr auto oversampling = sps_div;
   static constexpr auto adc_id = 1;
   static constexpr auto timer_id = 2;
   static constexpr auto channels = 1;
   static constexpr auto sampling_rate = clock;
   static constexpr auto buffer_size = 1024;
   static constexpr auto latency = buffer_size / sps_div;
   static constexpr auto start_phase = q::osc_phase(0);

   void process(std::array<float, 2>& out, float s, std::uint32_t channel)
   {
      auto synth_out = _fls(s, _ticks++);

      out[0] = s;
      out[1] = synth_out * _level;
   }

   using sin_synth = decltype(q::sin(1.0, sps, 0.0));
   using fls_type = inf::freq_locked_synth<sin_synth, sps, latency>;

   sin_synth   _synth = q::sin(0.0, sps, 0.0);
   fls_type    _fls = {_synth, start_phase};
   uint32_t    _ticks = 0;
   float       _level = 0;
};

inf::multi_channel_processor<inf::processor<my_processor>> proc;

///////////////////////////////////////////////////////////////////////////////
struct level_pid_config
{
   float static constexpr p = 0.05f;         // Proportional gain
   float static constexpr i = 0.0f;          // Integral gain
   float static constexpr d = 0.03f;         // derivative gain
   float static constexpr sps = 100.0f;      // 100 Hz
};

inf::pid<level_pid_config> level_pid;

///////////////////////////////////////////////////////////////////////////////
// Configuration
auto config = inf::config(
   i2c.setup(),
   enc.setup(),
   mode_btn.setup(set_mode, 10),
   proc.config<0>()
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
   proc.start();
   oled_type cnv{i2c};
   enc.start();
   mode_btn.start(rising_edge);  // call button_task on the rising edge

   factor_enc.activate();
   modulation_enc.activate();
   level_enc.activate();

   while (true)
   {
      switch (mode)
      {
         case mode_enum::level:
            display(cnv, "Level", std::round(level_enc() * 1000.0f), 2);
            break;
         case mode_enum::modulation:
            display(cnv, "---", std::round(modulation_enc() * 1000.0f), 2);
            break;
         case mode_enum::factor:
            display(cnv, "---", std::round(factor_enc() * 1000.0f), 2);
            break;
      }

      // Set the sustain level
      constexpr q::clip clamp{ 1.0f };
      proc._level += level_pid(level_enc()/8, proc._fls.envelope());
      proc._level = clamp(proc._level);

      delay_ms(10);
   }
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
