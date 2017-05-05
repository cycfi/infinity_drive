/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SPI_IMPL_HPP_MAY_4_2017)
#define CYCFI_INFINITY_SPI_IMPL_HPP_MAY_4_2017

#include <inf/detail/pin_impl.hpp>
//#include <inf/timer.hpp>
//#include <inf/detail/adc_impl.hpp>
//#include <array>
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"

namespace cycfi { namespace infinity { namespace detail
{
   template <std::size_t id>
   IRQn_Type spi_id();

   template <std::size_t id>
   uint32_t spi_periph_id();

   template <std::size_t id>
   SPI_TypeDef* get_spi();

   constexpr std::size_t num_spi() { return 3; }


#define INFINITY_SPI(N, PERIPH_ID)                                             \
                                                                               \
   template <>                                                                 \
   inline IRQn_Type spi_id<N>()                                                \
   {                                                                           \
      return SPI##N##_IRQn;                                                    \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline uint32_t spi_periph_id<N>()                                          \
   {                                                                           \
      return PERIPH_ID;                                       				   \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline SPI_TypeDef* get_spi<N>()                                            \
   {                                                                           \
      return SPI##N;                                                           \
   }                                                                           \
   /***/

   INFINITY_SPI(1, LL_APB2_GRP1_PERIPH_SPI1)
   INFINITY_SPI(2, LL_APB1_GRP1_PERIPH_SPI2)
   INFINITY_SPI(3, LL_APB1_GRP1_PERIPH_SPI3)

   void spi_config(
      std::size_t id,
      IRQn_Type spi_irqn,
      uint32_t periph_id,
      SPI_TypeDef* spi,
      bool master
   );

   template <typename GPIO, std::size_t pin>
   void spi_pin_config(GPIO* gpio)
   {
      static constexpr uint16_t bit = pin % 16;
      static constexpr uint32_t mask = 1 << bit;

      LL_GPIO_SetPinMode(gpio, mask, LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetAFPin_0_7(gpio, mask, LL_GPIO_AF_5);
      LL_GPIO_SetPinSpeed(gpio, mask, LL_GPIO_SPEED_FREQ_HIGH);
      LL_GPIO_SetPinPull(gpio, mask, LL_GPIO_PULL_DOWN);
   }

   template <std::size_t sck_pin, int mosi_pin, int miso_pin>
   void spi_pin_config()
   {
      static constexpr uint16_t port = sck_pin / 16;
      auto* gpio = &detail::get_port<port>();

      // Enable GPIO peripheral clock
      detail::enable_port_clock<port>();

      // Configure SCK Pin
      spi_pin_config<sck_pin>(gpio);

      // Configure MOSI Pin (unless it is -1)
      if (mosi_pin >= 0)
         spi_pin_config<mosi_pin>(gpio);

      // Configure MISO Pin (unless it is -1)
      if (mosi_pin >= 0)
         spi_pin_config<miso_pin>(gpio);
   }

   void spi_write(std::size_t id, std::uint8_t const* data, std::size_t len);
   bool spi_is_writing(std::size_t id);
   void spi_read(std::size_t id, std::uint8_t* data, std::size_t len);
   bool spi_is_reading(std::size_t id);

}}}

#endif
