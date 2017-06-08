/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_CONTROL_ACQUISITION_HPP_MAY_16_2017)
#define CYCFI_INFINITY_CONTROL_ACQUISITION_HPP_MAY_16_2017

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
   // Control Acquisition
   //
   // This class is designed for acquiring analog controls data. We read the
   // signal from ADC channels and present the values (uint32_t) to a base
   // processor class.
   //
   // (See multiprocessor.hpp for a more elaborate signal acquisition and
   // digital signal processing scheme.)
   //
   // - Base must declare some configuration constants:
   //
   //   - adc_id:          The id of the ADC we want to use (see adc.hpp)
   //   - timer_id:        The id of the timer we want to use (see timer.hpp)
   //   - channels:        The number of ADC channels
   //   - sampling_rate:   The ADC sampling rate
   //   - buffer_size:     The buffer size (see below)
   //
   //   Example:
   //
   //      static constexpr auto adc_id = 1;
   //      static constexpr auto timer_id = 3;
   //      static constexpr auto channels = 1;
   //      static constexpr auto sampling_rate = 1000;
   //      static constexpr auto buffer_size = 8;
   //
   // - Base must have a process member function with the signature:
   //
   //       template <typename T>
   //       void process(std::array<T, channels> const& in);
   //
   //       - in: Will hold the input values for each channel, at the
   //         native ADC resolution (e.g. 12 bits for the STM32F4 ADC).
   //
   //       - Note that we make it a template to make the data type generic,
   //         thereby making it future proof for ADCs with greater precision.
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
   class control_acquisition : public Base
   {
   public:

      static constexpr auto adc_id = Base::adc_id;
      static constexpr auto timer_id = Base::timer_id;
      static constexpr auto channels = Base::channels;
      static constexpr auto buffer_size = Base::buffer_size;
      static constexpr auto adc_clock_rate = 2000000;
      static constexpr auto sampling_rate = Base::sampling_rate;

      using adc_type = adc<adc_id, channels, buffer_size>;
      using timer_type = timer<timer_id>;

      static_assert(is_pow2(buffer_size),
         "buffer_size must be a power of 2, except 0"
      );

      control_acquisition()
       : _clock(adc_clock_rate, sampling_rate)
       , _adc(_clock)
      {}

      void start()
      {
         Base::setup_channels(_adc);

         _adc.start();
         _clock.start();
         _clock.enable_interrupt();
      }

      /////////////////////////////////////////////////////////////////////////
      // Interrupt handlers
      /////////////////////////////////////////////////////////////////////////
      void irq_conversion_half_complete()
      {
         // process channels
         for (auto i = _adc.begin(); i != _adc.middle(); ++i)
            Base::process(*i);
      }

      void irq_conversion_complete()
      {
         // process channels
         for (auto i = _adc.middle(); i != _adc.end(); ++i)
            Base::process(*i);
      }

   private:

      // The main clock
      timer_type _clock;

      // The ADC
      adc_type _adc;
   };
}}

#endif
