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

      enum { stop, wait, run, release };

      pls(Synth& synth_)
       : _agc(0.05f /* seconds */, sps)
       , _pll(synth_)
       , _start_phase(synth_.shift())
       , _target_shift(_start_phase)
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

         if (sample_clock & 256)
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
         synth().shift(_shift_lp(_target_shift));

         // Update the pll
         auto val = _pll(state);
         return (_stage != run)? 0.0f : val;
      }

      Synth& synth()
      {
         return _pll._synth;
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
         {
            _period_lp(sample_clock-_edge_start);
            _stage = run;
         }
         else
         {
            _period_lp.y = (sample_clock-_edge_start) * period_filter_k;
         }

         auto period = _period_lp() / period_filter_k;
         auto synth_freq = synth().freq();

         if (_cycles < 8)
         {
            auto new_freq = q::phase::period(period);
            synth().freq(new_freq);
         }

         if (_sync)
         {
            // Dynamic filter. We increase the time constant with
            // decreasing frequency. The higher the frequency, the
            // lower the time constant.
            _shift_lp.a = 0.05f / period;

            auto synth_period = q::one_cyc / synth_freq;
            period = (period + synth_period) / 2;
            auto samples_delay = period - (latency % period);
            _target_shift = _start_phase - (samples_delay * synth_freq);
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
               synth().phase(0);
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

      static constexpr auto period_filter_k = q::pow2<int32_t>(2);
      using period_filter_t = q::fixed_pt_leaky_integrator<period_filter_k>;

      agc<agc_config>   _agc;
      period_trigger    _trig;
      pll<Synth>        _pll;
      period_filter_t   _period_lp;
      q::one_pole_lp    _shift_lp = { 0.001 };
      int               _stage = stop;
      uint32_t          _cycles = 0;
      q::phase_t        _start_phase;
      uint32_t          _edge_start = 0;
      q::phase_t        _target_shift = 0;
      q::phase_t        _prev_freq = 0;
      bool              _sync = false;
   };
}}

#endif
