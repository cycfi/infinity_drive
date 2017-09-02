/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_MPR121_HPP_SEPTEMBER_2_2017)
#define CYCFI_INFINITY_MPR121_HPP_SEPTEMBER_2_2017

#include <inf/support.hpp>
#include <inf/canvas.hpp>
#include <cstdint>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // mpr121 driver
   ////////////////////////////////////////////////////////////////////////////
   template <typename Port, std::size_t timeout_ = 0xffffffff>
   struct mpr121
   {
   public:

      using port = Port;
      static std::size_t const timeout = timeout_;

                     mpr121(Port& io_);
      void           set_thresholds(std::uint8_t touch, std::uint8_t release);
      std::uint16_t  filtered(std::uint8_t t) const;
      std::uint16_t  baseline(std::uint8_t t) const;
      uint16_t       touched() const;

   private:

      void           write(std::uint16_t addr, std::uint8_t data);
      std::uint8_t   read8(std::uint16_t addr) const;
      std::uint16_t  read16(std::uint16_t addr) const;
      
      Port&          _io;
   };

   namespace mpr121_constants
   {
      enum
      {
         mpr121_i2c_addr = 0x5A << 1


         #define MPR121_I2CADDR_DEFAULT 0x5A
         
         #define MPR121_TOUCHSTATUS_L 0x00
         #define MPR121_TOUCHSTATUS_H 0x01
         #define MPR121_FILTDATA_0L  0x04
         #define MPR121_FILTDATA_0H  0x05
         #define MPR121_BASELINE_0   0x1E
         #define MPR121_MHDR         0x2B
         #define MPR121_NHDR         0x2C
         #define MPR121_NCLR         0x2D
         #define MPR121_FDLR         0x2E
         #define MPR121_MHDF         0x2F
         #define MPR121_NHDF         0x30
         #define MPR121_NCLF         0x31
         #define MPR121_FDLF         0x32
         #define MPR121_NHDT         0x33
         #define MPR121_NCLT         0x34
         #define MPR121_FDLT         0x35
         
         #define MPR121_TOUCHTH_0    0x41
         #define MPR121_RELEASETH_0    0x42
         #define MPR121_DEBOUNCE 0x5B
         #define MPR121_CONFIG1 0x5C
         #define MPR121_CONFIG2 0x5D
         #define MPR121_CHARGECURR_0 0x5F
         #define MPR121_CHARGETIME_1 0x6C
         #define MPR121_ECR 0x5E
         #define MPR121_AUTOCONFIG0 0x7B
         #define MPR121_AUTOCONFIG1 0x7C
         #define MPR121_UPLIMIT   0x7D
         #define MPR121_LOWLIMIT  0x7E
         #define MPR121_TARGETLIMIT  0x7F
         
         #define MPR121_GPIODIR  0x76
         #define MPR121_GPIOEN  0x77
         #define MPR121_GPIOSET  0x78
         #define MPR121_GPIOCLR  0x79
         #define MPR121_GPIOTOGGLE  0x7A

         #define MPR121_SOFTRESET 0x80
         
      };
   }

   template <typename Port, std::size_t timeout>
   inline void mpr121<Port, timeout>::write(std::uint16_t addr, std::uint8_t data)
   {
      using namespace mpr121_constants;
      _io.mem_write8(mpr121_i2c_addr, addr, data, timeout);
   }

   template <typename Port, std::size_t timeout>
   inline std::uint8_t mpr121<Port, timeout>::read8(std::uint16_t addr) const
   {
      using namespace mpr121_constants;
      return _io.mem_read8(mpr121_i2c_addr, addr, timeout);
   }

   template <typename Port, std::size_t timeout>
   inline std::uint16_t mpr121<Port, timeout>::read16(std::uint16_t addr) const
   {
      using namespace mpr121_constants;
      return _io.mem_read16(mpr121_i2c_addr, addr, timeout);
   }

   template <typename Port, std::size_t timeout>
   inline mpr121<Port, timeout>::mpr121(Port& io_)
    : _io(io_)
   {      
      // soft reset
      write(MPR121_SOFTRESET, 0x63);
      delay_ms(1);
   
      write(MPR121_ECR, 0x0);
   
      uint8_t c = read8(MPR121_CONFIG2);
      if (c != 0x24)
         return;
   
      set_thresholds(12, 6);
      write(MPR121_MHDR, 0x01);
      write(MPR121_NHDR, 0x01);
      write(MPR121_NCLR, 0x0E);
      write(MPR121_FDLR, 0x00);
   
      write(MPR121_MHDF, 0x01);
      write(MPR121_NHDF, 0x05);
      write(MPR121_NCLF, 0x01);
      write(MPR121_FDLF, 0x00);
   
      write(MPR121_NHDT, 0x00);
      write(MPR121_NCLT, 0x00);
      write(MPR121_FDLT, 0x00);
   
      write(MPR121_DEBOUNCE, 0);
      write(MPR121_CONFIG1, 0x10); // default, 16uA charge current
      write(MPR121_CONFIG2, 0x20); // 0.5uS encoding, 1ms period
   
      write(MPR121_ECR, 0x8F);  // start with first 5 bits of baseline tracking
   }

   template <typename Port, std::size_t timeout>
   void mpr121<Port, timeout>::set_thresholds(std::uint8_t touch, std::uint8_t release)
   {
      for (uint8_t i=0; i<12; i++) 
      {
         write(MPR121_TOUCHTH_0 + 2*i, touch);
         write(MPR121_RELEASETH_0 + 2*i, release);
      }
   }

   template <typename Port, std::size_t timeout>
   std::uint16_t mpr121<Port, timeout>::filtered(std::uint8_t t) const
   {
      if (t > 12) 
         return 0;
      return read16(MPR121_FILTDATA_0L + t*2);
   }
    
   template <typename Port, std::size_t timeout>
   std::uint16_t mpr121<Port, timeout>::baseline(std::uint8_t t) const
   {
      if (t > 12) 
         return 0;
      uint16_t bl = read8(MPR121_BASELINE_0 + t);
      return (bl << 2);
   }
    
   template <typename Port, std::size_t timeout>
   uint16_t mpr121<Port, timeout>::touched() const
   {
      uint16_t t = read16(MPR121_TOUCHSTATUS_L);
      return t & 0x0FFF;
   }
}}

#endif
