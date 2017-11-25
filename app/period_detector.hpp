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
   ////////////////////////////////////////////////////////////////////////////
   // Period detector
   //
   //    The period_trigger (see period_trigger.hpp) works nicely, but it is
   //    not perfect. Sometimes it generates extranous false triggers with
   //    higher harmonics, esp. since typical of guitars, the second harmonic
   //    is actually more prominent than the fundamental.
   //
   //    See Fast and Efficient Pitch Detection:
   //    http://www.cycfi.com/2017/10/fast-and-efficient-pitch-detection/
   //
   //    Even, with the dual peak triggers, sometimes, the plucked string can
   //    generate significant amount of harmonics that cause multiple triggers
   //    spanning both the positive and negative peaks. Based on numerous
   //    analysis and tests, the period triggering scheme can generate at most
   //    one false trigger per cycle. It is probable but highly unlikely for
   //    the period trigger to generate more than one false trigger per cycle,
   //    but such an unlikely event can be ignored. We will only deal with
   //    single false triggers per cycle.
   //
   //    A naive way to mitigate this issue is by using a low-pass or band-pass
   //    filter to tame the energetic high frequencies. But any form of filter
   //    will mess up the phase, so that's out of the question. We need precise
   //    alignment of phase in order to induce infinite sustain using positive
   //    feedback. It is not enough to synthesize a waveform having the same
   //    frequency as the vibrating string. The phase must be correct as well.
   //    Filtering complicates the problem.
   //
   //    So what do we do? The period detector maintains a low pass filter
   //    for the periods, essentially maintaining the average period at any
   //    given time. For each new period P, a) P is compared against the
   //    current average. If P falls within approximately one semitone off the
   //    average, then we are good. The low pass filter is updated and a new
   //    average is returned.
   //
   //    If the new period P does not fall within one semitone off the average,
   //    then b) we save the current period for later (call that prev). We
   //    return the current average.
   //
   //    When a new period comes along, we repeat a). If the new period P does
   //    not fall within one semitone off the average, then, since we saved
   //    the previous period (prev), we check if the sum of the previous (prev)
   //    and the new period P falls within one semitone off the average. If it
   //    does, we're in luck. We successfully detected a false trigger from
   //    overpowering harmonics. We update the averaging low pass filter with
   //    the sum of the currrent period P plus the previous period (prev) and
   //    a new average is returned.
   //
   //    If the sum of the new period P and the previous period (prev)
   //    does not fall within one semitone off the average, then the sum/2
   //    is added to the running average and a new average is returned.
   //
   ////////////////////////////////////////////////////////////////////////////
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
         int mean_div_16 = mean / 16;
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
