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
         mpr121_i2c_addr      = 0x5A << 1,
         mhdr                 = 0x2B,    
         nhdr                 = 0x2C,                    
         nclr                 = 0x2D,             
         fdlr                 = 0x2E,    
         mhdf                 = 0x2F,
         nhdf                 = 0x30,
         nclf                 = 0x31,
         fdlf                 = 0x32,
         ecr                  = 0x5E,
         auto_config0         = 0x7B,   
         auto_config_upper    = 0x7D,    
         auto_config_lower    = 0x7E,
         auto_config_target   = 0x7F,
         touch_status         = 0x00,
         filter_data          = 0x04,
         baseline             = 0x1E,
         touch_threshold      = 0x41,
         release_threshold    = 0x42
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
      using namespace mpr121_constants;
  
      // Controls filtering when data is > baseline.
      write(mhdr, 0x01);
      write(nhdr, 0x01);                          
      write(nclr, 0x00);                 
      write(fdlr, 0x00);
      
      // Controls filtering when data is < baseline.
      write(mhdf, 0x01);
      write(nhdf, 0x01);             
      write(nclf, 0x7F);
      write(fdlf, 0x09); 

      // Set the electrode thresholds
      set_thresholds(12, 6);
 
      // Auto Configuration
      write(auto_config0, 0x1B);         
      write(auto_config_upper, 0xC9);   	
      write(auto_config_lower, 0x82); 
      write(auto_config_target, 0xB5);

      // Measurement electrodes and proximity detection electrode 
      // configurations and set to Run Mode
      write(ecr, 0x84);
   }

   template <typename Port, std::size_t timeout>
   void mpr121<Port, timeout>::set_thresholds(std::uint8_t touch, std::uint8_t release)
   {
      using namespace mpr121_constants;
      for (uint8_t i = 0; i != 12; i++)
      {
         write(touch_threshold + 2*i, touch);
         write(release_threshold + 2*i, release);
      }
   }

   template <typename Port, std::size_t timeout>
   std::uint16_t mpr121<Port, timeout>::filtered(std::uint8_t t) const
   {
      using namespace mpr121_constants;
      return (t > 12)? 0 : read16(filter_data + t*2);
   }
    
   template <typename Port, std::size_t timeout>
   std::uint16_t mpr121<Port, timeout>::baseline(std::uint8_t t) const
   {
      using namespace mpr121_constants;
      return (t > 12)? 0 : read8(baseline + t) << 2;
   }
    
   template <typename Port, std::size_t timeout>
   uint16_t mpr121<Port, timeout>::touched() const
   {
      using namespace mpr121_constants;
      return read16(touch_status) & 0x0FFF;
   }
}}

#endif
