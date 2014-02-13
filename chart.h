#pragma once
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <assert.h>
#include "position.h"
#include "utility.h"
#include "vec.h"

namespace griddle
{

	template<typename P>
	class chart
	{
	public:
		typedef typename position<P> pos_t;
		size_t index(const pos_t& pos) const { return index_impl(validate(pos)); }
		inline bool in_range(const pos_t& pos) const { return !pos.nowhere() && in_range_impl(pos); }
		inline pos_t validate(const pos_t& pos) const { return in_range(pos) ? pos : pos_t(); }

		virtual size_t size() const = 0;
		virtual bool iterate(std::function<bool(pos_t)> callback) const = 0;
	protected:
		virtual bool in_range_impl(const pos_t& pos) const = 0;
		virtual size_t index_impl(const pos_t& pos) const = 0;
	};

	//implementations

	class bounded_grid : public chart<ivec>
	{
	public:
		bounded_grid(int left, int right, int top, int bottom) :
			mLeft(left), mRight(right), mTop(top), mBottom(bottom), mWidth(right - left), mHeight(bottom - top)
		{
			assert(left < right && top < bottom);
		}
		virtual size_t size() const { return mWidth*mHeight; }
		virtual bool iterate(std::function<bool(pos_t)> callback) const
		{
			for (int y = mTop; y < mBottom; y++) for (int x = mLeft; x < mRight; x++) if(!callback({ x, y })) return false;
			return true;
		}
	protected:
		virtual size_t index_impl(const pos_t& pos) const { return mWidth*(pos.y - mTop) + pos.x - mLeft; }
		virtual bool in_range_impl(const pos_t& pos) const { return in_rectangle(pos, mLeft, mRight, mTop, mBottom); }
	private:
		int mLeft;
		int mRight;
		int mTop;
		int mBottom;
		int mWidth;
		int mHeight;
	};

	class aligned_grid : public bounded_grid
	{
	public:
		aligned_grid(int width, int height) : bounded_grid(0, width, 0, height)
		{
			assert(width > 0 && height > 0);
		}
	};

	//TODO rename these to avoid confusion with the topologies that do similar jobs
	class torus_like : public chart<ivec>
	{
	public:
		int width() const { return mWidth; }
		int height() const { return mHeight; }
		torus_like(int width, int height, bool flip_x, bool flip_y) : mWidth(width), mHeight(height), mFlipX(flip_x), mFlipY(flip_y) {}
		virtual size_t size() const { return mWidth*mHeight; }
		virtual bool iterate(std::function<bool(pos_t)> callback) const
		{
			for (int y = 0; y < mHeight; y++) for (int x = 0; x < mWidth; x++) if (!callback({ x, y })) return false;
			return true;
		}
	protected:
		virtual size_t index_impl(const pos_t& pos) const
		{
			auto x = pos.x;
			auto y = pos.y;
			flip_xy(x, y);
			return mWidth*modulo(y, mHeight) + modulo(x, mWidth);
		}
		virtual bool in_range_impl(const pos_t& pos) const { return true; }
		virtual void flip_xy(int& x, int& y) const
		{
			if (mFlipX && flip(x, mWidth)) y = -y;
			if (mFlipY && flip(y, mHeight)) x = -x;
		}
		static inline bool flip(int coord, int m) { return (modulo(coord / m, 2) % 2) == 0; }
		bool mFlipX;
		bool mFlipY;
		int mWidth;
		int mHeight;
	};

	class torus : public torus_like
	{
	public:
		torus(int width, int height) : torus_like(width, height, false, false) {}
	};

	class klein_bottle : public torus_like
	{
	public:
		klein_bottle(int width, int height, bool flip_x) : torus_like(width, height, flip_x, !flip_x) {}
	};
	class real_projective_plane : public torus_like
	{
	public:
		real_projective_plane(int width, int height) : torus_like(width, height, true, true) {}
	};
	class sphere : public torus_like
	{
	public:
		sphere(int width, int height) : torus_like(width, height, true, true){}
	protected:
		virtual void flip_xy(int& x, int& y) const
		{
			bool quadrant_b_c = flip(x, mWidth);
			bool quadrant_c_d = flip(y, mWidth);
			int quadrant = 2 * (quadrant_c_d ? 1 : 0);
			quadrant += (quadrant_c_d ? !quadrant_b_c : quadrant_b_c);
			int tmp;
			for (int i = 0; i < quadrant; i++)
			{
				tmp = -y;
				y = x;
				x = tmp;
			}
		}
		bool mOrientation;
	};
	class moebius_like : public torus_like
	{
	public:
		moebius_like(int width, int height, bool wrap_x, bool flip_wrap) : torus_like(width, height, flip_wrap, flip_wrap), mWrapX(wrap_x)  {}
	protected:
		virtual bool in_range_impl(const pos_t& pos) const
		{
			if (mWrapX) return pos.y >= 0 && pos.y < mHeight;
			return pos.x >= 0 && pos.x < mHeight;
		}
		bool mWrapX;
	};

