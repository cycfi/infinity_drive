/*=================================================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=================================================================================================*/
#if !defined(CYCFI_INFINITY_INTERPOLATION_JULY_20_2014)
#define CYCFI_INFINITY_INTERPOLATION_JULY_20_2014

#include <inf/support.hpp>
#include <cmath>
#include <cstddef>

namespace cycfi { namespace infinity
{
   namespace sample_interpolation
   {
      struct none
      {
         template <typename Storage, typename T>
         T operator()(Storage const& buffer, T index) const
         {
            return buffer[std::size_t(index)];
         }
      };

      struct linear
      {
         template <typename Storage, typename T>
         T operator()(Storage const& buffer, T index) const
         {
            auto y1 = buffer[std::size_t(index)];
            auto y2 = buffer[std::size_t(index) + 1];
            auto mu = index - std::floor(index);
            return linear_interpolate(y1, y2, mu);
         }
      };
   }
}}

#endif
