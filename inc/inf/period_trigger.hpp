/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_DETECTOR_HPP_FEBRUARY_11_2016)
#define CYCFI_INFINITY_PERIOD_DETECTOR_HPP_FEBRUARY_11_2016

#include <inf/fx.hpp>

namespace cycfi { namespace infinity
{
   template <uint32_t sps>
   struct period_trigger
   {
      static constexpr float threshold = 0.03;

      period_trigger(float cutoff = 2000.0f)
       : _agc(1000, threshold, 0.5f, 0.0001f)
       , _dc_blk(20.0f, sps)
       , _state(0.0f)
      {}

      float operator()(float s)
      {                  
         // dc-block
         s = _dc_blk(s);
         
         // agc
         s = _agc(s);
      
         // detect the peaks
         auto pos = _pos_peak(s);
         auto neg = _neg_peak(-s);
         
         // A positive peak sets the state to 1 while the negative peak 
         // sets the state to 0. A complete cycle starts from the rising 
         // edge until the next rising of the state. This makes the detector
         // immune from multiple triggers, from multiple peaks, in both
         // the positive and negative peak detectors, since multiple positive
         // or negative triggers will not change the state.
         // 
         if (pos)
            _state = 1.0f;
         else if (neg || _agc.gated())
            _state = 0.0f;
         
         return _state;
      }

      peak_trigger   _pos_peak;
      peak_trigger   _neg_peak;
      agc            _agc;
      dc_block       _dc_blk;
      float          _state;
   };
}}

#endif
