/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/support.hpp>
#include <inf/detail/spi_impl.hpp>

namespace cycfi { namespace infinity { namespace detail
{
   struct spi_io_data
   {
      SPI_TypeDef* spi = nullptr;

      std::uint8_t const* write_data = nullptr;
      std::size_t write_len = 0;
      std::size_t write_index = 0;

      std::uint8_t* read_data = nullptr;
      std::size_t read_len = 0;
      std::size_t read_index = 0;
   };

   namespace
   {
      // We have an spi_io_data for each spi peripheral
      spi_io_data io_data[num_spi()];
   }

   void spi_config(
      std::size_t id,
      IRQn_Type spi_irqn,
      SPI_TypeDef* spi,
      bool master,
      bool half_duplex
   )
   {
      // Check if the spi peripheral is already used
      if (io_data[id-1].spi)
         error_handler();

      io_data[id-1].spi = spi;

      // Configure NVIC for SPI1 transfer complete/error interrupts
      NVIC_SetPriority(spi_irqn, 0);
      NVIC_EnableIRQ(spi_irqn);

      // Configure SPI communication
      LL_SPI_SetBaudRatePrescaler(spi, LL_SPI_BAUDRATEPRESCALER_DIV256);

      if (half_duplex)
      {
         if (master)
            LL_SPI_SetTransferDirection(spi, LL_SPI_HALF_DUPLEX_TX);
         else
            LL_SPI_SetTransferDirection(spi, LL_SPI_HALF_DUPLEX_RX);
      }
      else
      {
         LL_SPI_SetTransferDirection(spi, LL_SPI_FULL_DUPLEX);
      }

      LL_SPI_SetClockPhase(spi, LL_SPI_PHASE_2EDGE);
      LL_SPI_SetClockPolarity(spi, LL_SPI_POLARITY_HIGH);

      // Reset value is LL_SPI_MSB_FIRST
      LL_SPI_SetTransferBitOrder(spi, LL_SPI_MSB_FIRST);
      LL_SPI_SetDataWidth(spi, LL_SPI_DATAWIDTH_8BIT);
      LL_SPI_SetNSSMode(spi, LL_SPI_NSS_SOFT);
      LL_SPI_SetRxFIFOThreshold(spi, LL_SPI_RX_FIFO_TH_QUARTER);

      LL_SPI_SetMode(spi, master? LL_SPI_MODE_MASTER : LL_SPI_MODE_SLAVE);

      // Configure SPI transfer error interrupt
      LL_SPI_EnableIT_ERR(spi);
   }

   void spi_write(std::size_t id, std::uint8_t const* data, std::size_t len)
   {
      auto iodata_p = &io_data[id-1];
      io_data[id-1].write_data = data;
      io_data[id-1].write_len = len;
      io_data[id-1].write_index = 0;
      LL_SPI_EnableIT_TXE(io_data[id-1].spi);
   }

   bool spi_is_writing(std::size_t id)
   {
      return LL_SPI_IsActiveFlag_BSY(io_data[id-1].spi);

      //return (io_data[id-1].write_index != io_data[id-1].write_len)
      //   && LL_SPI_IsActiveFlag_TXE(io_data[id-1].spi);
   }

   void spi_read(std::size_t id, std::uint8_t* data, std::size_t len)
   {
      io_data[id-1].read_data = data;
      io_data[id-1].read_len = len;
      io_data[id-1].read_index = 0;
      LL_SPI_EnableIT_RXNE(io_data[id-1].spi);
   }

   bool spi_is_reading(std::size_t id)
   {
      return io_data[id-1].read_index != io_data[id-1].read_len;
   }

   void spi_irq_handler(std::size_t id, SPI_TypeDef* spi)
   {
      auto* iodata_p = &io_data[id-1];

      // Check TXE flag value in ISR register
      if (LL_SPI_IsActiveFlag_TXE(spi))
      {
         LL_SPI_TransmitData8(iodata_p->spi, iodata_p->write_data[iodata_p->write_index++]);
         if (iodata_p->write_index == iodata_p->write_len)
            LL_SPI_DisableIT_TXE(spi);
      }

      // Check RXNE flag value in ISR register
      if (LL_SPI_IsActiveFlag_RXNE(spi))
      {
         iodata_p->read_data[iodata_p->read_index++] = LL_SPI_ReceiveData8(spi);
         if (iodata_p->read_index == iodata_p->read_len)
            LL_SPI_DisableIT_RXNE(spi);
      }

      // Check STOP flag value in ISR register
      else if (LL_SPI_IsActiveFlag_OVR(spi))
      {
         LL_SPI_DisableIT_RXNE(spi);
         LL_SPI_DisableIT_TXE(spi);
         error_handler();
      }
   }

}}}

extern "C"
{
#define SPI_CALLBACK(N)                                                        \
   void SPI##N##_IRQHandler()                                                  \
   {                                                                           \
      cycfi::infinity::detail::spi_irq_handler(N, SPI##N);                     \
   }                                                                           \
   /***/

#ifdef SPI1
  SPI_CALLBACK(1)
#endif

#ifdef SPI2
  SPI_CALLBACK(2)
#endif

#ifdef SPI3
  SPI_CALLBACK(3)
#endif

}
