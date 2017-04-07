/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
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
   public:

      static constexpr std::size_t size = size_;
      static constexpr std::size_t capacity = size * 2;
      typedef std::array<T, capacity> array_type;
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
