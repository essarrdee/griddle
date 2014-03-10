#pragma once
#include <stdlib.h>
namespace griddle
{
	template<typename I, I max>
	class vec
	{
	public:
		inline vec(I x_ = max, I y_ = max)
			: x(x_), y(y_) {};
		I x;
		I y;
		inline bool operator==(const vec& rhs) const { return x == rhs.x && y == rhs.y; }
		inline bool operator!=(const vec& rhs) const { return !((*this) == rhs); }
		inline bool operator<(const vec& rhs) const
		{
			if (y < rhs.y) return true;
			if (y > rhs.y) return false;
			return x < rhs.x;
		}
	};

	template<typename I, I max>
	inline static vec<I,max> operator+(const vec<I, max>& lhs, const vec<I, max>& rhs)
	{
		return vec<I, max>(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	template<typename I, I max>
	inline static vec<I, max> operator*(const vec<I, max>& lhs, const vec<I, max>& rhs)
	{
		return vec<I, max>(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	template<typename I, I max>
	inline static vec<I, max> operator*(const vec<I, max>& lhs, const I& rhs)
	{
		return lhs * vec<I,max>{ rhs, rhs };
	}
	template<typename I, I max>
	inline static vec<I, max> operator*(const I& lhs, const vec<I, max>& rhs)
	{
		return rhs * lhs;
	}
	template<typename I, I max>
	inline static vec<I, max> operator-(const vec<I, max>& lhs, const vec<I, max>& rhs)
	{
		return lhs + ((-1)*rhs);
	}
	template<typename I, I max>
	inline bool in_rectangle(vec<I, max> xy, I left, I right, I top, I bottom)
	{
		return  xy.x >= left && xy.y >= top && xy.x < right && xy.y < bottom;
	}
	template<typename I, I max>
	inline bool in_grid_range(vec<I,max> xy, I width, I height)
	{
		return in_rectangle<I, max>(xy, 0, width, 0, height);
	}
	typedef vec<int, INT_MAX> ivec;
	typedef vec<size_t, SIZE_MAX> svec;
}
