/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_SPI_IMPL_HPP_MAY_4_2017)
#define CYCFI_INFINITY_SPI_IMPL_HPP_MAY_4_2017

#include <inf/detail/pin_impl.hpp>
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"

namespace cycfi { namespace infinity { namespace detail
{
   template <std::size_t id>
   IRQn_Type spi_id();

   template <std::size_t id>
   void spi_enable_clock();

   template <std::size_t id>
   SPI_TypeDef* get_spi();

   constexpr std::size_t num_spi() { return 3; }


#define INFINITY_SPI(N, PERIPH_TAG)                                            \
                                                                               \
   template <>                                                                 \
   inline IRQn_Type spi_id<N>()                                                \
   {                                                                           \
      return SPI##N##_IRQn;                                                    \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline void spi_enable_clock<N>()                                           \
   {                                                                           \
      PERIPH_TAG##_EnableClock(PERIPH_TAG##_PERIPH_SPI##N);     	             \
   }                                                                           \
                                                                               \
   template <>                                                                 \
   inline SPI_TypeDef* get_spi<N>()                                            \
   {                                                                           \
      return SPI##N;                                                           \
   }                                                                           \
   /***/

   INFINITY_SPI(1, LL_APB2_GRP1)
   INFINITY_SPI(2, LL_APB1_GRP1)
   INFINITY_SPI(3, LL_APB1_GRP1)

   void spi_config(
      std::size_t id,
      IRQn_Type spi_irqn,
      SPI_TypeDef* spi,
      bool master,
      bool half_duplex
   );

   template <std::size_t pin, bool is_half_duplex_slave, typename GPIO>
   void spi_pin_config(GPIO* gpio)
   {
      static constexpr uint16_t bit = pin % 16;
      static constexpr uint32_t mask = 1 << bit;

      LL_GPIO_SetPinMode(gpio, mask, LL_GPIO_MODE_ALTERNATE);
      if (is_half_duplex_slave)
         LL_GPIO_SetAFPin_8_15(gpio, mask, LL_GPIO_AF_6);
      else
         LL_GPIO_SetAFPin_0_7(gpio, mask, LL_GPIO_AF_5);
      LL_GPIO_SetPinSpeed(gpio, mask, LL_GPIO_SPEED_FREQ_HIGH);
      LL_GPIO_SetPinPull(gpio, mask, LL_GPIO_PULL_DOWN);
   }

   template <std::size_t sck_pin, int mosi_pin, int miso_pin, bool is_half_duplex_slave>
   void spi_pin_config()
   {
      static constexpr uint16_t port = sck_pin / 16;
      auto* gpio = &detail::get_port<port>();

      // Enable GPIO peripheral clock
      detail::enable_port_clock<port>();

      // Configure SCK Pin
      spi_pin_config<sck_pin, is_half_duplex_slave>(gpio);

      // Configure MOSI Pin (unless it is half duplex slave (-1))
      if (mosi_pin >= 0)
         spi_pin_config<mosi_pin, is_half_duplex_slave>(gpio);

      // Configure MISO Pin (unless it is half duplex master (-1))
      if (miso_pin >= 0)
         spi_pin_config<miso_pin, is_half_duplex_slave>(gpio);
   }

   void spi_write(std::size_t id, std::uint8_t const* data, std::size_t len);
   bool spi_is_writing(std::size_t id);
   void spi_read(std::size_t id, std::uint8_t* data, std::size_t len);
   bool spi_is_reading(std::size_t id);

}}}

#endif
