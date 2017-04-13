/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/synth.hpp>
#include <inf/fx.hpp>
#include <inf/app.hpp>
#include <inf/adc.hpp>
#include <inf/dac.hpp>
#include <inf/dbuff.hpp>
#include <inf/pitch_detector.hpp>
#include <inf/delay.hpp>

#include "diag/Trace.h"

namespace inf = cycfi::infinity;
using inf::portc;
using inf::on;
using inf::off;

constexpr uint32_t adc_sps = 31250;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/(adc_sps/2));
inf::timer<3> dac_clock(adc_clock_rate, adc_clock_rate/adc_sps);

constexpr unsigned buffer_size = 1024;

typedef inf::adc<1, 1, buffer_size> adc_type;
adc_type ad_conv(adc_clock);

inf::dac dac;

typedef inf::dbuff<float, buffer_size/2> obuff_type;
obuff_type obuff;

volatile auto out_first = obuff.end();
volatile auto out_last = obuff.end();

inf::output_pin<portc + 6> port_c6;

void start()
{
   // ADC channel 11, portc pin 1
   ad_conv.enable_channel<11, portc + 1, 1>();

   ad_conv.enable();
   adc_clock.enable();
   ad_conv.enable_interrupt(1);

   // wait for the first set of samples to come in
   while (out_first == out_last)
      ;

   dac_clock.enable();
   dac_clock.enable_interrupt(0);

   while (true)
   {
   }
}

inf::period_detector<adc_sps> pd;
inf::single_delay<float> d(512);

template <typename I1, typename I2>
inline void adc_copy(I1 first, I1 last, I2 src)
{
   float period = 512.0f;
   for (auto i = first; i != last; ++i)
   {
      float val = ((*src++)[0] / 2048.0f) - 1.0f;
      period = pd(val);
      val = d(val);
      *i = val;
   }
   d.samples_delay((std::ceil(buffer_size / period) * period) - buffer_size);
}

inline void irq(adc_conversion_half_complete)
{
   port_c6 = on;
   out_last = obuff.end();
   out_first = obuff.middle();
   adc_copy(obuff.begin(), obuff.middle(), ad_conv.begin());
   port_c6 = off;
}

inline void irq(adc_conversion_complete)
{
   port_c6 = on;
   out_last = obuff.middle();
   out_first = obuff.begin();
   adc_copy(obuff.middle(), obuff.end(), ad_conv.middle());
   port_c6 = off;
}

void irq(timer_task<3>)
{
   //if (out_first == out_last)
   //{
   //   dac(0);
   //   return;
   //}

   // We generate a 12 bit signal
   dac((*out_first++ * 2047) + 2048);
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
