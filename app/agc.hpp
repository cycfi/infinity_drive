/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(INFINITY_AGC_HPP_MAY_22_2017)
#define INFINITY_AGC_HPP_MAY_22_2017

#include <inf/fx.hpp>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Automatic gain control
   //
   //    The signal chain is s follows:
   //
   //       1) DC blocker: Blocks any DC offset from the signal.
   //       2) Adaptive filter: Applies a first-order low pass filter with a
   //          dynamically adjusted cutoff frequency from half the sampling
   //          frequency down to 300Hz. The lower the signal level (detected
   //          by the envelope follower), the lower the cutoff frequency.
   //          This filters out high frequency noise present in low-level
   //          signals that will be amplified by a large factor by the
   //          automatic gain control. The filter is activated only when the
   //          signal drops below a preset level (the lp_threshold).
   //       3) Envelope follower: Tracks the envelope of the signal. Take note
   //          that this is exracted *after* the automatic filter control.
   //       4) Noise gate: Suppresses the output when the signal drops below
   //          a certain threshold. Below this threshold, the signal is slowly
   //          attenuated until it is totally suppressed.
   //       5) Automatic gain control: Attempts to maintain a constant
   //          amplitude by dynamically applying gain from 1.0f to max_gain.
   //          The lower the signal level (detected by the envelope follower),
   //          the higher the gain.
   ////////////////////////////////////////////////////////////////////////////
   template <std::uint32_t sps>
   struct agc
   {
      static constexpr float max_gain = 100.0f;
      static constexpr float threshold = 1.0f / 20.0f; //1.0f / max_gain;
      static constexpr float lp_threshold = 1.0f / 10.0f;
      static constexpr float release = std::exp(-1.0f / (sps * 0.01f /*seconds*/));

      static constexpr float low_freq = 1.0f - exp(-_2pi * 300.0f/sps);
      static constexpr float high_freq = 1.0f - exp(-_2pi * 0.5);
      static constexpr float lp_freq = 1.0f - exp(-_2pi * 1000.0f/sps);

      float operator()(float s)
      {
         // DC block
         s = _dc_block(s);



         // Get the current envelope
//         auto env = envelope();

//         if (env < lp_threshold)
//         {
//            // Automatic filter control
//            static constexpr float factor = 1.0f / lp_threshold;
//            _low_pass.a = linear_interpolate(low_freq, high_freq, env * factor);
//            s = _low_pass(s);
//         }


         // Automatic filter control
//         _low_pass.a = linear_interpolate(low_freq, high_freq, env);
         s = _low_pass(s);



         // Update the envelope follower
         auto env = _env_follow(std::abs(s));

//         s = _low_pass2(s);


         // Noise gate
         if (!_noise_gate(env))
         {
             //op = 0;

//            return 0.0f;
            _gain *= release;
            return s * _gain;
         }
         else
         {
            //op = 1;
         }

         // Automatic gain control
         _gain = fast_inverse(env);
         return s * _gain;
      }

      bool gated() const
      {
         return _noise_gate();
      }

      float envelope() const
      {
         return _env_follow();
      }

      window_comparator _noise_gate = { threshold, lp_threshold };
//      schmitt_trigger _noise_gate = {/* hysteresis */ threshold * 0.5f};
//      envelope_follower _env_follow = {/* decay */ 0.995f};
      envelope_follower _env_follow = {/* decay */ 0.999f};
      dc_block _dc_block = { 10.0f /* hz */, sps };
      one_pole_lp _low_pass = {lp_freq};
      one_pole_lp _low_pass2 = {lp_freq};
      one_pole_lp _gain_smooth = {0.99995f};
      float _gain = {1.0f};
   };
}}

#endif
