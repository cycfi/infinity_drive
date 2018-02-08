/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PLS_HPP_SEPTEMBER_27_2017)
#define CYCFI_INFINITY_PLS_HPP_SEPTEMBER_27_2017

#include "pll.hpp"
#include "agc.hpp"
#include "period_trigger.hpp"
#include "period_detector.hpp"
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

      enum { stop, wait, run, release };

      pls(Synth& synth_)
       : _agc(0.05f /* seconds */, sps)
       , _synth(synth_)
       , _start_phase(synth_.shift())
       , _target_phase(_start_phase)
      {}

      float operator()(float s, uint32_t sample_clock)
      {
         bool was_active = _agc.active();
         auto agc_out = _agc(s);
         int prev_state = _trig();
         bool is_active = _agc.active();

         // If we're not active, start the deactivation process
         if (!is_active)
            return deactivate();

         int state = _trig(agc_out, is_active);
         bool onset = !was_active && is_active;

         // Update phase every 512 clock cycles
         if (sample_clock & 512)
            _sync = true;

         if (prev_state != state && state)
         {
            if (!onset)
               sync(sample_clock);
            else
               _stage = wait;
            _edge_start = sample_clock;
         }

         // Compensate for latency by shifting the phase
         _synth.shift(_shift_lp(_target_phase));

         // Synthesize!
         auto val = _synth();

         // Avoid synthesizer startup glitch. Wait for zero-crossing.
         if (_stage == wait)
         {
            if (_synth.is_start())
               _stage = run;
            else
               return 0.0f;
         }
         return val;
      }

      float envelope() const
      {
         return _agc.envelope();
      }

   private:

      struct agc_config
      {
         static constexpr float max_gain = 50.0f;
         static constexpr float low_threshold = 0.01f;
         static constexpr float high_threshold = 0.05f;
      };

      void sync(uint32_t sample_clock)
      {
         if (_cycles++)
            _period_lp(sample_clock-_edge_start);
         else
            _period_lp = sample_clock-_edge_start;

         auto period = _period_lp();
         auto synth_freq = _synth.freq();

         auto new_freq = q::phase::period(period);
         _synth.freq(new_freq);

         if (_sync)
         {
            auto iperiod = std::lround(period);
            auto samples_delay = iperiod - latency % iperiod;
            _target_phase = _start_phase - (samples_delay * synth_freq);
            _target_phase -= _synth.phase();

            if (_cycles == 1)
               _shift_lp.y = _target_phase;
            _sync = false; // done sync
         }
      }

      // Release the synth
      float deactivate()
      {
         switch (_stage)
         {
            case stop:
               _edge_start = 0;
               _cycles = 0;
               _synth.phase(0);
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

      agc<agc_config>      _agc;
      period_trigger       _trig;
      Synth&               _synth;
      period_detector      _period_lp = { 0.4 };
      q::one_pole_lowpass  _shift_lp = { 0.001 };
      int                  _stage = stop;
      uint32_t             _cycles = 0;
      q::phase_t           _start_phase;
      uint32_t             _edge_start = 0;
      q::phase_t           _target_phase = 0;
      q::phase_t           _prev_freq = 0;
      bool                 _sync = false;
   };
}}

#endif
