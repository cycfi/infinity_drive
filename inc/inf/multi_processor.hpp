/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_MULTI_PROCESSOR_HPP_MAY_16_2017)
#define CYCFI_INFINITY_MULTI_PROCESSOR_HPP_MAY_16_2017

#include <inf/adc.hpp>
#include <inf/dac.hpp>
#include <inf/dbuff.hpp>
#include <inf/support.hpp>
#include <inf/app.hpp>
#include <inf/pin.hpp>
#include <inf/processor.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // A multi-channel digital signal processor.
   //
   // Works in conjunction with the processor class (see processor.hpp).
   // We read the signal from ADC channels, process the signals and send
   // the result to two DAC channels.
   //
   // - Base conforms to the processor requirements
   // - Base must declare some configuration constants:
   //
   //   - oversampling:    The oversampling factor
   //   - adc_id:          The id of the ADC we want to use (see adc.hpp)
   //   - timer_id:        The id of the timer we want to use (see timer.hpp)
   //   - channels:        The number of ADC channels
   //   - sampling_rate:   The ADC sampling rate
   //   - buffer_size:     The buffer size (see below)
   //
   //   Example:
   //
   //      static constexpr auto oversampling = 4;
   //      static constexpr auto adc_id = 1;
   //      static constexpr auto timer_id = 2;
   //      static constexpr auto channels = 1;
   //      static constexpr auto sampling_rate = 44100;
   //      static constexpr auto buffer_size = 1024;
   //
   // - The
   //
   // - Base must have a setup_channels function responsible for setting up
   //   the ADC channels (using the adc enable_channel member function). The
   //   setup_channels function signature is as follows:
   //
   //       template <typename Adc>
   //       void setup_channels(Adc& adc);
   //
   //   Example:
   //
   //       template <typename Adc>
   //       void setup_channels(Adc& adc)
   //       {
   //          adc.template enable_channel<0, 1>();
   //       }
   //
   // The ADC is double buffered. The DAC output, after processing, will be
   // delayed by a number of samples (the buffer size). For example, with a
   // sampling rate of 32kHz and a buffer size of 1024, the delay will be
   // 16ms. This delay is computed as:
   //
   //    ((1 / 32000) * 1024) / 2
   //
   // We divide by two because the ADC is double buffered and calls our
   // processing routine twice. First, when it finishes sampling half the
   // buffer size (adc_conversion_half_complete) and then again when it
   // concludes the entire conversion for the entire buffer
   // (adc_conversion_complete)
   //
   // - The irq_conversion_half_complete() member function must be called
   //   from the irq(adc_conversion_half_complete<1>) interrupt function.
   //
   // - The irq_conversion_complete() member function must be called
   //   from the irq(irq_conversion_complete<1>) interrupt function.
   //
   // - The irq_timer_task() member function must be called from the
   //   irq(timer_task<timer_id>) interrupt function.
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class multi_channel_processor : public Base
   {
   public:

      static constexpr auto adc_id = Base::adc_id;
      static constexpr auto timer_id = Base::timer_id;
      static constexpr auto channels = Base::channels;
      static constexpr auto buffer_size = Base::buffer_size;
      static constexpr auto adc_clock_rate = 2000000;

      using adc_type = adc<adc_id, channels, buffer_size>;
      using timer_type = timer<timer_id>;

      static constexpr auto sampling_rate = Base::sampling_rate;
      static constexpr auto resolution = adc_type::resolution;
      static constexpr auto half_resolution = resolution / 2;
      static constexpr auto oversampling = Base::oversampling;

      static_assert(q::is_pow2(buffer_size),
         "buffer_size must be a power of 2, except 0"
      );

      static_assert(buffer_size > Base::oversampling,
         "buffer_size must be greater than Base::oversampling"
      );

      multi_channel_processor()
       : _out(_obuff.end())
       , _ocount(0)
      {}

      template <std::size_t... channels>
      auto config()
      {
         // Config the timer (clock)
         auto cfg1 = _clock.setup(
            adc_clock_rate,
            sampling_rate,
            [this]() { this->irq_timer_task(); }
         );

         // Config the ADC
         auto cfg2 = _adc.setup(
               _clock,
               [this]() { this->irq_conversion_half_complete(); },
               [this]() { this->irq_conversion_complete(); }
            );

         // Config the ADC channels
         auto cfg3 = _adc.template enable_channels<channels...>();

         // Config the DACs
         auto cfg4 = _dac_l.setup();
         auto cfg5 = _dac_r.setup();

#ifdef INFINITY_PROFILER
         auto cfg6 = _profile_pin.setup();
#endif
         return [=](auto base)
         {
#ifdef INFINITY_PROFILER
            return cfg1(cfg2(cfg3(cfg4(cfg5(cfg6(base))))));
#else
            return cfg1(cfg2(cfg3(cfg4(cfg5(base)))));
#endif
         };
      }

      void start()
      {
         _adc.start();
         _clock.start();
         _clock.enable_interrupt();
      }

      /////////////////////////////////////////////////////////////////////////
      // Interrupt handlers
      /////////////////////////////////////////////////////////////////////////
      static float convert(std::uint32_t sample)
      {
         return (sample / float(half_resolution)) - 1.0f;
      }

      void irq_conversion_half_complete()
      {
         _out = _obuff.middle();

#ifdef INFINITY_PROFILER
         _profile_pin = 1;
#endif
         // process channels and place them in the output buffer
         Base::process(
            _obuff.begin(), _obuff.middle(), _adc.begin(),
            [](std::uint32_t sample) { return convert(sample); }
         );

#ifdef INFINITY_PROFILER
         _profile_pin = 0;
#endif
      }

      void irq_conversion_complete()
      {
         _out = _obuff.begin();

#ifdef INFINITY_PROFILER
         _profile_pin = 1;
#endif
         // process channels and place them in the output buffer
         Base::process(
            _obuff.middle(), _obuff.end(), _adc.middle(),
            [](std::uint32_t sample) { return convert(sample); }
         );

#ifdef INFINITY_PROFILER
         _profile_pin = 0;
#endif
      }

      void irq_timer_task()
      {
         if ((Base::oversampling == 1) || ((_ocount++ & (Base::oversampling-1)) == 0))
         {
            // We generate the output signals
            _dac_l(((*_out)[0] * (half_resolution-1)) + half_resolution);
            _dac_r(((*_out)[1] * (half_resolution-1)) + half_resolution);
            _out++;
         }
      }

   private:

      using out_type = std::array<float, 2>;
      using obuff_type = dbuff<out_type, buffer_size / (2 * Base::oversampling)>;
      using oiter_type = typename obuff_type::iterator;

      // The main clock
      timer_type _clock;

      // The ADC
      adc_type _adc;

      // The DACs
      dac<0> _dac_l;
      dac<1> _dac_r;

      // The Output buffer and iterator
      obuff_type _obuff;
      oiter_type _out;

      // output count (for downsampling)
      int _ocount;

#ifdef INFINITY_PROFILER
      // For profiling
      main_test_pin_type _profile_pin;
#endif
   };

}}

#endif
