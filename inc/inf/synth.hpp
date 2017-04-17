/*=============================================================================
   Copyright © 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SYNTH_HPP_DECEMBER_24_2015)
#define CYCFI_INFINITY_SYNTH_HPP_DECEMBER_24_2015

#include <inf/support.hpp>
#include <inf/sin_table.hpp>
#include <cstdint>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // osc_freq: given frequency (freq) and samples per second (sps),
   // calculate the fixed point frequency that the phase accumulator
   // (see below) requires.
   ////////////////////////////////////////////////////////////////////////////
   constexpr uint32_t osc_freq(double freq, uint32_t sps)
   {
      return (int_max<uint32_t>() * freq) / sps;
   }

   ////////////////////////////////////////////////////////////////////////////
   // osc_phase: given phase (in radians), calculate the fixed point phase
   // that the phase accumulator (see below) requires. phase runs from
   // 0 to uint32_max (0 to 2pi)
   ////////////////////////////////////////////////////////////////////////////
   constexpr uint32_t osc_phase(double phase)
   {
      return int_max<uint32_t>() * (phase / _2pi);
   }

   ////////////////////////////////////////////////////////////////////////////
   // accum phase synthesizer
   ////////////////////////////////////////////////////////////////////////////
   class accum
   {
   public:

      // we use fixed point computations where
      //    freq = (uint32_max * frequency) / samples_per_second
      //    phase runs from 0 to uint32_max (0 to 2pi)

      accum(uint32_t freq)
       : _freq(freq)
       , _phase(0)
      {}

      accum(float freq, uint32_t sps)
       : _freq(osc_freq(freq, sps))
       , _phase(0)
      {}

      uint32_t operator()()
      {
         auto val = _phase;
         _phase += _freq;
         return val;
      }

      bool is_phase_start() const
      {
         // return true if we are at the start phase
         return _phase < _freq;
      }

      uint32_t freq() const { return _freq; }
      uint32_t phase() const { return _phase; }

      void freq(uint32_t freq) { _freq = freq; }
      void freq(uint32_t freq, uint32_t sps) { _freq = osc_freq(freq, sps); }
      void phase(uint32_t phase) { _phase = phase; }

   private:

      uint32_t _freq;
      uint32_t _phase;
   };

   ////////////////////////////////////////////////////////////////////////////
   // pulse synthesizer (this is not bandwidth limited)
   ////////////////////////////////////////////////////////////////////////////
   class pulse
   {
   public:

      pulse(uint32_t freq, uint32_t width)
       : base(freq)
       , width(width)
      {}

      pulse(float freq, float width, uint32_t sps)
       : base(osc_freq(freq, sps))
       , width(width * int_max<uint32_t>())
      {}

      float operator()()
      {
         return base() > width ? 1.0f : -1.0f;
      }

      uint32_t freq() const { return base.freq(); }
      uint32_t phase() const { return base.phase(); }

      void freq(uint32_t freq) { base.freq(freq); }
      void freq(uint32_t freq, uint32_t sps) { base.freq(freq, sps); }
      void phase(uint32_t phase) { base.freq(phase); }

   private:

      accum base;
      uint32_t width;
   };

   ////////////////////////////////////////////////////////////////////////////
   // sin wave synthesizer
   ////////////////////////////////////////////////////////////////////////////
   class sin
   {
   public:

      sin(uint32_t freq)
       : base(freq)
      {}

      sin(float freq, uint32_t sps)
       : base(osc_freq(freq, sps))
      {}

      float operator()()
      {
         return sin_gen(base());
      }

      uint32_t freq() const { return base.freq(); }
      uint32_t phase() const { return base.phase(); }

      void freq(uint32_t freq) { base.freq(freq); }
      void freq(uint32_t freq, uint32_t sps) { base.freq(freq, sps); }
      void phase(uint32_t phase) { base.phase(phase); }

   private:

      accum base;
   };

   ////////////////////////////////////////////////////////////////////////////
   // two operator FM synth
   ////////////////////////////////////////////////////////////////////////////
   class fm
   {
   public:

      fm(uint32_t mfreq, float mgain_, uint32_t cfreq)
       : mbase(mfreq)
       , mgain(fxp(mgain_) * 32767)
       , cbase(cfreq)
      {
      }

      fm(float mfreq, float mgain_, float cfreq, uint32_t sps)
       : mbase(mfreq, sps)
       , mgain(fxp(mgain_) * 32767)
       , cbase(cfreq, sps)
      {
      }

      float operator()()
      {
         int32_t modulator_out = sin_gen(mbase()) * mgain;
         return sin_gen(cbase() + modulator_out);
      }

      uint32_t freq() const { return cbase.freq(); }
      uint32_t phase() const { return cbase.phase(); }

      void freq(uint32_t freq) { cbase.freq(freq); }
      void freq(uint32_t freq, uint32_t sps) { cbase.freq(freq, sps); }
      void phase(uint32_t phase) { cbase.freq(phase); }

   private:

      accum mbase;   // modulator phase synth
      int32_t mgain; // modulator gain (1.31 bit fixed point)
      accum cbase;   // carrier phase synth
   };
}}

#endif
