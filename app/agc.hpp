/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(INFINITY_AGC_HPP_MAY_22_2017)
#define INFINITY_AGC_HPP_MAY_22_2017

#include <q/fx.hpp>
#include <q/support.hpp>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

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
   //
   //    Config is a class that sets the AGC parameters. Config must declare
   //    some configuration constants:
   //
   //       1) low_threshold: Sets the low threshold when the noise gate will
   //          kick in and inhibit the output.
   //       2) high_threshold: Sets the high threshold when the noise gate will
   //          open.
   //
   //    Example:
   //
   //       struct agc_config
   //       {
   //          static constexpr float low_threshold = 0.01f;
   //          static constexpr float high_threshold = 0.05f;
   //       };
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Config>
   struct agc
   {
      static constexpr float low_threshold = Config::low_threshold;
      static constexpr float high_threshold = Config::high_threshold;

      agc(float decay, uint32_t sps)
       : _env_follow(decay, sps)
       , _dc_block(10.0f /* hz */, sps)
      {}

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
         _gain = q::fast_inverse(env);
         return s * _gain;
      }

      bool active() const
      {
         return _noise_gate();
      }

      float envelope() const
      {
         return _env_follow();
      }

      q::window_comparator _noise_gate = { low_threshold, high_threshold };
      q::envelope_follower _env_follow;
      q::dc_block          _dc_block;
      float                _gain = { 1.0f };
   };
}}

#endif
