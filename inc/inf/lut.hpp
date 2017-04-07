/*=================================================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=================================================================================================*/
#if !defined(CYCFI_INFINITY_LUT_JULY_22_2014)
#define CYCFI_INFINITY_LUT_JULY_22_2014

#include <inf/interpolation.hpp>
#include <inf/buffer.hpp>
#include <math.h>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // lut: a basic fractional lookup table (lut)
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <
      typename T
    , typename Storage = buffer<T>
    , typename Interpolation = sample_interpolation::linear>
   class lut
   {
   public:

      typedef T value_type;
      typedef Storage storage_type;
      typedef Interpolation interpolation_type;

      // constructor
      lut(T max_size)
       : buffer(std::size_t(ceil(max_size)))
      {}

      // copy and assign
      lut(lut const& rhs) = default;
      lut(lut&& rhs) = default;
      lut& operator=(lut const& rhs) = default;
      lut& operator=(lut&& rhs) = default;

      // size of lut
		std::size_t size() const
		{
			return buffer.size();
		}

      // push fresh data to the front of the lookup table
		friend void operator>>(T val, lut& l)
		{
         l.buffer.push(val);
		}

      // get data (index can be fractional)
      T operator[](T index) const
      {
         interpolation_type interpolate;
         return interpolate(buffer, index);
      }

   private:

        storage_type buffer;
   };
}}

#endif
