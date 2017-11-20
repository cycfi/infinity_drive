/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_PLL_HPP_SEPTEMBER_27_2017

#include <q/synth.hpp>
#include <cmath>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // PLL classes. Based on Implementing a digital phase-locked loop in
   // software. See https://en.wikipedia.org/wiki/Phase-locked_loop
   ////////////////////////////////////////////////////////////////////////////
   struct flip_flop
   {
      bool operator()(bool set, bool reset)
      {
         _out = !reset && (_out || (set && !_prev));
         _prev = set;
         return _out;
      }

      bool operator()() const
      {
         return _out;
      }

      bool _out = 0;
      bool _prev = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   struct phase_detector
   {
      int operator()(bool sig, bool ref)
      {
         // Reset the flip-flops when both signal (sig) and reference (ref)
         // are high.
         bool reset = _qsig() && _qref();

         // Trigger signal (_qsig) and reference (_qref) flip-flops on
         // leading edge of signal and reference (respectively).
         _qsig(sig, reset);
         _qref(ref, reset);

         // Compute the error signal. Returns 1 if the reference is leading,
         // -1 if the signal is leading, or 0 if both are in sync.
         return _qref() - _qsig();
      }

      flip_flop   _qsig;   // signal flip-flop
      flip_flop   _qref;   // reference flip-flop
   };

   ////////////////////////////////////////////////////////////////////////////
   struct pll_loop_filter
   {
      // Loop filter constants (proportional and derivative)
      // Currently powers of two to facilitate multiplication and
      // by division using shifts.
      static constexpr auto kp = q::pow2<int32_t>(20);
      static constexpr auto kd = q::pow2<int32_t>(10);

      q::phase_t operator()(int error, q::phase_t freq)
      {
         // Implement a pole-zero filter by proportional and derivative
         // input to frequency. Derivative uses kd gain (above) which is
         // a statically known power of two (constexpr). The multiplication
         // will be optimized by the compiler using shifts.
         auto delta = error - _prev_error;
         auto out = error + (delta * kd);
         _prev_error = error;

         // Integrate VCO frequency using the error signal, scaled to
         // the phase_t range (see synth.hpp). The computation uses kp gain.
         // The division will be optimized out by the compiler since
         // both one_cyc and kp are statically known constexprs.
         freq -= (q::one_cyc / kp) * out;
         return freq;
      }

      int   _prev_error = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Synth>
   struct pll
   {
      static constexpr q::phase_t phase = q::one_cyc / 2;

      pll(Synth& synth)
       : _synth(synth)
      {}

      float operator()(bool sig)
      {
         auto ref = _synth.phase() < phase;
         auto error = _pd(sig, ref);
         _synth.freq(_lf(error, _synth.freq()));
         return _synth();
      }

      Synth&            _synth;
      phase_detector    _pd;
      pll_loop_filter   _lf;
   };
}}

#endif
