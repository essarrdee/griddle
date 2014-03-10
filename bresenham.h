#pragma once
#include <functional>
#include "topology.h"


// Original Source:
///**
//* @author Zingl Alois
//* @date 17.12.2012
//* @version 1.1
//*/
// This code has no copyright and could be used and modified by anyone as wanted.
// (see http://members.chello.at/~easyfilter/Bresenham.pdf)
//
//void plotLine(int x0, int y0, int x1, int y1)
//{
//	int dx = abs(x1 - x0), sx = x0<x1 ? 1 : -1;
//	int dy = -abs(y1 - y0), sy = y0<y1 ? 1 : -1;
//	int err = dx + dy, e2;                                   /* error value e_xy */
//
//	for (;;){                                                          /* loop */
//		setPixel(x0, y0);
//		e2 = 2 * err;
//		if (e2 >= dy) {                                         /* e_xy+e_x > 0 */
//			if (x0 == x1) break;
//			err += dy; x0 += sx;
//		}
//		if (e2 <= dx) {                                         /* e_xy+e_y < 0 */
//			if (y0 == y1) break;
//			err += dx; y0 += sy;
//		}
//	}
//}
//
// Modifications below by Simon Donnelly, 09.02.2014
// Same non-license.


namespace griddle
{

	inline bool nop(int x, int y) { return true; }

	inline bool plot_line_both(
		int x0, int y0, int x1, int y1,
		std::function<bool(int x, int y)> location_callback,
		std::function<bool(int dx, int dy)> delta_callback
		)
	{
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2;                                   /* error value e_xy */
		for (;;){                                                          /* loop */
			if (!location_callback(x0, y0)) return false;
			e2 = 2 * err;
			if (e2 >= dy) {                                         /* e_xy+e_x > 0 */
				if (x0 == x1) return true;
				err += dy; x0 += sx;
			}
			if (e2 <= dx) {                                         /* e_xy+e_y < 0 */
				if (y0 == y1) return true;
				err += dx; y0 += sy;
			}
			if (!delta_callback((e2 >= dy) ? sx : 0, (e2 <= dx) ? sy : 0)) return false;
		}
	}
	inline bool plot_line_locations(
		int x0, int y0, int x1, int y1,
		std::function<bool(int x, int y)> location_callback)
	{
		return plot_line_both(x0, y0, x1, y1, location_callback, nop);
	}
	inline bool plot_line_deltas(
		int x0, int y0, int x1, int y1,
		std::function<bool(int x, int y)> delta_callback)
	{
		return plot_line_both(x0, y0, x1, y1, nop, delta_callback);
	}

	template<typename P, typename Dir>
	class bresenham_pather
	{
	public:
		typedef typename topology<P, Dir> topo_t;
		typedef typename position<P> pos_t;//topo_t::pos_t pos_t;
		bool follow_line(const topo_t& top, pos_t pos, int delta_x, int delta_y,
			std::function<bool(pos_t)> callback)
		{
			return plot_line_deltas(0, 0, delta_x, delta_y, [&](int dx, int dy)->bool
			{
				pos = top.neighbour(pos, get_direction(dx, dy));
				return callback(pos);
			});
		}
		virtual Dir get_direction(int x, int y) const = 0;
	};

	template<typename P>
	class bresenham_pather_8 : public bresenham_pather<P,dir8>
	{
		virtual dir8 get_direction(int x, int y) const {
			return griddle::get_dirN<8>(x, y);
		}
		
	};
}