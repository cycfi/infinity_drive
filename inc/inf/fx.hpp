/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_FILTER_HPP_DECEMBER_24_2015)
#define CYCFI_INFINITY_FILTER_HPP_DECEMBER_24_2015

#include <cstdint>
#include <cmath>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Leaky Integrator
   ////////////////////////////////////////////////////////////////////////////
   struct leaky_integrator
   {
      leaky_integrator(float a)
       : y(0)
       , a(a)
      {}

      float operator()(float s)
      {
         return y = s + a * (y - s);
      }

      float y;
      float a;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Basic one pole low-pass filter (6dB/Oct)
   ////////////////////////////////////////////////////////////////////////////
   struct low_pass
   {
      low_pass(float a)
       : y(0)
       , a(a)
      {}

      low_pass(float freq, uint32_t sps)
       : y(0)
       , a(1.0f - exp(-_2pi * freq/sps))
      {}

      float operator()(float s)
      {
         return y += a * (s - y);
      }

      float y;
      float a;
   };

   constexpr int32_t filter_freq(float freq, uint32_t sps)
   {
      return (1.0f - exp(-_2pi * freq/sps)) * 65536.0f;
   }

   ////////////////////////////////////////////////////////////////////////////
   // The peak detector will follow the envelope of a signal with gradual
   // decay (given by the d parameter). The signal decays exponentially if
   // the signal is below the peak.
   ////////////////////////////////////////////////////////////////////////////
   struct peak_detector
   {
      peak_detector(float d = 0.001f)
       : y(0)
       , d(d)
      {}

      float operator()(float s)
      {
         if (s > y)
            y = s;
         else
            y -= (y-s) * d;
         return y;
      }

      float y;
      float d;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The output of a simple comparator is determined by its inputs. The output
   // is high (1.0) if the positive input (spos) is greater than the negative
   // input (sneg). Otherwise, the output is low (-1.0).
   //
   // The schmitt trigger adds some hysteresis to improve noise immunity and
   // minimize multiple triggering by adding and subtracting a certain fraction
   // of the previous output (state) back to the positive input (spos).
   // hysteresis is the fraction (should be less than < 1.0) that determines
   // how much is added or subtracted. By doing so, the comparator "bar" is
   // raised or lowered depending on the previous state.
   ////////////////////////////////////////////////////////////////////////////
   struct schmitt_trigger
   {
      schmitt_trigger(float hysteresis)
       : state(-1.0f)
       , hysteresis(hysteresis)
      {}

      float operator()(float spos, float sneg)
      {
         auto delta = (state - spos) * hysteresis;
         return state = (spos + delta) > sneg ? 1.0f : -1.0f;
      }

      float state;
      float hysteresis;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Simple gain (amplifier).
   ////////////////////////////////////////////////////////////////////////////
   struct gain
   {
      constexpr gain(float a)
       : a(a)
      {}

      constexpr float operator()(float s) const
      {
         return s * a;
      }

      float a;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The peak_trigger generates pulses that coincide with the peaks of a
   // waveform. This is accomplished by sending the signal through a peak-
   // detector and comparing the result with the original signal (slightly
   // attenuated) using a schmitt_trigger.
   ////////////////////////////////////////////////////////////////////////////
   struct peak_trigger
   {
      peak_trigger(float d = 0.001f)
       : pk(d)
       , cmp(0.002f)
      {}

      float operator()(float s)
      {
         constexpr gain g{0.9f};
         return cmp(s, g(pk(s)));
      }

      peak_detector pk;
      schmitt_trigger cmp;
   };

   ////////////////////////////////////////////////////////////////////////////
   // clip a signal to range -max...+max
   ////////////////////////////////////////////////////////////////////////////
   struct clip
   {
      constexpr clip(float max = 1.0f)
       : max(max)
      {}

      constexpr float operator()(float s)
      {
         return (s > max) ? max : (s < -max) ? -max : s;
      }

      float max;
   };

   ////////////////////////////////////////////////////////////////////////////
   struct differentiator
   {
      differentiator()
       : x(0) {}

      float operator()(float s)
      {
         auto val = s - x;
         x = s;
         return val;
      }

      float x;
   };

   ////////////////////////////////////////////////////////////////////////////
   struct integrator
   {
      integrator()
       : y(0) {}

      float operator()(float s)
      {
         return y = s + y;
      }

      float y;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Fast Downsampling with antialiasing. A quick and simple method of
   // downsampling a signal by a factor of two with a useful amount of
   // antialiasing. Each source sample is convolved with { 0.25, 0.5, 0.25 }
   // before downsampling. (from http://www.musicdsp.org/)
   ////////////////////////////////////////////////////////////////////////////
   struct downsample
   {
      downsample()
       : state(0)
      {}

      uint16_t operator()(uint16_t s1, uint16_t s2)
      {
         auto out = state + (s1 >> 1);
         state = s2 >> 2;
         return out + state;
      }

      uint16_t state;
   };

   ////////////////////////////////////////////////////////////////////////////
   // window_comparator
   ////////////////////////////////////////////////////////////////////////////
   struct window_comparator
   {
      window_comparator(float low = -0.5f, float high = 0.5f)
       : low(low)
       , high(high)
       , state(1.0)
      {}

      float operator()(float s)
      {
         if (s < low)
            state = -1.0f;
         else if (s > high)
            state = 1.0f;
         return state;
      }

      float low;
      float high;
      float state;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Automatic gain control
   ////////////////////////////////////////////////////////////////////////////
   struct agc
   {
      agc(float threshold = 0.001f, float decay = 0.001f)
       : peak(decay)
       , threshold(threshold)
      {}

      float operator()(float s)
      {
         auto env = peak(std::fabs(s));
         if (env > threshold)
            return s / env;
         return 0.0f;
      }

      peak_detector peak;
      float threshold;
   };

   ////////////////////////////////////////////////////////////////////////////
   // DC blocker based on Julius O. Smith's document
   ////////////////////////////////////////////////////////////////////////////
   struct dc_block
   {
      dc_block(float r)
       : r(r)
       , x(0.0f)
       , y(0.0f)
      {}

      dc_block(float cutoff, uint32_t sps)
       : r(1.0f -(_2pi * cutoff/sps))
       , x(0.0f)
       , y(0.0f)
      {}

      float operator()(float s)
      {
         y = s - x + r * y;
         x = s;
         return y;
      }

      float r;
      float x;
      float y;
   };

}}

#endif
