/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017)
#define CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017

#include <q/support.hpp>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

   ////////////////////////////////////////////////////////////////////////////
   // The processor:
   //
   //    1) Converts data from the ADC (of given resolution) to float
   //
   //    2) Performs some processing
   //       - The base Processor class must have a process member function
   //         with the signature:
   //
   //             void process(
   //                std::array<float, 2>& out
   //              , float s
   //              , std::uint32_t channel);
   //
   //         - out: Will hold the output values (2 channels) that will
   //           be sent to the 2-channel DAC
   //         - s: The current sample to be processed (normalized
   //           to -1.0f...1.0f)
   //         - channel: The current channel being processed
   //
   //    3) Stores the processed data to the output buffer
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class processor : public Base
   {
   public:

      static constexpr auto oversampling = 1;
      static constexpr auto channels = Base::channels;

      template <typename I1, typename I2, typename Convert>
      void process(I1 first, I1 last, I2 src, Convert convert)
      {
         for (auto i = first; i != last; ++i)
         {
            i->fill(0.0f);
            for (auto c = 0; c != channels; ++c)
               Base::process(*i, convert((*src)[c]), c);
            ++src;
         }
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class oversampling_processor : public Base
   {
   public:

      static constexpr auto oversampling = Base::oversampling;
      static constexpr auto channels = Base::channels;

      static_assert(q::is_pow2(oversampling),
         "oversampling must be a power of 2, except 0"
      );

      template <typename I1, typename I2, typename Convert>
      void process(I1 first, I1 last, I2 src, Convert convert)
      {
         for (auto i = first; i != last; ++i)
         {
            i->fill(0.0f);
            for (auto c = 0; c != channels; ++c)
            {
               auto src_base = src;
               std::uint32_t val = 0;
               for (auto j = 0; j != oversampling; ++j)
                  val += Base::downsample((*src_base++)[c], j);
               Base::process(*i, convert(val), c);
            }
            src += oversampling;
         }
      }
   };
}}

#endif
