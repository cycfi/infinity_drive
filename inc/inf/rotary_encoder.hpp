/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017)
#define CYCFI_INFINITY_ROTARY_ECNODER_HPP_SEPTEMBER_13_2017

#include <inf/pin.hpp>
#include <q/support.hpp>

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

      // The number of rotations needed to get the full range
      // from 0.0 to 1.0, at the highest speed.
      static constexpr float fast_rotations = 1.0f;
      static constexpr float fast_incr = 1.0f / (fast_rotations * steps);

      rotary_encoder(float incr = fast_incr / 2.0f)
       : incr(incr) {}

      void operator()(float value_)
      {
         value = value_;
      }

      float operator()() const
      {
         return value;
      }

      float increment() const
      {
         return incr;
      }

      void increment(float incr_)
      {
         incr = incr_;
      }

      void on_edge()
      {
         auto now = millis();
         auto elapsed = now - time;
         time = now;

         // debounce
         if (elapsed < 5)
            return;

         float scaled_incr = (elapsed < 20)?
            fast_incr :
            incr * std::max<float>(1.0f, (1000.0f * q::fast_inverse(elapsed)))
         ;

         if (pin_b)
         {
            value += scaled_incr;
            if (value > 1.0f)
               value = 1.0f;
         }
         else
         {
            value -= scaled_incr;
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
      float incr;
      std::uint32_t time = 0;
   };

   template <typename T>
   class encoder_param
   {
   public:

      encoder_param(T& encoder, float init, float offset, float scale, float incr)
       : encoder(encoder)
       , value((init - offset) / scale)
       , offset(offset)
       , scale(scale)
       , incr(incr / scale)
      {}

      void activate()
      {
         encoder(value);
         encoder.increment(incr);
         active = true;
      }

      void deactivate()
      {
         value = encoder();
         encoder.increment();
         active = false;
      }

      float operator()() const
      {
         return offset + ((active? encoder() : value) * scale);
      }

   private:

      T& encoder;
      float value;
      float offset;
      float scale;
      float incr;
      bool active = false;
   };
}}

#endif
