/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_DRIVER_HPP_MARCH_24_2018)
#define CYCFI_INFINITY_DRIVER_HPP_MARCH_24_2018

#include <q/fx.hpp>
#include <q/literals.hpp>

namespace cycfi { namespace infinity
{
   using namespace q::literals;

   template <std::uint32_t sps>
   struct driver
   {
      static constexpr float threshold = 0.02f;

      float operator()(float s)
      {
         // DC block
         s = _dc_block(s);

         // Envelope follower
         auto env = _env(s);

         // Noise Gate
         if (env < threshold)
            return 0;

         // Normalize
         s = s * q::fast_inverse(env);

         // Clip
         s = _clip(s);

         return s * 0.8;
      }

      q::dc_block                _dc_block   { 0.1_Hz, sps };
      q::peak_envelope_follower  _env        { 100_ms, sps };
      q::window_comparator       _clip       { -0.5, 0.0 };
   };
}}

#endif
