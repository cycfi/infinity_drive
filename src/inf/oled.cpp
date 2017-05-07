/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/oled.hpp>

namespace cycfi { namespace infinity { namespace detail
{
   void fast_hline_impl(
      std::uint8_t* buffer, int width, int height,
      int x, int y, int w, color color_)
   {
      // Do bounds/limit checks
      if (y < 0 || y >= height)
         return;

      // make sure we don't try to draw below 0
      if (x < 0)
      {
         w += x;
         x = 0;
      }

      // make sure we don't go off the edge of the display
      if ((x + w) > width)
      {
         w = (width - x);
      }

      // if our width is now negative, punt
      if (w <= 0)
         return;

      // set up the pointer for  movement through the buffer
      auto* p = buffer;
      // adjust the buffer pointer for the current row
      p += (y/8) * width;
      // and offset x columns in
      p += x;

      auto mask = 1 << (y&7);

      switch (color_)
      {
         case color::white:
            while(w--)
               *p++ |= mask;
            break;

         case color::black:
            mask = ~mask;
            while(w--)
               *p++ &= mask;
            break;

         case color::inverse:
            while(w--)
               *p++ ^= mask;
            break;
      }
   }

   void fast_vline_impl(
      std::uint8_t* buffer, int width, int height,
      int16_t x, int16_t y_, int16_t h_, color color_)
   {
      // do nothing if we're off the left or right side of the screen
      if (x < 0 || x >= width)
         return;

      // make sure we don't try to draw below 0
      if (y_ < 0)
      {
        // y_ is negative, this will subtract enough from h_ to account for y_ being 0
         h_ += y_;
         y_ = 0;
      }

      // make sure we don't go past the height of the display
      if ((y_ + h_) > height)
         h_ = (height - y_);

      // if our height is now negative, punt
      if (h_ <= 0)
        return;

      // this display doesn't need ints for coordinates, use local byte registers for faster juggling
      auto y = y_;
      auto h = h_;

      // set up the pointer for fast movement through the buffer
      auto* p = buffer;
      // adjust the buffer pointer for the current row
      p += (y/8) * width;
      // and offset x columns in
      p += x;

      // do the first partial byte, if necessary - this requires some masking
      auto mod = y&7;
      if (mod)
      {
         // mask off the high n bits we want to set
         mod = 8-mod;

         // note - lookup table results in a nearly 10% performance improvement in fill* functions
         // register uint8_t mask = ~(0xFF >> (mod));
         static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
         auto mask = premask[mod];

         // adjust the mask if we're not going to reach the end of this byte
         if (h < mod)
            mask &= (0XFF >> (mod-h));

         switch (color_)
         {
            case color::white:   *p |=  mask;  break;
            case color::black:   *p &= ~mask;  break;
            case color::inverse: *p ^=  mask;  break;
         }

         // fast exit if we're done here!
         if (h < mod)
            return;

        h -= mod;
        p += width;
      }

         // write solid bytes while we can - effectively doing 8 rows at a time
         if (h >= 8)
         {
            if (color_ == color::inverse)
            {
               // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
               do
               {
                  *p=~(*p);

                  // adjust the buffer forward 8 rows worth of data
                  p += width;

                  // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                  h -= 8;
               } while (h >= 8);
            }
            else
            {
               // store a local value to work with
               auto val = (color_ == color::white) ? 255 : 0;

               do
               {
                  // write our value in
                  *p = val;

                  // adjust the buffer forward 8 rows worth of data
                  p += width;

                  // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                  h -= 8;
              } while (h >= 8);
            }
        }

      // now do the final partial byte, if necessary
      if (h)
      {
         mod = h & 7;
         // this time we want to mask the low bits of the byte, vs the high bits we did above
         // register uint8_t mask = (1 << mod) - 1;
         // note - lookup table results in a nearly 10% performance improvement in fill* functions
         static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
         auto mask = postmask[mod];
         switch (color_)
         {
            case color::white:   *p |=  mask;  break;
            case color::black:   *p &= ~mask;  break;
            case color::inverse: *p ^=  mask;  break;
         }
      }
    }
}}}
