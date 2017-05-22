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

      float operator()(float s)
      {
         constexpr agc agc = {1000, 0.5f};
         constexpr noise_gate ng = {threshold};
         
         // update the current envelope
         auto env = _ef(std::abs(s));

         // dc-block
         s = _dc_blk(s);
         
         // automatic gain control
         s = agc(s, env);
         
         // noise gate
         s = ng(s, env);
      
         // detect the peaks
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
         else if (neg || ng.gated(env))
            _state = 0.0f;         
         return _state;
      }

      envelope_follower _ef = {0.0001f};
      dc_block          _dc_blk = {20.0f, sps};

      peak_trigger      _pos_peak;
      peak_trigger      _neg_peak;
      float             _state = {0.0f};
   };
}}

#endif
