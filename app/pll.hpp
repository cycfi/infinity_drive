/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017

#include <q/fx.hpp>
#include <q/synth.hpp>
#include <cmath>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // PLL classes. Based on Implementing a digital phase-locked loop in
   // software. See https://en.wikipedia.org/wiki/Phase-locked_loop
   ////////////////////////////////////////////////////////////////////////////

   struct phase_detector
   {
      int operator()(bool sig, bool ref)
      {
         // Reset the "flip-flop" of the phase-frequency detector when both
         // signal and reference are high
         bool rst = !(_qsig && _qref);

         // Trigger signal flip-flop and leading edge of signal
         _qsig = (_qsig || (sig && !_lsig)) && rst;

         // Trigger reference flip-flop on leading edge of reference
         _qref = (_qref || (ref && !_lref)) && rst;

         // Store these values for next iteration (for edge detection)
         _lref = ref;
         _lsig = sig;

         // Compute the error signal (whether frequency should increase
         // or decrease). Error signal is given by one or the other flip
         // flop signal.
         return _qref - _qsig;
      }

      bool  _qsig = 0;  // current signal
      bool  _qref = 0;  // current reference
      bool  _lsig = 0;  // previous signal
      bool  _lref = 0;  // previous reference
   };

   ////////////////////////////////////////////////////////////////////////////
   struct pll_loop_filter
   {
      // Loop filter constants (proportional and derivative)
      // Currently powers of two to facilitate multiplication by shifts
      static constexpr float kp = 128; // n / 128
      static constexpr int kd = 64;    // n * 64

      q::phase_t operator()(int ersig)
      {
         // Implement a pole-zero filter by proportional and derivative
         // input to frequency
         auto filtered_ersig = ersig + (ersig - _lersig) * kd;

         // Keep error signal for proportional output
         _lersig = ersig;

         // Integrate VCO frequency using the error signal
         _freq -= (q::one_cyc / kp) * filtered_ersig;
         return _freq;
      }

      q::phase_t operator()() const
      {
         return _freq;
      }

      int         _lersig = 0;
      q::phase_t  _freq = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Synth>
   struct pll
   {
      static constexpr q::phase_t pi = q::phase::angle(q::pi);

      pll(Synth& synth)
       : _synth(synth) {}

      float operator()(bool s)
      {
         auto error = _pd(s, synth_phase() > pi);
         auto val = _synth();
         return error;
      }

      q::phase_t synth_phase() const
      {
         return _synth.phase() + _synth.shift();
      }

      Synth&            _synth;
      phase_detector    _pd;
      pll_loop_filter   _lf;
   };
}}

#endif
