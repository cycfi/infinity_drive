/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017

#include <q/fx.hpp>
#include <inf/pid.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Infinity Sustainer
   ////////////////////////////////////////////////////////////////////////////
   struct level_pid_config
   {
      float static constexpr p = 0.05f;         // Proportional gain
      float static constexpr i = 0.0f;          // Integral gain
      float static constexpr d = 0.03f;         // Derivative gain
      float static constexpr sps = 100.0f;      // Update Frequency (Hz)
   };

   template <std::uint32_t sps, std::uint32_t latency>
   class sustainer
   {
   public:

      static constexpr float max_gain = 2;
      static constexpr float low_threshold = 0.01f;
      static constexpr float high_threshold = 0.05f;

      float operator()(float s, uint32_t sample_clock)
      {
         // DC block
         s = _dc_block(s);

         // Update the envelope follower
         auto env = _env_follow(std::abs(s));

         // Noise gate
         if (!_noise_gate(env))
				return 0;

         // // Automatic gain control
         // _gain = q::fast_inverse(env) * _level;
         // if (_gain > max_gain)
         //    _gain = max_gain;
         // return s * _gain;

         return s * _level;
      }

      float envelope() const
      {
         return _env_follow();
      }

      // Update the level. This should be called approximately
      // every 10ms (or adjust level_pid_config sps accordingly).
      void update_level(float level, float max)
      {
         _level += _level_pid(level, envelope());

         // Clamp the level to 0 to max
         _level = std::max(std::min(_level, max), 0.0f);
      }

   private:

      using pid_type = pid<level_pid_config>;

      q::dc_block          _dc_block;
      q::envelope_follower _env_follow;
      q::window_comparator _noise_gate = { low_threshold, high_threshold };
      float                _gain = { 1.0f };
      pid_type             _level_pid;
      float                _level = 0;
   };
}}

#endif
