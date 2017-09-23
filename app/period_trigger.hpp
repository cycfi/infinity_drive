/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016)
#define CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016

#include <inf/fx.hpp>
#include "agc.hpp"

namespace cycfi { namespace infinity
{
   template <uint32_t sps>
   struct period_trigger
   {
      float operator()(float s)
      {
         // Automatic gain control
         s = _agc(s);

         // Detect the peaks
         auto pos = _pos_peak(s);
         auto neg = _neg_peak(-s);
         
         // A positive peak sets the state to 1 while the negative peak 
         // sets the state to 0. A complete cycle starts from a rising 
         // edge until the next rising edge of the state. This makes the 
         // detector immune from multiple triggers, from multiple peaks, in 
         // both the positive and negative peak detectors, since multiple 
         // positive or negative triggers will not change the state.
         //
         // Multiple triggers are caused by overtones overpowering the
         // fundamental frequency. These harmonics typically occur as local 
         // positive or negative peaks close to the intensity of the highest
         // peak.

         if (pos)
            _state = 1.0f;
         else if (neg || _agc.gated())
            _state = 0.0f;         
         return _state;
      }

      agc<sps>		  _agc;
      peak_trigger  _pos_peak;
      peak_trigger  _neg_peak;
      float         _state = {0.0f};
   };
}}

#endif
