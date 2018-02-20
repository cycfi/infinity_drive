/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(INFINITY_DELTA_FILTER_HPP_FEBRUARY_16_2018)
#define INFINITY_DELTA_FILTER_HPP_FEBRUARY_16_2018

#include <q/fx.hpp>
#include <q/support.hpp>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

   struct delta_filter
   {
      delta_filter(float dfreq, std::uint32_t sps)
       : _dlp(dfreq, sps)
      {}

      float operator()(float s)
      {
         auto dy = s - _y;
         return _y += _dlp(dy);
      }

      float _y = 0.0f;
      q::one_pole_lowpass _dlp;
   };
}}

#endif
