/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_DETECTOR_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_PERIOD_DETECTOR_HPP_NOVEMBER_11_2017

#include <q/fx.hpp>
#include <cmath>

namespace cycfi { namespace infinity
{
   struct period_detector
   {
      period_detector(float a)
       : _lp(a)
      {}

      period_detector(float freq, uint32_t sps)
       : _lp(freq, sps)
      {}

      bool within(int p)
      {
         // Test that p falls within (approximately)
         // one semitone from the mean (higher or lower).
         return (p > _min) && (p < _max);
      }

      float update(int p)
      {
         _prev = 0;
         auto val = _lp(p);                  // Update the filter
         compute_min_max(std::ceil(val));    // Compute min and max
         return val;                         // Return the current mean
      }

      void compute_min_max(int mean)
      {
         // Compute the _min and _max (approximately 1 semitone
         // below and above val).
         auto mean_div_16 = mean / 16;
         _max = mean + mean_div_16;
         _min = mean - mean_div_16;
      }

      float operator()() const
      {
         return _lp();
      }

      float operator()(int p)
      {
         if (within(p))
            return update(p);

         if (_prev)
         {
            auto sum = _prev + p;
            if (within(sum))
               return update(sum);
            return update(sum / 2);
         }
         else
         {
            _prev = p;
         }
         return _lp();
      }

      period_detector& operator=(int p)
      {
         _prev = 0;
         _lp = p;             // Set the filter
         compute_min_max(p);  // Compute min and max
         return *this;
      }

      int _min, _max;
      int _prev = 0;
      q::one_pole_lp _lp;
   };
}}

#endif
