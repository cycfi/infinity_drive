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
#include <inf/period_detector.hpp>

#include "diag/Trace.h"

namespace inf = cycfi::infinity;
using inf::portc;
using inf::on;
using inf::off;

constexpr uint32_t adc_sps = 31250;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/(adc_sps/2));
constexpr unsigned buffer_size = 1024;

typedef inf::adc<1, 1, buffer_size> adc_type;
adc_type ad_conv(adc_clock);

volatile float g_pitch = 0;
inf::period_detector<adc_sps> pd;

inf::output_pin<portc + 6> port_c6;

void start()
{
   // ADC channel 11, portc pin 1
   ad_conv.enable_channel<11, portc + 1, 1>();

   ad_conv.enable();
   adc_clock.enable();
   ad_conv.enable_interrupt(0);

   while (true)
   {
      trace_printf("Pitch: %f\n", g_pitch);
   }
}

template <typename Iterator>
inline void detect_pitch(Iterator first, Iterator last, std::size_t channel)
{
   for (std::size_t i = 0; first != last; ++i)
   {
      float val = ((*first++)[channel] / 2048.0f) - 1.0f;
      val = pd(val);

      g_pitch = val;
   }
}

inline void irq(adc_conversion_half_complete)
{
   port_c6 = on;
   detect_pitch(ad_conv.begin(), ad_conv.middle(), 0);
   port_c6 = off;
}

inline void irq(adc_conversion_complete)
{
   port_c6 = on;
   detect_pitch(ad_conv.middle(), ad_conv.end(), 0);
   port_c6 = off;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
