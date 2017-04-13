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

#include "diag/Trace.h"
#include <cmath>

// ADC and PWM test. We generate a signal and send it through portc pin 6
// as PWM. The filtered PWM signal is then sent to the ADC channel 11,
// portc pin 1. This signal is then sent down as PWM to portc pin 7.

namespace inf = cycfi::infinity;
using inf::portc;

constexpr uint32_t adc_sps = 32000;
constexpr uint32_t adc_clock_rate = 2000000;

inf::timer<8> adc_clock(adc_clock_rate, adc_clock_rate/adc_sps);

typedef inf::adc<1, 1, 1024> adc_type;
adc_type ad_conv(adc_clock);
inf::dac dac;

float g_pitch = 0;

void start()
{
   // ADC channel 11, portc pin 1
   ad_conv.enable_channel<11, portc + 1, 1>();

   ad_conv.enable();
   adc_clock.enable();
   ad_conv.enable_interrupt();

   while (true)
   {
      inf::delay_ms(500);
      //trace_printf("Pitch: %f\n", g_pitch);
   }
}

struct period_detector
{
   struct peak_info
   {
      float val;
      unsigned index;
   };

   enum peak_state { high, low, none };

   period_detector()
    : lp1(1500/*Hz*/, adc_sps)
    , lp2(1500/*Hz*/, adc_sps)
    , num_peaks(0)
    , state(none)
   {}

   void reset()
   {
      num_peaks = 0;
      state = none;
   }

   void collect_peaks(adc_type::stride samples, unsigned offset)
   {
      for (unsigned i = 0; i < samples.size(); ++i)
      {
         float val = (samples[i] / 2048.0f) - 1.0f;

         // lowpass filter
         val = lp2(lp1(val));

         // peak triggers
         auto valp = peak_pos(val);    // positive peaks
         auto valn = peak_neg(-val);   // negative peaks

         if (valp > 0 || valn > 0)
         {
            auto new_state = peak_state(valp > 0);
            if (state != new_state)
            {
               // we got a new peak
               peaks[num_peaks++] = peak_info{ val, i+offset };
               if (num_peaks >= max_peaks)
                  break;
               state = new_state;
            }
         }
         else
         {
            state = none;
         }
      }
   }

   void collect_peaks(adc_type::stride samples, adc_conversion_half_complete)
   {
      collect_peaks(samples, 0);
   }

   void collect_peaks(adc_type::stride samples, adc_conversion_complete)
   {
      collect_peaks(samples, samples.size());
   }

   float detect_pitch()
   {
      unsigned half = num_peaks/2;
      float period = 0;
      unsigned prev_index = peaks[0].index;
      unsigned num_periods = 0;

      for (unsigned i = 1; i < half; ++i)
      {
         float sum = 0;
         for (unsigned j = 0; j < half; ++j)
            sum += std::fabs(peaks[j].val - peaks[i+j].val);

         if (sum < threshold)
         {
            ++num_periods;
            float new_period = peaks[i].index - prev_index;
            prev_index = peaks[i].index;
            period += new_period;
         }
      }
      period /= float(num_periods);
      return float(adc_sps) / period;
   }

   inf::low_pass lp1;
   inf::low_pass lp2;

   inf::peak_trigger peak_pos;
   inf::peak_trigger peak_neg;

   static constexpr float threshold = 0.01;
   static constexpr unsigned max_peaks = 16;

   peak_info peaks[max_peaks];
   unsigned num_peaks;
   peak_state state;
};

period_detector pd;

inline void irq(adc_conversion_half_complete phase)
{
   auto stride = ad_conv.get_stride(phase, 1);
   pd.reset();
   pd.collect_peaks(stride, phase);
}

inline void irq(adc_conversion_complete phase)
{
   static uint16_t arr[1024];
   for (unsigned i = 0; i < 1024; ++i)
      arr[i] = ad_conv.values[0][i];

   auto stride = ad_conv.get_stride(phase, 1);
   pd.collect_peaks(stride, phase);
   float pitch = pd.detect_pitch();
   g_pitch = (g_pitch + pitch) / 2;
}

// The actual "C" interrupt handlers are defined here:
#include <inf/irq.hpp>
