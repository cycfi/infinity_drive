/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/spi.hpp>
#include <inf/app.hpp>
#include <algorithm>

namespace inf = cycfi::infinity;

using inf::output_pin;
using inf::input_pin;
using inf::port_input_type;
using inf::delay_ms;
using inf::porta;
using inf::portb;
using inf::portc;
using inf::on;
using inf::off;
using inf::spi_master;
using inf::spi_slave;
using inf::error_handler;

auto const pull_up = port_input_type::pull_up;

void start()
{
   output_pin<porta+5> red_led; // porta, pin 5
   input_pin<portc+13, pull_up> button; // portc, pin 13
   red_led = off;

   // SPI master: SCK: portb+3, MOSI: portb+5
   spi_master<1, portb+3, portb+5> master{};

   // SPI slave: SCK: portc+10, MISO: portb+5
   spi_slave<3, portc+10, -1, portc+11> slave{};
   slave.enable();

   uint8_t out[] = "Hello SPI!!!";
   constexpr std::size_t len = sizeof(out);

   uint8_t in[len];
   slave.read(in, len);    // read is non-blocking

   // Wait for button press
   while (button)
   {
      delay_ms(100);
      red_led = !red_led;
   }

   master.enable();
   master.write(out, len); // write is non-blocking

   while (master.is_writing() || slave.is_reading())
      ; // this shouldn't take long!!!

   // compare the input and output. they should be the same
   if (!std::equal(std::begin(out), std::end(out), std::begin(in)))
      error_handler();

   while (true)
   {
      delay_ms(1000);
      red_led = !red_led;
   }
}
