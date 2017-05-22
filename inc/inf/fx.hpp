/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_FX_HPP_DECEMBER_24_2015)
#define CYCFI_INFINITY_FX_HPP_DECEMBER_24_2015

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{   
   ////////////////////////////////////////////////////////////////////////////
   // Basic one pole low-pass filter (6dB/Oct)
   ////////////////////////////////////////////////////////////////////////////
   struct one_pole_lp
   {
      // y: current value
      // a: coefficient

      one_pole_lp(float a)
       : y(0.0f), a(a)
      {}

      one_pole_lp(float freq, uint32_t sps)
       : y(0.0f), a(1.0f - exp(-_2pi * freq/sps))
      {}

      float operator()(float s)
      {
         return y += a * (s - y);
      }

      float y, a;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The envelope follower will follow the envelope of a signal with gradual
   // decay (given by the d parameter). The signal decays exponentially if
   // the signal is below the peak.
   ////////////////////////////////////////////////////////////////////////////
   struct envelope_follower
   {
      // y: current value
      // d: decay
      
      envelope_follower(float d = 0.001f)
       : y(0.0f), d(d)
      {}

      float operator()(float s)
      {
         if (s > y)
            y = s;
         else
            y -= (y-s) * d;
         return y;
      }
      
      float operator()() const
      {
         return y;
      }

      float y, d;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The output of a simple comparator is determined by its inputs. The output
   // is high (1) if the positive input (spos) is greater than the negative
   // input (sneg). Otherwise, the output is low (0).
   //
   // The schmitt trigger adds some hysteresis (h) to improve noise immunity 
   // and minimize multiple triggering by adding and subtracting a certain 
   // fraction of the previous output (y) back to the positive input (spos). 
   // hysteresis is the fraction (should be less than < 1.0) that determines 
   // how much is added or subtracted. By doing so, the comparator "bar" is 
   // raised or lowered depending on the previous state.
   //
   // The result is a bool.
   ////////////////////////////////////////////////////////////////////////////
   struct schmitt_trigger
   {
      // y: current value
      // h: hysteresis

      schmitt_trigger(float h)
       : y(0.0f), h(h)
      {}

      bool operator()(float spos, float sneg)
      {
         auto delta = (y - spos) * h;
         y = (spos + delta) > sneg;
         return y;
      }

      bool y;
      float h;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Simple gain (amplifier).
   ////////////////////////////////////////////////////////////////////////////
   struct gain
   {
      // g: gain

      constexpr gain(float g)
       : g(g)
      {}

      constexpr float operator()(float s) const
      {
         return s * g;
      }

      float g;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The peak_trigger generates pulses that coincide with the peaks of a
   // waveform. This is accomplished by sending the signal through an envelope
   // follower and comparing the result with the original signal (slightly
   // attenuated) using a schmitt_trigger.
   //
   // The result is a bool corresponding to the peaks.
   ////////////////////////////////////////////////////////////////////////////
   struct peak_trigger
   {      
      peak_trigger(float d = 0.001f)
       : ef(d), cmp(0.002f)
      {}

      bool operator()(float s)
      {
         constexpr gain g{0.70f};
         return cmp(s, g(ef(s)));
      }

      envelope_follower ef;
      schmitt_trigger cmp;
   };

   ////////////////////////////////////////////////////////////////////////////
   // clip a signal to range -m...+m
   ////////////////////////////////////////////////////////////////////////////
   struct clip
   {
      // m: maximum value

      constexpr clip(float m = 1.0f)
       : m(m)
      {}

      constexpr float operator()(float s)
      {
         return (s > m) ? m : (s < -m) ? -m : s;
      }

      float m;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The differentiator returns the time derivative of the input (s).
   ////////////////////////////////////////////////////////////////////////////
   struct differentiator
   {
      // x: delayed input sample

      differentiator()
       : x(0.0f) {}

      float operator()(float s)
      {
         auto val = s - x;
         x = s;
         return val;
      }

      float x;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The integrator accumulates the input samples (s).
   ////////////////////////////////////////////////////////////////////////////
   struct integrator
   {
      // y: current value

      integrator()
       : y(0.0f) {}

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
   //
   // This class is templated on the native integer sample type 
   // (e.g. uint16_t).
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   struct downsample
   {
      downsample()
       : x(0.0f)
      {}

      T operator()(T s1, T s2)
      {
         auto out = x + (s1 >> 1);
         x = s2 >> 2;
         return out + x;
      }

      T x;
   };
   
   ////////////////////////////////////////////////////////////////////////////
   // window_comparator. If input (s) exceeds a high threshold (h), the
   // current state (y) becomes 1. Else, if input (s) is below a low
   // threshold (l), the current state (y) becomes 0. If the state (s) 
   // is in between the low and high thresholds, the previous state is kept.
   ////////////////////////////////////////////////////////////////////////////
   struct window_comparator
   {
      // l: low threshold
      // h: high threshold
      // y: current state

      window_comparator(float l = -0.5f, float h = 0.5f)
       : l(l), h(h), y(1.0f)
      {}

      bool operator()(float s)
      {
         if (s < l)
            y = 0;
         else if (s > h)
            y = 1;
         return y;
      }

      float l, h;
      bool y;
   };

   ////////////////////////////////////////////////////////////////////////////
   // DC blocker based on Julius O. Smith's document
   ////////////////////////////////////////////////////////////////////////////
   struct dc_block
   {
      // y: current value
      // x: delayed input sample
      // r: pole 
      
      // A smaller r value allows faster tracking of "wandering dc levels", 
      // but at the cost of greater low-frequency attenuation.

      dc_block(float r = 0.995)
       : r(r), x(0.0f), y(0.0f)
      {}

      dc_block(float cutoff, uint32_t sps)
       : r(1.0f - (_2pi * cutoff/sps)), x(0.0f), y(0.0f)
      {}

      float operator()(float s)
      {
         y = s - x + r * y;
         x = s;
         return y;
      }

      float r, x, y;
   };
}}

#endif
