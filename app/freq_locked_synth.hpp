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

      enum { stop, run, release };

      freq_locked_synth(Synth& synth_, q::signed_phase_t start_phase_)
       : _agc(0.05f /* seconds */, sps)
       , _synth(synth_)
       , _start_phase(start_phase_)
      {}

      float operator()(float s, uint32_t sample_clock)
      {
         bool was_active = _agc.active();
         auto agc_out = _agc(s);
         int prev_state = _trig();
         bool is_active = _agc.active();

         if (!is_active)
            return deactivate();

         _stage = run;
         int state = _trig(agc_out, is_active);
         bool onset = !was_active && is_active;

         if (prev_state != state && state)
            sync(sample_clock, onset);
         return _synth();
      }

      float envelope() const
      {
         return _agc.envelope();
      }

      q::signed_phase_t start_phase() const
      {
         return _start_phase;
      }

      void start_phase(q::signed_phase_t start_phase_)
      {
         _start_phase = start_phase_;
      }

   private:

      struct agc_config
      {
         static constexpr float max_gain = 50.0f;
         static constexpr float low_threshold = 0.01f;
         static constexpr float high_threshold = 0.05f;
      };

      // Synchronize the synth
      void sync(uint32_t sample_clock, bool onset)
      {
         if (!onset)
         {
            // update the synth frequency and phase
            float period = _period_lp(sample_clock - _edge_start);
            _synth.period(period);
            std::size_t samples_delay = period - std::fmod(latency, period);
            auto target_phase = _start_phase - (samples_delay * _synth.freq());

            q::signed_phase_t phase_diff = target_phase - _synth.phase();
            auto shift = (((long long)_synth.freq()) * phase_diff) >> 32;
            _synth.shift(shift);
         }
         _edge_start = sample_clock;
      }

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
               if (_synth.is_start())
               {
                  _stage = stop;
                  return 0.0f;
               }
               return _synth();
         }
         return 0.0f;
      }

      agc<agc_config>   _agc;
      period_trigger    _trig;
      Synth&            _synth;
      q::one_pole_lp    _period_lp = {0.4};
      uint32_t          _edge_start = 0;
      uint32_t          _start_phase;
      int               _stage = stop;
   };
}}

#endif
