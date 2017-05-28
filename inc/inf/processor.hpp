/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017)
#define CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017

#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
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
   //       - The base class Base must have a static oversampling constant
   //         that defines the oversampling factor.
   //
   //    3) Stores the processed data to the output buffer
   //
   // If oversampling > 1, we perform down-sampling. Samples from the ADC 
   // are accumulated. The sum is divided by the oversampling factor before 
   // calling base Processor process function. Thus, the base Processor 
   // process function is called, and the result copied to the output buffer 
   // only once per oversampling.
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class processor : public Base
   {
   public:

      static constexpr auto oversampling = Base::oversampling;   
      static constexpr auto channels = Base::channels;   

      static_assert(is_pow2(oversampling),
         "oversampling must be a power of 2, except 0"
      );
      
      template <typename I1, typename I2, typename Convert>
      void process(I1 first, I1 last, I2 src, Convert convert)
      {
         process_impl(first, last, src, convert, bool_<oversampling == 1>{});
      }
      
   private:
   
      // case oversampling == 1
      template <typename I1, typename I2, typename Convert>
      void process_impl(
         I1 first, I1 last, I2 src, Convert convert, std::true_type)
      {
         for (auto i = first; i != last; ++i)
         {
            for (auto c = 0; c != channels; ++c)
               Base::process(*i, convert((*src)[c]), c);
            ++src;
         }
      }
      
      // case oversampling != 1
      template <typename I1, typename I2, typename Convert>
      void process_impl(
         I1 first, I1 last, I2 src, Convert convert, std::false_type)
      {
         for (auto i = first; i != last; ++i)
         {
            for (auto c = 0; c != channels; ++c)
            {
               auto src_base = src;
               std::uint32_t val = 0;
               for (auto j = 0; j != oversampling; ++j)
                  val += (*src_base++)[c];
               Base::process(*i, convert(val), c);
            }
            src += oversampling;
         }
      }
   };
}}

#endif
