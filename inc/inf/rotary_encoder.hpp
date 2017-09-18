/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017)
#define CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017

#include <inf/pin.hpp>

namespace cycfi { namespace infinity
{
   template <
      std::size_t A, std::size_t B, 
      std::size_t steps_ = 100, bool acceleration_ = false>
   class rotary_encoder
   {
   public:

      using pin_a_type = input_pin<A, port::pull_up>;
      using pin_b_type = input_pin<B, port::pull_up>;
      static std::size_t const steps = steps_;
      static bool const acceleration = acceleration_;

      float operator()() const
      {
         return value;
      }

      void on_edge()
      {
         if (pin_b)
         {
            value += 1.0f / steps;
            if (value > 1.0f)
               value = 1.0f;
         }
         else
         {
            value -= 1.0f / steps;          
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

   private:
   
      pin_a_type pin_a;
      pin_b_type pin_b;
      float value;
   };
}}

#endif
