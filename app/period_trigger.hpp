/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016)
#define CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016

#include <inf/fx.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Period trigger
   //
   // The peak trigger employs two peak_trigger, each detecting positive and 
   // negative peaks.
   //
   // A positive peak sets the state to 1 while the negative peak sets the 
   // state to 0. A complete cycle starts from a rising edge until the next 
   // rising edge of the state. This makes the detector immune from multiple 
   // triggers from multiple peaks, in either the positive and negative peak 
   // detectors, since multiple positive or negative triggers will not change 
   // the state. Multiple triggers spanning both the positive and negaative 
   // peaks may still give false triggers.
   //
   // Multiple triggers are caused by overtones overpowering the fundamental 
   // frequency. These harmonics typically occur as local positive or negative 
   // peaks close to the intensity of the highest peak.
   //
   // Note: for best results, use an automatic gain control (see agc.hpp) 
   // before the period_trigger. Use the AGCs gate as the 'active' argument. 
   // Example:
   //
   //    auto result = _trig(_agc(s), _agc.active());
   //
   // where s is the sample input, _trig is the period_trigger and _agc
   // is the automatic gain control.
   ////////////////////////////////////////////////////////////////////////////
   struct period_trigger
   {
      bool operator()(float s, bool active = true)
      {
         // Detect the peaks
         auto pos = _pos_peak(s);
         auto neg = _neg_peak(-s);

         // Set the state
         if (!active || neg)
            _state = 0;
         else if (pos)
            _state = 1;

         return _state;
      }

      bool state() const { return _state; }

      peak_trigger  _pos_peak = {0.995};
      peak_trigger  _neg_peak = {0.995};
      bool          _state = 0;
   };
}}

#endif
