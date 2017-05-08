/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <inf/canvas.hpp>
#include <cstdlib>
#include <inf/detail/fonts.h>
#include <inf/detail/roboto8_font.h>
#include <inf/detail/roboto12_font.h>
#include <inf/detail/roboto20_font.h>

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

      while (w--)
         draw_byte(*p++, mask, color_);
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
         // y_ is negative, this will subtract enough from h_ 
         // to account for y_ being 0
         h_ += y_;
         y_ = 0;
      }

      // make sure we don't go past the height of the display
      if ((y_ + h_) > height)
         h_ = (height - y_);

      // if our height is now negative, punt
      if (h_ <= 0)
        return;

      // this display doesn't need ints for coordinates, use local byte 
      // registers for faster juggling
      auto y = y_;
      auto h = h_;

      // set up the pointer for fast movement through the buffer
      auto* p = buffer;
      // adjust the buffer pointer for the current row
      p += (y / 8) * width;
      // and offset x columns in
      p += x;

      // do the first partial byte, if necessary - this requires some masking
      auto mod = y & 7;
      if (mod)
      {
         // mask off the high n bits we want to set
         mod = 8 - mod;

         // note - lookup table results in a nearly 10% performance improvement 
         // in fill* functions uint8_t mask = ~(0xFF >> (mod));
         static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
         auto mask = premask[mod];

         // adjust the mask if we're not going to reach the end of this byte
         if (h < mod)
            mask &= (0XFF >> (mod-h));

         draw_byte(*p, mask, color_);

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
            // separate copy of the code so we don't impact performance of the 
            // black/white write version with an extra comparison per loop
            do
            {
               *p = ~(*p);

               // adjust the buffer forward 8 rows worth of data
               p += width;

               // adjust h & y (there's got to be a faster way for me to do this, 
               // but this should still help a fair bit for now)
               h -= 8;
            } 
            while (h >= 8);
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

               // adjust h & y (there's got to be a faster way for me to do this, 
               // but this should still help a fair bit for now)
               h -= 8;
            } 
            while (h >= 8);
         }
      }

      // now do the final partial byte, if necessary
      if (h)
      {
         mod = h & 7;
         // this time we want to mask the low bits of the byte, vs the high 
         // bits we did above: uint8_t mask = (1 << mod) - 1;
         // note - lookup table results in a nearly 10% performance improvement 
         // in fill* functions
         static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
         auto mask = postmask[mod];
         draw_byte(*p, mask, color_);
      }
   }

   void line_impl(
      std::uint8_t* buffer, int width, int height,
      int x0, int y0, int x1, int y1, color color_
   )
   {
      auto steep = std::abs(y1 - y0) > abs(x1 - x0);
      if (steep)
      {
         std::swap(x0, y0);
         std::swap(x1, y1);
      }

      if (x0 > x1)
      {
         std::swap(x0, x1);
         std::swap(y0, y1);
      }

      auto dx = x1 - x0;
      auto dy = std::abs(y1 - y0);

      auto err = dx / 2;
      int ystep;

      if (y0 < y1)
         ystep = 1;
      else
         ystep = -1;

      for (; x0<=x1; x0++)
      {
         if (steep)
            draw_pixel(buffer, width, height, y0, x0, color_);
         else
            draw_pixel(buffer, width, height, x0, y0, color_);
         err -= dy;
         if (err < 0)
         {
            y0 += ystep;
            err += dx;
         }
      }
   }

   void draw_bitmap(
      std::uint8_t* buffer, int width, int height,
      int x, int y, bitmap const& bm, color color_
   )
   {
      int const cols = bm.width;
      int const rows = (bm.height + 7) / 8;
      auto dest = buffer + x + ((y / 8) * width);
      auto src = bm.data;
      
      for (int yi = 0; yi < rows; yi++)
      {
         for (int xi = 0; xi < cols; xi++)
         {
            if (xi + x < width)
            {
               auto yoffs = y % 8;
               auto pix = src[xi];
               auto out = dest + xi;
               
               draw_byte(*out, pix << yoffs, color_);              
               if (yoffs && ((y + 8) < height))
                  draw_byte(*(out + width), pix >> (8 - yoffs), color_);
            }
         }
         dest += width; 
         src += cols;
      }
   }
   
   int draw_char(
       std::uint8_t* buffer, int width, int height,
       int x, int y, char ch, font font_, color color_
   )
   {
      FONT_INFO const* font_info = nullptr;
      switch (font_)
      {
         default:
         case font::small:
            font_info = &verdana_8ptFontInfo; /*&robotoBk_8ptFontInfo; /*/ //&Terminal_8ptFontInfo;
            break;
         case font::medium:
            font_info = &verdana_12ptFontInfo; //&roboto_12ptFontInfo; // MedProp_11ptFontInfo;
            break;
         case font::large:
            font_info = &roboto_20ptFontInfo; // LCDLarge_24ptFontInfo;
            break;
      }
      
      // See if we are not out of bounds:
      if (ch < font_info->StartCharacter || ch > font_info->EndCharacter)
         return 0;

      // StartCharacter is the first glyph in the table
      ch -= font_info->StartCharacter;

      // The descriptors have 2 data: glyph width and offset to actual pixels
      std::size_t glyph_index = ch * 2; 

      unsigned int const* descr = font_info->Descriptors;
      int const glyph_height = font_info->CharacterHeight * 8;
      int const glyph_width = descr[glyph_index];
      std::uint8_t const* address = font_info->Bitmaps + descr[glyph_index + 1];

      bitmap const bm = { address, glyph_width, glyph_height };
      draw_bitmap(buffer, width, height, x, y, bm, color_);
      return x + glyph_width + 2;
   }
}}}
