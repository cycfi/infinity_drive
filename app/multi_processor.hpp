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
   template <
      typename Base
    , std::size_t sampling_rate_ = 32000
    , std::size_t buffer_size_ = 1024
   >
   struct multi_processor : Base
   {   
      static constexpr auto sampling_rate = sampling_rate_;
      static constexpr auto adc_clock_rate = 2000000;
      static constexpr auto buffer_size = buffer_size_;
      static constexpr auto resolution = Base::resolution;

      static_assert(is_pow2(buffer_size),
         "buffer_size must be a power of 2, except 0"
      );
      
      static_assert(buffer_size > Base::n_samples, 
         "buffer_size must be greater than Base::n_samples"
      );

      multi_processor()
       : _clock(adc_clock_rate, sampling_rate)
       , _adc(_clock)
       , _out(_obuff.end())
       , _ocount(0)
      {}
      
      void start()
      {
         // channel 0, porta pin 0
         _adc.template enable_channel<0, 1>();
         _adc.start();
         _clock.start();
         _clock.enable_interrupt();
      }
      
      /////////////////////////////////////////////////////////////////////////
      // Interrupt handlers
      /////////////////////////////////////////////////////////////////////////   
      void irq_conversion_half_complete()
      {
         _out = _obuff.middle();
         _in = _ibuff.middle();
         
         // process channel 0 into the output buffer
         Base::process(_obuff.begin(), _obuff.middle(), _adc.begin(), 0);
      }

      void irq_conversion_complete()
      {
         _out = _obuff.begin();
         _in = _ibuff.begin();

         // process channel 0 into the output buffer
         Base::process(_obuff.middle(), _obuff.end(), _adc.middle(), 0);
      }
      
      void irq_timer_task()
      {
         if ((Base::n_samples == 1) || ((_ocount++ & (Base::n_samples-1)) == 0))
         {
            // We generate the output signals
            _dac_l((_out[0] * (resolution-1)) + resolution);
            _dac_l((_out[1] * (resolution-1)) + resolution);
            _out++;
         }
      }
      
   private:
      
      using timer_type = timer<2>;
      using adc_type = adc<1, 1, buffer_size>;
      using out_type = std::array<float, 2>;
      using obuff_type = dbuff<out_type, buffer_size / (2 * Base::n_samples)>;
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
   };
}}

#endif