	class moebius_strip : public moebius_like
	{
	public:
		moebius_strip(int width, int height, bool wrap_x) : moebius_like(width, height, wrap_x, true) {}
	};


	class ring : public moebius_like
	{
	public:
		ring(int width, int height, bool wrap_x) : moebius_like(width, height, wrap_x, false) {}
	};

	template<typename P>
	class mapped_chart : public chart<P>
	{
	public:
		mapped_chart(std::initializer_list<P> init) { size_t i = 0; for (const auto& P : init) { mMap[P] = i; i++; } }
		virtual size_t size() const { return mChart.size(); }
		virtual bool iterate(std::function<bool(pos_t)> callback) const
		{
			for (const auto& it : mMap) if(!callback(pos_t(it.first))) return false;
			return true;
		}
		//void iterate(pos_t pos, std::function<void(pos_t)> callback) const
		//{
		//	iterate([](pos_t pos)
		//	{callback(pos); return true; });
		//};
	protected:
		virtual size_t index_impl(const pos_t& pos) const { return mMap.find(pos); }
		virtual bool in_range_impl(const pos_t& pos) const { return mMap.find(pos) != mMap.end(); }
	private:
		std::map<P, size_t> mMap;
	};

	template<typename P>
	class cross_chart : public chart<std::vector<position<P>>>
	{
	public:
		typedef typename chart<P>::pos_t sub_pos_t;
		cross_chart(std::vector<chart<P>> base_charts) : mBaseCharts(base_charts)
		{
			
		}
		virtual size_t size() const
		{
			size_t r = 1;
			for (const auto& c : mBaseCharts)
				r *= c.size();
			return r;
		}
		virtual bool iterate(std::function<bool(pos_t)> callback) const
		{
			pos_t current;
			return iterate_(callback, current);
		}
	protected:
		virtual bool iterate_(std::function<bool(pos_t) > callback, pos_t& current, int base_chart_number=0) const
		{
			if (base_chart_number < mBaseCharts.size()-1)
			{
				return mBaseCharts[base_chart_number].iterate(
					[&callback,&current](sub_pos_t pos)->bool
				{
					current[base_chart_number] = pos;
					return iterate_(callback, current, base_chart_number + 1);
				});
			}
			else if (base_chart_number = mBaseCharts.size()-1)
			{
				return mBaseCharts[base_chart_number].iterate(
					[&callback, &current](sub_pos_t pos)->bool
				{
					current[base_chart_number] = pos;
					return callback(current);
				}
				);
			}
			else { return false; }
		}
		virtual bool in_range_impl(const pos_t& pos) const
		{
			size_t b = 0;
			for (const auto& c : mBaseCharts)
			{
				if (!c.in_range(pos[b]))
					return false;
				b++;
			}
			return true;
		}
		virtual size_t index_impl(const pos_t& pos) const
		{
			size_t i = 0;
			size_t b = 0;
			for (const auto& c : mBaseCharts)
			{
				i *= c.size();
				i += c.size[pos[b]];
				b++;
			}
			return i;
		}
		const std::vector<chart<P>> mBaseCharts;
	};

	template<typename P>
	class union_chart : public chart<union_position<P>>
	{
	public:

		typedef position<union_position<P>> pos_t;
		union_chart(const std::vector<const chart<P>*>& components) :mComponents(components)
		{
			mSizes.push_back(0);
			for (const auto& c : mComponents)
			{
				mSizes.push_back(mSizes.back() + c->size());
			}
		}
		virtual size_t size() const { return mSizes.back(); }
		virtual bool iterate(std::function<bool(pos_t)> callback) const
		{
			int b = 0;
			for (const auto& c : mComponents)
			{
				if (!c->iterate([&callback,b](chart<P>::pos_t pos)->bool
				{
					return callback(pos_t(union_position<P>(pos, b)));
				}))
					return false;
				b++;
			}
			return true;
		}
	protected:
		const std::vector<const chart<P>*>& mComponents;
		std::vector<size_t> mSizes;

		virtual bool in_range_impl(const pos_t& pos) const
		{
			return mComponents[pos.get_component()]->in_range(pos.pos);
		}
		virtual size_t index_impl(const pos_t& pos) const
		{
			return mSizes[pos.get_component()] + mComponents[pos.get_component()]->index(pos.pos);
		}
	};
}