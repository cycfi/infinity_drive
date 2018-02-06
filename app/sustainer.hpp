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

      static constexpr float max_gain = 4;
      static constexpr float set_point_max = 0.4;
      static constexpr float low_threshold = 0.01f;
      static constexpr float high_threshold = 0.05f;

      float operator()(float s)
      {
         // DC block
         s = _dc_block(s);

         // Update the envelope follower
         auto env = _env_follow(std::abs(s));

         // Noise gate
         if (!_noise_gate(env))
				return _lpf(0);

         return _lpf(s * _level);
      }

      float envelope() const
      {
         return _env_follow();
      }

      // Update the level. This should be called approximately
      // every 10ms (or adjust level_pid_config sps accordingly).
      void update_level(float level)
      {
         auto param = std::pow(2, level) - 1.0f;
         _level += _level_pid(param * set_point_max, envelope());

         // Clamp the level to 0 to max
         _level = std::max(std::min(_level, max_gain), 0.0f);
      }

   private:

      using pid_type = pid<level_pid_config>;

      q::dc_block          _dc_block;
      q::envelope_follower _env_follow;
      q::window_comparator _noise_gate = { low_threshold, high_threshold };
      q::one_pole_lp       _lpf = { 2000.0f, sps };

      pid_type             _level_pid;
      float                _level = 0;
   };
}}

#endif
