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
   //       2) Envelope follower: Tracks the envelope of the signal.
   //       3) Noise gate: Suppresses the output when the signal drops below
   //          a certain threshold.
   //       4) Automatic gain control: Attempts to maintain a constant
   //          amplitude by dynamically applying gain. The lower the signal 
   //          level (detected by the envelope follower), the higher the gain.
   ////////////////////////////////////////////////////////////////////////////
   template <std::uint32_t sps>
   struct agc
   {
      static constexpr float l_threshold = 0.02f;
      static constexpr float h_threshold = 0.1f;

      float operator()(float s)
      {
         // DC block
         s = _dc_block(s);

         // Update the envelope follower
         auto env = _env_follow(std::abs(s));

         // Noise gate
         if (!_noise_gate(env))
				return 0;

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

      window_comparator _noise_gate = { l_threshold, h_threshold };
      envelope_follower _env_follow = {/* decay */ 0.999f};
      dc_block _dc_block = { 10.0f /* hz */, sps };
      float _gain = { 1.0f };
   };
}}

#endif
