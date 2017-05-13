/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_FILTER_HPP_DECEMBER_24_2015)
#define CYCFI_INFINITY_FILTER_HPP_DECEMBER_24_2015

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
      // a: coeefficient

      one_pole_lp(float a)
       : y(0)
       , a(a)
      {}

      one_pole_lp(float freq, uint32_t sps)
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
      
      float operator()() const
      {
         return y;
      }

      float y;
      float d;
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
       : y(-1.0f)
       , h(h)
      {}

      bool operator()(float spos, float sneg)
      {
         auto delta = (y - spos) * h;
         return (y = (spos + delta) > sneg);
      }

      float y;
      float h;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Simple gain (amplifier).
   ////////////////////////////////////////////////////////////////////////////
   struct gain
   {
      // a: gain

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
   // waveform. This is accomplished by sending the signal through an envelope
   // follower and comparing the result with the original signal (slightly
   // attenuated) using a schmitt_trigger.
   //
   // The result is a bool corresponding to the peaks.
   ////////////////////////////////////////////////////////////////////////////
   struct peak_trigger
   {      
      peak_trigger(float d = 0.001f)
       : ef(d)
       , cmp(0.002f)
      {}

      bool operator()(float s)
      {
         constexpr gain g{0.9f};
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
   // The integrator accumulates the input samples (s).
   ////////////////////////////////////////////////////////////////////////////
   struct integrator
   {
      // y: current value

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
   //
   // This class is templated on the native integer sample type 
   // (e.g. uint16_t).
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   struct downsample
   {
      downsample()
       : x(0)
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
   // Automatic gain control (agc) uses an envelope follower to dynamically 
   // adjust the gain following the signal level (s). Weaker signals are
   // amplified more than stronger signals to maintain a constant output 
   // level.
   //
   // The agc incorporates a noise gate that turns off the amplifier below a 
   // low threshold (l). Above this threshold, the gain is h / env, where h is 
   // the high threshold and env is the current envelope. 
   //
   // For example, if the high threshold is 1.0 and the current envelope is 
   // 0.01, then the gain is 100 (1.0 / 0.01). If the current envelope is 1.0, 
   // then the gain is 1 (1.0 / 1.0). 
   //
   // The maximum gain is constrained to a given maximum (a).
   ////////////////////////////////////////////////////////////////////////////
   struct agc
   {
      // a: maximum gain
      // l: low threshold
      // h: high threshold
      // d: decay

      agc(float a, float l, float h = 1.0f, float d = 0.001f)
       : ef(d)
       , a(a)
       , l(l)
       , h(h)
      {}

      float operator()(float s)
      {
         // get current value of the peak detector
         auto env = ef(s);        
         
         // env < l:    noise gate closes; gain is 0 
         // otherwise:  gain = h / env, limited to maximum gain (a)
         return (env < l) ? 0.0f : s * std::min<float>(a, h / env);
      }

      envelope_follower ef;
      float a;
      float l;
      float h;
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
