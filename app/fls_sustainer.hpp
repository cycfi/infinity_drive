/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_SUSTAINER_HPP_NOVEMBER_11_2017

#include <q/synth.hpp>
#include "freq_locked_synth.hpp"

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Infinity Sustainer
   ////////////////////////////////////////////////////////////////////////////
   template <std::uint32_t sps, std::uint32_t latency>
   class sustainer
   {
   public:

      float operator()(float s, uint32_t sample_clock)
      {
         return _fls(s, sample_clock++);
      }

      float envelope() const
      {
         return _fls.envelope();
      }

   private:

      using sin_synth = decltype(q::sin(0.0, sps, 0.0));
      using fls_type = freq_locked_synth<sin_synth, sps, latency>;

      sin_synth   _synth = q::sin(0.0, sps, 0.0);
      fls_type    _fls = { _synth, 0 };
   };
}}

#endif
