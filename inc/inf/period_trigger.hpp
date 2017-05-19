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

      period_trigger(float cutoff = 1500.0f)
       : _lp(cutoff, sps)
       , _agc(1000, threshold, 1.0f, 0.0001f)
       , _dc_blk(20.0f, sps)
       , _state(0)
      {}

      bool operator()(float s)
      {
         // low-pass filter
         auto val = _lp(s);

         // dc-block
         val = _dc_blk(val);

         // agc
         val = _agc(val);

         // peak triggers
         auto valp = _peak_pos(val);   // positive peaks
         auto valn = _peak_neg(-val);  // negative peaks
                                    
         if (valp)
            _state = 1;
         else if (valn || _agc.gated())
            _state = 0;

         return _state;
      }

      one_pole_lp    _lp;
      peak_trigger   _peak_pos;
      peak_trigger   _peak_neg;
      agc            _agc;
      dc_block       _dc_blk;
      bool           _state;
   };
}}

#endif
