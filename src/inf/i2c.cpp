/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/detail/i2c_impl.hpp>
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace cycfi { namespace infinity { namespace detail
{
   I2C_HandleTypeDef hi2c3;

   // I2C SPEEDCLOCK define to max value: 400 KHz
   #define I2C_SPEEDCLOCK                 400000
   #define I2C_DUTYCYCLE                  I2C_DUTYCYCLE_2
   #define SSD1306_ADDR                   (0x78)

   void setup_i2c_pin(GPIO_TypeDef& gpio, std::uint32_t pin_mask)
   {
      LL_GPIO_SetPinMode(&gpio, pin_mask, LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetAFPin_8_15(&gpio, pin_mask, LL_GPIO_AF_4);
      LL_GPIO_SetPinSpeed(&gpio, pin_mask, LL_GPIO_SPEED_FREQ_HIGH);
      LL_GPIO_SetPinOutputType(&gpio, pin_mask, LL_GPIO_OUTPUT_OPENDRAIN);
      LL_GPIO_SetPinPull(&gpio, pin_mask, LL_GPIO_PULL_UP);
   }

   void i2c_config(
      GPIO_TypeDef& sda_gpio, std::uint32_t sda_pin_mask,
      GPIO_TypeDef& scl_gpio, std::uint32_t scl_pin_mask
   )
   {
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      hi2c3.Instance = I2C3;
      hi2c3.Init.ClockSpeed = I2C_SPEEDCLOCK;
      hi2c3.Init.DutyCycle = I2C_DUTYCYCLE;
      hi2c3.Init.OwnAddress1 = 0;
      hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
      hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
      hi2c3.Init.OwnAddress2 = 0;
      hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
      hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

      // Enable the peripheral clock of GPIOA and GPIOC
      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

      // Configure SDA and SCL Pins
      setup_i2c_pin(sda_gpio, sda_pin_mask);
      setup_i2c_pin(scl_gpio, scl_pin_mask);

      // Peripheral clock enable
      __HAL_RCC_I2C3_CLK_ENABLE();

      HAL_I2C_Init(&hi2c3);
   }

   void i2c_write(uint8_t const* data, uint32_t len, uint32_t timeout)
   {
      HAL_I2C_Master_Transmit(&hi2c3, SSD1306_ADDR, const_cast<uint8_t*>(data), len, timeout);
   }
}}}
