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
   //       2) Automatic filter control: Applies a one pole low pass filter 
   //          with a dynamically adjusted cutoff frequency from half the 
   //          sampling frequency down to 300Hz. The lower the signal level
   //          (detected by the envelope follower), the lower the cutoff
   //          frequency. This filters out high frequency noise present  
   //          in low-level signals that will be amplified by a large factor
   //          by the automatic gain control.
   //       3) Envelope follower: Tracks the envelope of the signal. Take note
   //          that this is exracted *after* the automatic filter control.
   //       4) Noise gate: Suppresses the output when the signal drops to a
   //          certain threshold.
   //       5) Automatic gain control: Attempts to maintain a constant 
   //          amplitude by dynamically applying gain from 1.0f to max_gain.
   //          The lower the signal level (detected by the envelope follower), 
   //          the higher the gain.
   ////////////////////////////////////////////////////////////////////////////
   template <std::uint32_t sps>
   struct agc
   {  
      static constexpr float max_gain = 500.0f;
      static constexpr float threshold = 1.0f / max_gain;
      static constexpr float low_freq = 1.0f - exp(-_2pi * 300.0f/sps);
      static constexpr float high_freq = 1.0f - exp(-_2pi * 0.5);

      float operator()(float s)
      {      
         // DC block
         s = _dc_block(s);

         // Automatic filter control
         _low_pass.a = linear_interpolate(low_freq, high_freq, _env_follow());
         s = _low_pass(s);

         // Envelope follower
         auto env = _env_follow(std::abs(s));

         // Noise gate      
         if (_noise_gate(threshold, env))
            return 0.0f;

         // Automatic gain control
         return s * inverse(env);      
      }

      schmitt_trigger _noise_gate = {threshold * 0.5f};
      envelope_follower _env_follow = {0.001f};
      one_pole_lp _low_pass = {0.0f};
      dc_block _dc_block;
   };
}}

#endif
