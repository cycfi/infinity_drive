/*=============================================================================
   Copyright (c) 2015-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(INFINITY_DBUFF_OCTOBER_26_2014)
#define INFINITY_DBUFF_OCTOBER_26_2014

#include <cstddef>
#include <algorithm>
#include <array>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // dbuff: a simple fixed sized double (ping-pong) buffer
   ////////////////////////////////////////////////////////////////////////////
   template <typename T, std::size_t size_>
   class dbuff
   {
      typedef std::array<T, size_ * 2> array_type;

   public:

      static constexpr std::size_t size = size_;
      typedef typename array_type::const_iterator const_iterator;
      typedef typename array_type::iterator iterator;

      dbuff()
      {
         std::fill(data.begin(), data.end(), T{0});
      }

      const_iterator begin() const { return data.begin(); }
      iterator begin() { return data.begin(); }
      const_iterator middle() const { return data.begin() + size; }
      iterator middle() { return data.begin() + size; }
      const_iterator end() const { return data.end(); }
      iterator end() { return data.end(); }

   private:

      array_type data;
   };
}}

#endif
