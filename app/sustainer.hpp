/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017

#include <q/synth.hpp>
#include <inf/pid.hpp>
#include "pls.hpp"

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

      float operator()(float s, uint32_t sample_clock)
      {
         return _pls(s, sample_clock++) * _level;
      }

      float envelope() const
      {
         return _pls.envelope();
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

      using sin_synth = decltype(q::sin(0.0, sps, 0.0));
      using pls_type = pls<sin_synth, sps, latency>;
      using pid_type = pid<level_pid_config>;

      sin_synth   _synth = q::sin(0.0, sps, q::pi/4);
      pls_type    _pls = { _synth };
      pid_type    _level_pid;
      float       _level = 0;
   };
}}

#endif
/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017

#include <q/synth.hpp>
#include <inf/pid.hpp>
#include "pls.hpp"

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

      float operator()(float s, uint32_t sample_clock)
      {
         return _pls(s, sample_clock++) * _level;
      }

      float envelope() const
      {
         return _pls.envelope();
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

      using sin_synth = decltype(q::sin(0.0, sps, 0.0));
      using pls_type = pls<sin_synth, sps, latency>;
      using pid_type = pid<level_pid_config>;

      sin_synth   _synth = q::sin(0.0, sps, q::pi/4);
      pls_type    _pls = { _synth };
      pid_type    _level_pid;
      float       _level = 0;
   };
}}

#endif
