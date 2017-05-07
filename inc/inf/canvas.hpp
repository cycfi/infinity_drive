/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_CANVAS_HPP_MAY_7_2015)
#define CYCFI_INFINITY_CANVAS_HPP_MAY_7_2015

#include <utility>
#include <cstdint>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // drawing canvas
   ////////////////////////////////////////////////////////////////////////////
   enum class color
   {
      white,
      black,
      inverse,
   };

   struct bitmap
   {
      std::uint8_t const* data;
      int width;
      int height;
   };

   template <std::size_t width_, std::size_t height_>
   struct canvas
   {
      enum { width  = width_, height = height_ };

      void clear();

      void draw_pixel(int x, int y, color color_);
      void draw_line(int x0, int y0, int x1, int y1, color color_);
      void fill_rect(int x, int y, int w, int h, color color_);
      void draw_rect(int x, int y, int w, int h, color color_);
      void draw_bitmap(bitmap const& img, int x, int y, color color_);

      void fast_hline(int x, int y, int w, color color_);
      void fast_vline(int x, int y, int h, color color_);

      std::uint8_t _buffer[(width * height) / 8];
   };

   ////////////////////////////////////////////////////////////////////////////
   // canvas implementation
   ////////////////////////////////////////////////////////////////////////////
   namespace detail
   {
      void fast_hline_impl(
         std::uint8_t* buffer, int width, int height,
         int x, int y, int w, color color_
      );

      void fast_vline_impl(
         std::uint8_t* buffer, int width, int height,
         int16_t x, int16_t y_, int16_t h_, color color_
      );

      void line_impl(
         std::uint8_t* buffer, int width, int height,
         int x0, int y0, int x1, int y1, color color_
      );
      
		void draw_bitmap(
			std::uint8_t* buffer, int width, int height,
			int x, int y, bitmap const& bm, color color_
		);

      inline void draw_pixel(
         std::uint8_t* buffer, int width, int height,
         int16_t x, int16_t y, color color_)
      {
         if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
            return;

         auto mask = y&7;
         auto* p = buffer + (x+(y/8)*width);

         switch (color_)
         {
            case color::white:
               *p |= 1 << mask;
               break;

            case color::black:
               *p &= ~1 << mask;
               break;

            case color::inverse:
               *p ^= 1 << mask;
               break;
         }
      }
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::clear()
   {
      for (auto& pixels : _buffer)
         pixels = 0;
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::fast_hline(int x, int y, int w, color color_)
   {
      detail::fast_hline_impl(_buffer, width, height, x, y, w, color_);
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::fast_vline(int x, int y, int h, color color_)
   {
      detail::fast_vline_impl(_buffer, width, height, x, y, h, color_);
   }

   template <std::size_t width, std::size_t height>
   void canvas<width, height>::draw_pixel(int x, int y, color color_)
   {
      detail::draw_pixel(_buffer, width, height, x, y, color_);
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::draw_line(int x0, int y0, int x1, int y1, color color_)
   {
      if (x0 == x1)
      {
         if (y0 > y1)
            std::swap(y0, y1);
         fast_vline(x0, y0, y1 - y0 + 1, color_);
      }
      else if (y0 == y1)
      {
         if(x0 > x1)
            std::swap(x0, x1);
         fast_hline(x0, y0, x1 - x0 + 1, color_);
      }
      else
      {
         detail::line_impl(_buffer, width, height, x0, y0, x1, y1, color_);
      }
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::fill_rect(int x, int y, int w, int h, color color_)
   {
      for (auto i=x; i<x+w; ++i)
         fast_vline(i, y, h, color_);
   }

   template <std::size_t width, std::size_t height>
   inline void canvas<width, height>::draw_rect(int x, int y, int w, int h, color color_)
   {
      fast_hline(x, y, w, color_);
      fast_hline(x, y+h-1, w, color_);
      fast_vline(x, y, h, color_);
      fast_vline(x+w-1, y, h, color_);
   }
}}

#endif
