/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_PID_HPP_JUNE_10_2017)
#define CYCFI_INFINITY_PID_HPP_JUNE_10_2017

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // pid controller. No frills Proportional–integral–derivative controller.
   // The base class Config is expected to supply the required PID parameters.
   //
   // Example usage:
   //
   //    struct my_pid
   //    {
   //       float static constexpr p = 0.1f;          // Proportional gain
   //       float static constexpr i = 0.5f;          // Integral gain
   //       float static constexpr d = 0.01f;         // derivative gain
   //       float static constexpr sps = 100.0f;      // 100 Hz
   //    };
   //
   //    pid<my_pid> the_pid;
   //
   // The compiler will produce optimized code since p, i, d and sps are all
   // constant expressions.
   //
   ////////////////////////////////////////////////////////////////////////////
   template <typename Config>
   struct pid
   {
      float static constexpr kp = Config::p;          // Proportional gain
      float static constexpr ki = Config::i;          // Integral gain
      float static constexpr kd = Config::d;          // derivative gain
      float static constexpr dt = 1.0f / Config::sps; // Time interval

      float operator()(double set_point, double process_val)
      {
         auto const error = set_point - process_val;

         // Proportional
         auto const propertional = kp * error;

         // Integral
         _integral += error * dt;
         auto const integral = ki * _integral;

         // Derivative
         auto const derivative = kd * ((error - _prev_error) / dt);
         _prev_error = error;

         // Result
         return propertional + integral + derivative;
      }

   private:

      float _prev_error = 0.0f;
      float _integral = 0.0f;
   };
}}

#endif
