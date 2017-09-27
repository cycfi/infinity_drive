/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_SEPTEMBER_27_2017

#include <inf/fx.hpp>
#include <inf/synth.hpp>
#include "agc.hpp"
#include "period_trigger.hpp"

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // freq_locked_synth
   //
   // The freq_locked_synth looks at the input audio and extracts the 
   // fundamental frequency and phase from the waveform and uses these
   // information to set the frequency and phase of a synthesiser, provided
   // by the client.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Synth, std::uint32_t sps>
   struct freq_locked_synth
   {
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
            _time_from_onset = 0;
            return 0.0f;
         }

         int state = _trig(agc_out, is_active);
         bool onset = !was_active && is_active;
   
         if (prev_state != state && state)
         {
            if (!onset)
            {
               // update the synth frequency and phase
               auto period = ticks - _edge_start;
               if (_time_from_onset < 10)
               {
                  _period_lp.y = period;
                  _synth.period(period);
                  _synth.phase(_start_phase);
               }
               else
               {
                  _synth.period(_period_lp(period));
                  int32_t phase_diff = _synth.phase() - _start_phase;
                  if (phase_diff > 0)
                     _synth.decr();
                  else if (phase_diff < 0)
                     _synth.incr();
               }
               ++_time_from_onset;
            }
            _edge_start = ticks;
         }
         return _synth();
      }

      agc<sps>          _agc;
      period_trigger    _trig;
      Synth&            _synth;
      one_pole_lp  		_period_lp = {0.8};
      uint32_t          _edge_start = 0;
      uint32_t          _start_phase;
      uint32_t          _time_from_onset = 0;
   };
}}

#endif
