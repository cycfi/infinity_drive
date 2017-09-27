/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_FEBRUARY_16_2016)
#define CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_FEBRUARY_16_2016

#include <inf/fx.hpp>
#include <inf/synth.hpp>
#include "agc.hpp"
#include "period_trigger.hpp"

namespace cycfi { namespace infinity
{
   template <typename Synth, std::uint32_t sps>
   struct freq_locked_synth
   {
      freq_locked_synth(Synth& synth_, uint32_t start_phase_)
       : _synth(synth_)
       , _start_phase(start_phase_)
      {}

      float operator()(float s, uint32_t ticks)
      {
         auto agc_out = _agc(s);
         int prev_state = _trig.state();
         bool is_active = _agc.active();
         int state = _trig(agc_out, is_active);

         if (!is_active)
            return 0.0f;
   
         if (prev_state != state && state)
         {
            // update the synth frequency and phase
            auto period = ticks - _nstart;
            if (period)
            {
               _synth.period(_period_lp(period));
               int32_t phase_diff = _synth.phase() - _start_phase;
               if (phase_diff > 0)
                  _synth.decr();
               else if (phase_diff < 0)
                  _synth.incr();
            }
            _nstart = ticks;
         }
         return _synth();
      }

      agc<sps>          _agc;
      period_trigger    _trig;
      Synth&            _synth;
      one_pole_lp  		_period_lp = {0.8};
      uint32_t          _nstart = 0;
      uint32_t          _start_phase;
   };
}}

#endif
