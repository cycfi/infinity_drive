/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_FEBRUARY_16_2016)
#define CYCFI_INFINITY_FREQ_LOCKED_SYNTH_HPP_FEBRUARY_16_2016

#include <inf/fx.hpp>
#include <inf/period_detector.hpp>
#include <inf/synth.hpp>
#include <cmath>

namespace cycfi { namespace infinity
{
   template <typename Synth, std::uint32_t sps, std::uint32_t buffer_size>
   class freq_locked_synth
   {
   public:

      float attack = 1.0f / (0.1f * sps);
      float decay = 1.0f / (4.0f * sps);
      std::size_t start_phase = osc_phase(pi + (pi / 2));

      freq_locked_synth(Synth& synth_)
       : _synth(synth_)
       , _ref_synth(_synth.freq())
       , _period(0)
       , _count(0)
       , _samples_delay(0)
       , _synth_envelope(0)
      {}

      float synthesize()
      {
         _ref_synth();
         return _synth_envelope(_synth());
      }

      void update_attack()
      {
         // update the attack envelope
         if (_synth_envelope.a < 1.0f)
            _synth_envelope.a = std::min<float>(1.0f, _synth_envelope.a + attack);
      }

      void update_decay()
      {
         // update the decay envelope
         if (_synth_envelope.a > 0.0f)
            _synth_envelope.a = std::max<float>(0.0f, _synth_envelope.a - decay);
      }

      void detect_cycle_start(int state, int prev_state)
      {
         // detect the leading edge to synchronize the _ref_synth phase
         if ((state != prev_state) && (state > 0))
            _ref_synth.phase(start_phase - (_samples_delay * _ref_synth.freq()));


         //// detect the leading edge to synchronize the synth phase
         //if (_count == 0)
         //{
         //   // use a counter to compensate for the buffer latency
         //   if ((state != prev_state) && (state > 0))
         //      _count = _samples_delay;
         //}
         //else if (--_count == 0)
         //{
         //   // reset _ref_synth phase
         //   _ref_synth.phase(start_phase);
         //}
      }

      void update(float s)
      {
         auto const& wc = _pd.trigger.wc;
         int prev_state = wc.state;
         _period = _pd(s);

         if (_pd.gate.state > 0.0f)
         {
            update_attack();
            detect_cycle_start(wc.state, prev_state);
         }
         else
         {
            update_decay();
         }
      }

      float operator()(float s)
      {
         update(s);
         return synthesize();
      }

      void sync()
      {
         if (_pd.gate.state > 0.0f)
         {
            auto freq = sps/_period;
            _synth.freq(osc_freq(freq, sps));
            _ref_synth.freq(osc_freq(freq, sps));
            _samples_delay = (std::ceil(buffer_size / _period) * _period) - buffer_size;
         }
      }

      void sync_phase()
      {
         if (_synth.phase() != _ref_synth.phase())
            _synth.phase(_ref_synth.phase());
      }

      void run()                                   { _pd.run(); }
      accum const& ref_synth() const               { return _ref_synth; }

   protected: // $$$ fixme $$$

      period_detector<sps> _pd;
      Synth& _synth;
      accum _ref_synth;

      float _period;
      std::size_t _count;
      std::size_t _samples_delay;
      gain  _synth_envelope;
   };
}}

#endif
