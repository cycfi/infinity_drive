/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "processor_test.hpp"
#include <inf/fx.hpp>

///////////////////////////////////////////////////////////////////////////////
// AGC test. Tests the AGC (automatic gain control).
//
// Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
// pin PA4 to an oscilloscope to see the waveform. 
///////////////////////////////////////////////////////////////////////////////

namespace inf = cycfi::infinity;
using inf::exp;
using inf::_2pi;
using inf::linear_interpolate;
using inf::inverse;

static constexpr auto clock = 64000;
static constexpr auto sps_div = 4;
static constexpr auto sps = clock / sps_div;

struct my_processor
{  
   static constexpr float max_gain = 1000.0f;
   static constexpr float low_freq = 1.0f - exp(-_2pi * 300.0f/sps);
   static constexpr float high_freq = 1.0f - exp(-_2pi * 0.5);
   
   float process(float val)
   {      
//      val *= 4;
      
      // DC block
      val = _dc_blk(val);

      // Automatic filter control
      _lp.a = {linear_interpolate(low_freq, high_freq, _ef())};
      val = _lp(val);

      // Envelope follower
      auto env = _ef(std::abs(val));
      
      // Noise gate      
      if (_ng(1.0f/max_gain, env))
         return 0.0f;

      // Automatic gain control
      inf::gain g = {inverse(env)};
      return g(val);      
   }
   
   inf::schmitt_trigger _ng = {0.001};
   inf::envelope_follower _ef = {0.0001f};
   inf::one_pole_lp _lp = {0.0f};
   inf::dc_block _dc_blk;
};

inf::mono_processor<inf::processor<my_processor, 2048, sps_div>, clock, 8> proc;
inf::output_pin<inf::port::portc + 3> pin; // portc, pin 3

void start()
{
   proc.start();
   while (true)
      inf::delay_ms(500);
}

inline void irq(adc_conversion_half_complete<1>)
{
   pin = 1;
   proc.irq_conversion_half_complete();
   pin = 0;
}

inline void irq(adc_conversion_complete<1>)
{
   pin = 1;
   proc.irq_conversion_complete();
   pin = 0;
}

void irq(timer_task<2>)
{
   proc.irq_timer_task();
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
