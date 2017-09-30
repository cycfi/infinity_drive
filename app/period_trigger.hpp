/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016)
#define CYCFI_INFINITY_PERIOD_TRIGGER_HPP_FEBRUARY_11_2016

#include <inf/fx.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // The peak_trigger generates pulses that coincide with the peaks of a
   // waveform. This is accomplished by sending the signal through an envelope
   // follower and comparing the (slightly attenuated) result with the
   // original signal using a schmitt_trigger.
   //
   // The result is a bool corresponding to the peaks.
   ////////////////////////////////////////////////////////////////////////////
   struct peak_trigger
   {
      static constexpr float hysteresis = 0.002f;
      static constexpr float drop = 0.80f;

      peak_trigger(float r)
       : _ef(r), _cmp(hysteresis)
      {}

      bool operator()(float s)
      {
         return _cmp(s, _ef(s) * drop);
      }

      envelope_follower _ef;
      schmitt_trigger   _cmp;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Period trigger
   //
   // The period_trigger employs two peak_trigger, each detecting positive
   // and negative peaks.
   //
   // A positive peak sets the state to 1 while the negative peak sets the
   // state to 0. A complete cycle starts from a rising edge until the next
   // rising edge of the state. This makes the detector immune from multiple
   // triggers from multiple peaks, in either the positive and negative peak
   // detectors, since multiple positive or negative triggers will not change
   // the state. Multiple triggers spanning both the positive and negative
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
      static constexpr float dead_zone = 0.5;

      int operator()(float s, bool active = true)
      {
         if (!active)
         {
            _state = 0;
            return _state;
         }

         // kill dead-zone
         if (s < dead_zone && s > -dead_zone)
            s = 0.0f;

         // Detect the peaks
         auto pos = _pos_peak(s);
         auto neg = _neg_peak(-s);

         // Set the state
         if (neg)
            _state = 0;
         else if (pos)
            _state = 1;
         return _state;
      }

      bool state() const { return _state; }

      peak_trigger   _pos_peak = {0.999};
      peak_trigger   _neg_peak = {0.999};
      int            _state = 0;
   };
}}

#endif
