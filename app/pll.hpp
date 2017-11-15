/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017

#include <q/fx.hpp>
#include <q/synth.hpp>
#include "agc.hpp"
#include "period_trigger.hpp"
#include <cmath>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

   template <std::uint32_t sps, std::uint32_t latency>
   class phase_detector
   {
   public:

      phase_detector(q::signed_phase_t start_phase_)
       : _start_phase(start_phase_)
      {}

      // n:  number of samples, edge to edge
      // p:  current phase of the reference synthesizer
      // f:  reference synthesizer frequency
      phase_t operator()(uint32_t n, phase_t p, phase_t f)
      {
         float period = q::osc_period(n, sps);

         std::size_t samples_delay = period - std::fmod(latency, period);
         auto target_phase = _start_phase - (samples_delay * f);

         q::signed_phase_t phase_diff = target_phase - p;
         return (((long long) f) * phase_diff) >> 32;
      }

   private:

      uint32_t _start_phase;
   };
}}

#endif
