/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017)
#define CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017

#include <inf/pin.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // rotary_encoder. Driver for quadrature rotary encoders.
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t A, std::size_t B, std::size_t steps_>
   class rotary_encoder
   {
   public:

      using pin_a_type = input_pin<A, port::pull_up>;
      using pin_b_type = input_pin<B, port::pull_up>;

      // Number of pulses per full 360 degree rotation
      static constexpr std::size_t steps = steps_;

      // The number of rotations needed to get the full range from 
      // 0.0 to 1.0, at the lowest (fine tune) speed.
      static constexpr float rotations = 4.5;
      
      void operator()(float value_)
      {
         value = value_;
      }

      float operator()() const
      {
         return value;
      }

      void on_edge()
      {
         auto now = millis();
         auto elapsed = now - time;
         time = now;

         // debounce
         if (elapsed < 5)
            return;

         float div = steps * rotations;

         // acceleration
         constexpr float min_fast = (1.0f / steps) * 5;
         constexpr float max_fast = 1.0f - min_fast;
         if (elapsed < 100 && value > min_fast && value < max_fast)
            div *= map<float>(elapsed, 5, 100, 0.01, 1);
         auto incr = fast_inverse(div);

         if (pin_b)
         {
            value += incr;
            if (value > 1.0f)
               value = 1.0f;
         }
         else
         {
            value -= incr;
            if (value < 0.0f)
               value = 0.0f;        
         }
      }

      auto setup(std::size_t priority = 0)
      {
         auto task = [this]()
         {
            this->on_edge();
         };

         auto cfg_pin_a = pin_a.setup(task, priority);
         auto cfg_pin_b = pin_b.setup();
         return [cfg_pin_a, cfg_pin_b](auto base)
         {
            return cfg_pin_a(cfg_pin_b(base));
         };
      }

      void start(port_edge edge = port::falling_edge)
      {
         pin_a.start(edge);
      }

   private:
   
      pin_a_type pin_a;
      pin_b_type pin_b;
      float value = 0.0f;
      std::uint32_t time = 0.0f;
   };
}}

#endif
