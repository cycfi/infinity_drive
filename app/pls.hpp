/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PLS_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_PLS_HPP_SEPTEMBER_27_2017

#include "pll.hpp"
#include "agc.hpp"
#include "period_trigger.hpp"
#include <cmath>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

   ////////////////////////////////////////////////////////////////////////////
   // phase locked synth (pls)
   //
   // The pls looks at the input audio and extracts the fundamental frequency
   // and phase from the waveform and uses these information to set the
   // frequency and phase of a synthesiser, provided by the client. The pls
   // generates phase accurate synthesized output required for polyphonic
   // sustain.
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Synth, uint32_t sps, uint32_t latency>
   class pls
   {
   public:

      enum { stop, run, release };

      pls(Synth& synth_, q::phase_t start_phase_)
       : _agc(0.05f /* seconds */, sps)
       , _pll(synth_)
       , _start_phase(start_phase_)
      {}

      float operator()(float s)
      {
         auto agc_out = _agc(s);
         bool is_active = _agc.active();
         int state = _trig(agc_out, is_active);

         if (!is_active)
            return deactivate();

         _stage = run;
         return _pll(state);
      }

      float envelope() const
      {
         return _agc.envelope();
      }

      q::phase_t start_phase() const
      {
         return _start_phase;
      }

      void start_phase(q::phase_t start_phase_)
      {
         _start_phase = start_phase_;
      }

      Synth& synth()
      {
         return _pll._synth;
      }

   private:

      struct agc_config
      {
         static constexpr float max_gain = 50.0f;
         static constexpr float low_threshold = 0.01f;
         static constexpr float high_threshold = 0.05f;
      };

      // Release the synth
      float deactivate()
      {
         switch (_stage)
         {
            case stop:
               return 0.0f;

            case run:
               _stage = release;
               // fall through...

            case release: // continue until the next start phase
               if (synth().is_start())
               {
                  _stage = stop;
                  return 0.0f;
               }
               return synth()();
         }
         return 0.0f;
      }

      agc<agc_config>   _agc;
      period_trigger    _trig;
      pll<Synth>        _pll;
      uint32_t          _start_phase;
      int               _stage = stop;
   };
}}

#endif
