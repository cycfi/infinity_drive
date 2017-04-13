/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#include <inf/timer.hpp>
#include <inf/pin.hpp>
#include <inf/app.hpp>

namespace inf = cycfi::infinity;
using inf::portc;

void start()
{
   // 4-channel PWM

   inf::timer<3> pwm(8000000, 256);

   pwm.enable_channel<0, portc + 6>();
   pwm.enable_channel<1, portc + 7>();
   pwm.enable_channel<2, portc + 8>();
   pwm.enable_channel<3, portc + 9>();

   pwm.width<0>(10);
   pwm.width<1>(90);
   pwm.width<2>(127);
   pwm.width<3>(200);

   pwm.enable();

   while (true)
      ;
}
