/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_SEPTEMBER_27_2017

#include <q/fx.hpp>
#include <q/synth.hpp>
#include "agc.hpp"
#include "period_trigger.hpp"
#include <cmath>

namespace cycfi { namespace infinity
{
   namespace q = cycfi::q;

   ////////////////////////////////////////////////////////////////////////////
   // freq_locked_synth
   //
   // The freq_locked_synth looks at the input audio and extracts the
   // fundamental frequency and phase from the waveform and uses these
   // information to set the frequency and phase of a synthesiser, provided
   // by the client. The freq_locked_synth generates phase accurate
   // synthesized output required for polyphonic sustain.
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Synth, std::uint32_t sps, std::uint32_t latency>
   class freq_locked_synth
   {
   public:

      enum { stop, wait, run };

      freq_locked_synth(Synth& synth_, uint32_t start_phase_)
       : _synth(synth_)
       , _start_phase(start_phase_)
      {}

      float operator()(float s, uint32_t ticks)
      {
         bool was_active = _agc.active();
         auto agc_out = _agc(s);
         int prev_state = _trig.state();
         bool is_active = _agc.active();

         if (!is_active)
         {
            _synth.period(0);
            _synth.phase(0);
            _edges_from_onset = 0;
            _stage = stop;
            return 0.0f;
         }

         int state = _trig(agc_out, is_active);
         bool onset = !was_active && is_active;

         if (prev_state != state && state)
         {
            if (!onset)
            {
               // update the synth frequency and phase
               float period = ticks - _edge_start;
               if (_edges_from_onset <= 1)
               {
                  _period_lp.y = period;
                  if (_stage != run)
                     _stage = wait;
               }
               else
               {
                  period = _period_lp(period);
               }

               _synth.period(period);
               std::size_t samples_delay = period - std::fmod(latency, period);
               auto target_phase = _start_phase - (samples_delay * _synth.freq());

               q::signed_phase_t phase_diff = target_phase - _synth.phase();
               auto shift = _synth.freq() * (phase_diff / q::pow2<float>(32));
               _synth.shift(shift);
            }
            _edge_start = ticks;
         }

         auto synth_out = _synth();
         if (_stage == wait)
         {
            if (_synth.is_start())
            	_stage = run;
            else
               return 0.0f;
         }
         return synth_out;
      }

   private:

      agc<sps>          _agc;
      period_trigger    _trig;
      Synth&            _synth;
      q::one_pole_lp    _period_lp = {0.6};
      uint32_t          _edge_start = 0;
      uint32_t          _start_phase;
      uint32_t          _edges_from_onset = 0;
      int               _stage = stop;
   };
}}

#endif
