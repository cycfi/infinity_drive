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
   //    2) Performs some processing 
   //       - The base Processor class must have a process function with
   //         the signature float process(float);
   //    3) Copies the processed data to the output buffer
   //
   // If n_samples > 1, we perform down sampling. n_samples from the ADC 
   // are accumulated. For every n_samples, the sum is divided by n_samples 
   // before calling base Processor process function. Thus, the base 
   // Processor process function is called, and the result copied to the 
   // output buffer only once per n_samples.
   ////////////////////////////////////////////////////////////////////////////
   template <
      typename Base
    , std::uint32_t resolution_
    , std::uint32_t n_samples_ = 1
   >
   struct processor : Base
   {
      static constexpr std::uint32_t resolution = resolution_;   
      static constexpr std::uint32_t n_samples = n_samples_;   
      static_assert(is_pow2(n_samples),
         "n_samples must be a power of 2, except 0"
      );

      float convert(std::uint32_t sample)
      {
         return (sample / float(resolution_)) - 1.0f;
      }
      
      template <typename I1, typename I2>
      inline void process(I1 first, I1 last, I2 src, std::uint32_t channel)
      {
         // This if-else will be optimized by the compiler since n_samples
         // is a constant that is known at compile time.
         if (n_samples == 1)
         {
            for (auto i = first; i != last; ++i)
               *i = Base::process(convert((*src++)[channel]));
         }
         else
         {
            for (auto i = first; i != last; ++i)
            {
               std::uint32_t val = 0;
               for (auto j = 0; j != n_samples; ++j)
                  val += (*src++)[channel];
               *i = Base::process(convert(val / n_samples));
            }
         }
      }
   };
}}

#endif
