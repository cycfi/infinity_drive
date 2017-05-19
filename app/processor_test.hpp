/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PROCESSOR_TEST_HPP_MAY_16_2017)
#define CYCFI_INFINITY_PROCESSOR_TEST_HPP_MAY_16_2017

#include <inf/adc.hpp>
#include <inf/dac.hpp>
#include <inf/dbuff.hpp>
#include <inf/support.hpp>
#include <inf/app.hpp>
#include <inf/pin.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // A monophonic processor for DSP testing purposes
   //
   // We read the signal from ADC channel 1 using pin PC0, process it and
   // send the result output through the DAC pin PA4. The ADC is buffered. 
   // The DAC output, after processing will be delayed by a number of samples 
   // (the buffer size). With a sampling 
   // rate of 32kHz, the delay will be 16ms. This delay is computed as:
   //
   //    ((1/32000) * 1024) / 2
   //
   // We divide by two because the ADC calls our processing routine twice.
   // First when it finishes sampling half the buffer size:
   //
   //    adc_conversion_half_complete
   //
   // and then again when it concludes the entire conversion for the entire 
   // buffer:
   //
   //    adc_conversion_complete
   //
   // Setup: Connect an input signal (e.g. signal gen) to pin PA0. Connect
   // pin PA4 to an oscilloscope to see the waveform. 
   ////////////////////////////////////////////////////////////////////////////
   template <
      typename Processor
    , std::size_t sampling_rate_ = 32000
    , std::size_t buffer_size_ = 1024
   >
   struct mono_processor : Processor
   {
      static constexpr auto sampling_rate = sampling_rate_;
      static constexpr auto adc_clock_rate = 2000000;
      static constexpr auto buffer_size = buffer_size_;
      static constexpr auto capacity = buffer_size * 2;
      
      mono_processor()
       : _clock(adc_clock_rate, sampling_rate)
       , _adc(_clock)
       , _out(_obuff.end())
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
      // The main process function:
      //    1) Converts data from the ADC (12 bits unsigned int) to float
      //    2) Performs some processing 
      //       - The base Processor class must have a process function with
      //         the signature float process(float);
      //    3) Copies the processed data to the output buffer
      /////////////////////////////////////////////////////////////////////////
      template <typename I1, typename I2>
      inline void process(I1 first, I1 last, I2 src)
      {
         for (auto i = first; i != last; ++i)
            *i = Processor::process(((*src++)[0] / float(capacity)) - 1.0f);
      }
      
      /////////////////////////////////////////////////////////////////////////
      // Interrupt handlers
      /////////////////////////////////////////////////////////////////////////   
      void irq_conversion_half_complete()
      {
         _out = _obuff.middle();
         process(_obuff.begin(), _obuff.middle(), _adc.begin());
      }

      void irq_conversion_complete()
      {
         _out = _obuff.begin();
         process(_obuff.middle(), _obuff.end(), _adc.middle());
      }
      
      void irq_timer_task()
      {
         // We generate a 12 bit signal
         _dac((*_out++ * (capacity-1)) + capacity);
      }
      
   private:
      
      using timer_type = timer<2>;
      using adc_type = adc<1, 1, buffer_size>;
      using dac_type = dac<0>;
      using obuff_type = dbuff<float, buffer_size/2>;
      using iter_type = typename obuff_type::iterator;

      // The main clock
      timer_type _clock;

      // The ADC
      adc_type _adc;

      // The DAC
      dac<0> _dac;

      // The Output buffer and iterator
      obuff_type _obuff;
      iter_type _out;
   };
}}

#endif
