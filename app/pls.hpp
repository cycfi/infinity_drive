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

      pls(Synth& synth_)
       : _agc(0.05f /* seconds */, sps)
       , _pll(synth_)
       , _start_phase(synth_.shift())
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

         // We are running
         _stage = run;
         int state = _trig(agc_out, is_active);
         bool onset = !was_active && is_active;

         if (prev_state != state && state)
         {
            if (!onset)
            {
               // latency compensation
               auto period = sample_clock - _edge_start;
               std::size_t samples_delay = period - (latency % period);

               if (_start++ < 10)
               {
                  if (_start > 1)
                  {
                     auto new_freq = q::phase::period(period);
                     synth().freq(new_freq);
                     _pll._lf._freq = new_freq;
                  }
               }
               _start = 0;

               auto freq = synth().freq();
               auto shift = _start_phase - (samples_delay * freq);
               synth().shift(_shift_lp(shift));

               // auto curr_shift = synth().shift();
               // if (curr_shift < shift)
               //    freq = -freq;
               // synth().shift(curr_shift + freq);
            }
            _edge_start = sample_clock;
         }

         // Update the pll
         auto val = _pll(state);
         return val;
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

      // Release the synth
      float deactivate()
      {
         switch (_stage)
         {
            case stop:
               _edge_start = 0;
               _start = 0;
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
      q::one_pole_lp    _shift_lp = { 0.05 };
      int               _start = 0;
      int               _stage = stop;
      q::phase_t        _start_phase;
      uint32_t          _edge_start = 0;
      q::phase_t        _target_phase = 0;
   };
}}

#endif
