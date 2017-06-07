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
   //       1) DC blocker: Blocks any DC offset from the original signal.
   //       2) Integrator: Integrates the signal.
   //       3) DC blocker: Blocks any DC offset from the integrated signal.
   //       4) Envelope follower: Tracks the envelope of the signal. Take note
   //          that this is exracted *after* integration.
   //       5) Noise gate: Suppresses the output when the signal drops to a
   //          certain threshold.
   //       6) Automatic gain control: Attempts to maintain a constant 
   //          amplitude by dynamically applying gain from 1.0f to max_gain.
   //          The lower the signal level (detected by the envelope follower), 
   //          the higher the gain.
   ////////////////////////////////////////////////////////////////////////////
   template <std::uint32_t sps>
   struct agc
   {  
      static constexpr float max_gain = 100.0f;
      static constexpr float threshold = 1.0f / max_gain;
      static constexpr float release = std::exp(-1.0f / (sps * 0.2f /*seconds*/));

      float operator()(float s)
      {      
         // DC block
         s = _dc_block(s);
         
         // Integrate and second DC block
         s = _dc_block2(_integrate(s));
         
         // Envelope follower
         auto env = _env_follow(std::abs(s));

         // Noise gate      
         if (_noise_gate(threshold, env))
         {
            _gain *= release;
            return s * _gain;
         }

         // Automatic gain control
         _gain = fast_inverse(env);
         return s * _gain;      
      }
      
      bool gated() const
      {
         return _noise_gate();
      }

      schmitt_trigger _noise_gate = {/* hysteresis */ threshold * 0.5f};
      envelope_follower _env_follow = {/* decay */ 0.9995f};
      dc_block _dc_block = { 10.0f /* hz */, sps };
      dc_block _dc_block2 = { 10.0f /* hz */, sps };
      integrator _integrate = {0.1};
      float _gain = {1.0f};
   };
}}

#endif
