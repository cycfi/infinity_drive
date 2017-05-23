/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017)
#define CYCFI_INFINITY_PROCESSOR_HPP_MAY_20_2017

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // The processor:
   //
   //    1) Converts data from the ADC (of given resolution) to float
   //
   //    2) Performs some processing 
   //       - The base Processor class must have a process function with
   //         the signature:
   //
   //             void process(
   //                std::array<float, 2>& out
   //              , float s
   //              , std::uint32_t channel);
   //
   //         - out: Will hold the output values (2 channels) that will
   //           be sent to the 2-channel DAC
   //         - s: The current sample to be processed
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
   struct processor : Base
   {
      static constexpr auto oversampling = Base::oversampling;   
      static_assert(is_pow2(oversampling),
         "oversampling must be a power of 2, except 0"
      );
      
      template <typename I1, typename I2, typename Convert>
      inline void process(
         I1 first, I1 last, I2 src, std::uint32_t channel, Convert convert
      )
      {
         // This if-else will be optimized by the compiler since oversampling
         // is a constant that is known at compile time.
         if (oversampling == 1)
         {
            for (auto i = first; i != last; ++i)
               Base::process(*i, convert((*src++)[channel]), channel);
         }
         else
         {
            for (auto i = first; i != last; ++i)
            {
               std::uint32_t val = 0;
               for (auto j = 0; j != oversampling; ++j)
                  val += (*src++)[channel];
               Base::process(*i, convert(val), channel);
            }
         }
      }
   };
}}

#endif
