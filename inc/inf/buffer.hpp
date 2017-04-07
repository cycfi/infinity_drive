/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_BUFFER_JULY_12_2014)
#define CYCFI_INFINITY_BUFFER_JULY_12_2014

#include <vector>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // buffer: a simple fixed size buffer using a ring buffer.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename T>
	class buffer
	{
	public:

		explicit buffer(std::size_t size)
       : pos(0)
		{
         // allocate the data in a size that is a power of two for efficient indexing
         std::size_t capacity = smallest_pow2(size);
         mask = capacity - 1;
         data.resize(capacity, T());
      }

      buffer(buffer const& rhs) = default;
      buffer(buffer&& rhs) = default;
      buffer& operator=(buffer const& rhs) = default;
      buffer& operator=(buffer&& rhs) = default;

      // size of buffer
		std::size_t size() const
		{
			return data.size();
		}

      // push the latest element, overwriting the oldest element
		void push(T val)
		{
         --pos &= mask;
			data[pos] = val;
		}

      // get the nth latest element (b[0] is latest element. b[1] is the second latest)
		T operator[](std::size_t index) const
		{
			return data[(pos + index) & mask];
		}

      // get the nth latest element (b[0] is latest element. b[1] is the second latest)
		T& operator[](std::size_t index)
		{
			return data[(pos + index) & mask];
		}

	private:

		std::size_t mask;
		std::size_t pos;
		std::vector<T> data;
	};
}}

#endif
