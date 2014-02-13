#pragma once
#include <algorithm>
#include "vec.h"
#include "chart.h"
#include "container.h"
#include "directions.h"

namespace griddle
{
	template<typename P, typename Dir>
	class topology
	{
	public:
		typedef typename position<P> pos_t;
		typedef Dir dir_t;

		void iterate_neighbourhood(pos_t pos, const std::function<void(pos_t)>& callback) const
		{
			iterate_neighbourhood_stop(pos, [callback](pos_t pos)->bool
			{callback(pos); return true; });
		};
		std::vector<pos_t> build_neighbourhood(pos_t pos) const
		{
			std::vector<pos_t> r;
			iterate_neighbourhood(pos, [&r](pos_t p)->void{r.push_back(p); });
			return r;
		}

		virtual void iterate_neighbourhood_stop(pos_t pos, std::function<bool(pos_t)> callback) const = 0;
		virtual pos_t neighbour(const pos_t& pos, const dir_t& direction) const = 0;
	};

	template<typename P, typename Dir, typename Del, typename It = typename griddle::direction_set<Dir>::iterator>
	class step_topology : public topology<P, Dir>
	{
	public:
		typedef typename Del del_t;
		//typedef typename griddle::direction_set<dir_t> dir_set;
		//typedef typename griddle::delta_map del_map;
		typedef typename griddle::direction_delta_set<dir_t, del_t, It> dir_del_set;
		step_topology(const dir_del_set& dirs_dels) : mDirs(dirs_dels) {}
		virtual void iterate_neighbourhood_stop(pos_t pos, std::function<bool(pos_t)> callback) const
		{
			for (const del_t& del : mDirs)
			if (!callback(step(pos, del)))
				return;
		};
		virtual pos_t neighbour(const pos_t& pos, const dir_t& direction) const
		{
			return step(pos, mDirs.deltas.delta(direction));
		};

		virtual pos_t step(const pos_t& lhs, const del_t& rhs) const = 0;
	protected:
		const dir_del_set& mDirs;
	};

	template<typename P, typename Dir, typename It = typename griddle::direction_set<Dir>::iterator>
	using lie_topology = step_topology<P, Dir, P, It>;

	template<unsigned int N>
	class cartesian_topology : public lie_topology<ivec,griddle::dirN<N>>
	{
	public:
		cartesian_topology(const dir_del_set& dirs_dels) : lie_topology<ivec, griddle::dirN<N>>(dirs_dels) {}
		virtual pos_t step(const pos_t& lhs, const del_t& rhs) const { return lhs + rhs; }
	protected:
	};

	const cartesian_topology<4> CARDINAL_TOPOLOGY(CARDINAL_DELTAS);
	const cartesian_topology<6> HEX_TOPOLOGY_A(HEX_DELTAS_A);
	const cartesian_topology<6> HEX_TOPOLOGY_B(HEX_DELTAS_B);
	const cartesian_topology<8> ALL_TOPOLOGY(ALL_DELTAS);

	template<unsigned int N>
	class torus_topology : public cartesian_topology<N>
	{
	public:
		torus_topology(const dir_del_set& dirs_dels, int width, int height) : cartesian_topology<N>(dirs_dels), mWidth(width), mHeight(height) {}
		torus_topology(const dir_del_set& dirs_dels, const torus_like& tl) : torus_topology<N>(dirs_dels, tl.width(), tl.height()) {}
		virtual pos_t step(const pos_t& lhs, const del_t& rhs) const
		{
			pos_t r = cartesian_topology::step(lhs, rhs);
			r.x = modulo(r.x, mWidth);
			r.y = modulo(r.y, mHeight);
			return r;
		}
	protected:
		int mWidth;
		int mHeight;
	};


	template<typename P, typename Dir, typename Del, typename Sym, typename It = typename griddle::direction_set<Dir>::iterator>
	class transforming_topology : public step_topology<typename position_facing<position<P>, Sym>, Dir, Del, It>
	{
	public:
		typedef typename Sym sym_t;
		typedef typename position<P> sub_pos_t;
		typedef typename position_facing<sub_pos_t, Sym> pos_sym_t;
		typedef typename std::pair<sub_pos_t, Dir> portal_t;
		typedef typename step_topology<pos_sym_t, Dir, Del, It> base_t;
		//typedef base_t::dir_t dir_t;
		typedef typename step_topology<P, Dir, Del, It> sub_topo_t;
		typedef typename chart<P> chart_t;
		transforming_topology(const sub_topo_t& source_topology, const griddle::direction_delta_set<Dir, Del, It>& dirs)
			: m_source_topology(source_topology), base_t(dirs){}

		virtual pos_t step(const pos_t& lhs, const del_t& rhs) const
		{
			return{ m_source_topology.step(((const pos_sym_t&)lhs).pos, rhs), lhs.face };
		}

		virtual pos_t neighbour(const pos_t& pos, const dir_t& direction) const
		{
			auto newpos_actuallyuse = unusual_neighbour(pos, direction);

			if (newpos_actuallyuse.second)
				return newpos_actuallyuse.first;

			dir_t real_dir = direction + pos.face;
			return step(pos, mDirs.deltas.delta(real_dir));
		}

		virtual void iterate_neighbourhood_stop(pos_t pos, std::function<bool(pos_t)> callback) const
		{
			for (const dir_t& dir : mDirs.dirs)
			if (!callback(neighbour(pos, dir)))
				return;
		};
		const sub_topo_t& m_source_topology;
		virtual std::pair<pos_t, bool> unusual_neighbour(const pos_t& pos, const dir_t& direction) const { return{ pos, false }; };
	protected:

	};


	const std::vector<bool> NO_WEIRD_MIRRORS{ false, false, false, false, false };
	class fundamentally_square_topology : public transforming_topology<ivec, dir8, ivec, sym8>
	{
	public:
		fundamentally_square_topology(int width, int height, const std::vector<int>& edge_maps, const std::vector<bool>& edge_reflections = NO_WEIRD_MIRRORS)
			: mWidth(width), mHeight(height), transforming_topology(ALL_TOPOLOGY, ALL_DELTAS), m_edge_maps(edge_maps), m_edge_reflections(edge_reflections)
		{
			assert(edge_maps.size() == 5);
			assert(edge_reflections.size() == 5);
			for (unsigned int i = 1; i < 5; i++)
			{
				int edge_map = edge_maps[i];
				bool zero_map = (edge_map == 0);
				int absolute_map = abs(edge_map);
				int sign_map = (edge_map < 0)?-1:1;
				if (!zero_map)
				{
					assert(edge_maps[absolute_map] == i * sign_map);
					assert(edge_reflections[absolute_map] == edge_reflections[i]);
				}
			}
			if (abs(edge_maps[1]) == 2 || abs(edge_maps[1]) == 4 || abs(edge_maps[3]) == 2 || abs(edge_maps[3]) == 4)
				assert(mWidth == mHeight);
		}
	protected:
		std::pair<int, int> find_edge_x(int x, int y, int dx, int dy) const
		{
			if (x == 0 && dx == -1)
			{
				if (m_edge_maps[1] != 0)
					return{ 1, y };
			}
			else if (x == mWidth-1 && dx == 1)
			{
				if (m_edge_maps[3] != 0)
					return{ 3, y };
			}
			return{ 0, INT_MAX };
		}

		std::pair<int, int> find_edge_y(int x, int y, int dx, int dy) const
		{
			if (y == 0 && dy == -1)
			{
				if (m_edge_maps[2] != 0)
					return{ 2, x };
			}
			else if (y == mHeight-1 && dy == 1)
			{
				if (m_edge_maps[4] != 0)
					return{ 4, x };
			}
			return{ 0, INT_MAX };
		}

		bool traverse_boundaries(int& x, int& y, int& dx, int& dy, sym_t& face) const
		{
			// find a traversable x edge
			auto edge_coord = find_edge_x(x, y, dx, dy);

			if (edge_coord.first == 0) // there's no traversable x edge
			{
				// find a traversable y edge
				edge_coord = find_edge_y(x, y, dx, dy);
				if (edge_coord.first == 0) // there's no traversable y edge
					return false;
				else // there's a traversable y edge
					dy = 0;
			}
			else // there's a traversable x edge
				dx = 0;

			// find out about the edge traversal
			int target_edge = m_edge_maps[edge_coord.first]; // not zero
			int abs_target_edge = abs(target_edge);
			bool reflect_target_edge = (target_edge < 0);
			bool reflect_perception = (abs_target_edge == edge_coord.first);
			reflect_perception = reflect_perception != m_edge_reflections[edge_coord.first];



			int rotations = modulo(abs_target_edge - edge_coord.first, 4);
			reflect_perception = (reflect_perception != reflect_target_edge);
			reflect_perception = (reflect_perception != (rotations % 2 == 1));
			// transfer coordinates to target edge
			switch (abs_target_edge)
			{
			case 1:
				x = 0; y = edge_coord.second;
				if (reflect_target_edge)
					y = mHeight - y - 1;
				if (reflect_perception)
				{
					face = face * facing::SOUTH_FLIPPED_8;
					if (rotations != 0)
						dy = -dy;
				}
				break;
			case 2:
				y = 0; x = edge_coord.second;
				if (reflect_target_edge)
					x = mWidth - x - 1;
				if (reflect_perception)
				{
					face = face * facing::NORTH_FLIPPED_8;
					if (rotations != 0)
						dx = -dx;
				}
				break;
			case 3:
				x = mWidth-1; y = edge_coord.second;
				if (reflect_target_edge)
					y = mHeight - y - 1;
				if (reflect_perception)
				{
					face = face * facing::SOUTH_FLIPPED_8;
					if (rotations != 0)
						dy = -dy;
				}
				break;
			case 4:
				y = mHeight-1; x = edge_coord.second;
				if (reflect_target_edge)
					x = mWidth - x - 1;
				if (reflect_perception)
				{
					face = face * facing::NORTH_FLIPPED_8;
					if (rotations != 0)
						dx = -dx;
				}
				break;
			}

			// rotate facing so that the reverse portal is in the same direction as the reverse travel direction
			switch (rotations)
			{
			case 0:
				face = face * facing::SOUTH_8;
				break;
			case 1:
				face = face * facing::WEST_8;
				dx = -dx;
				std::swap(dx, dy);
				break;
			case 2:
				break;
			case 3:
				face = face * facing::EAST_8;
				dy = -dy;
				std::swap(dx, dy);
				break;
			}
			return true;
		}

		virtual std::pair<pos_t, bool> unusual_neighbour(const pos_t& pos, const dir_t& direction) const
		{
			if (!in_grid_range(pos.pos, mWidth, mHeight))
				return{ pos, false };
			dir_t real_dir = direction + pos.face;
			del_t del = mDirs.deltas.delta(real_dir);
			pos_t new_pos = pos;
			int& x = new_pos.pos.x;
			int& y = new_pos.pos.y;
			sym_t& face = new_pos.face;
			int& dx = del.x;
			int& dy = del.y;
			
			if (!traverse_boundaries(x, y, dx, dy, face))
				return{ pos, false };

			if (!traverse_boundaries(x, y, dx, dy, face))
			{
				x += dx; dx = 0;
				y += dy; dy = 0;
			}
			if (!traverse_boundaries(x, y, dx, dy, face))
			{
				x += dx; dx = 0;
				y += dy; dy = 0;
			}
			if (!in_grid_range(new_pos.pos, mWidth, mHeight))
				new_pos.pos.invalidate();
			
			return{ new_pos, true };
		};

		const std::vector<int>& m_edge_maps;
		const std::vector<bool>& m_edge_reflections;
		const int mWidth;
		const int mHeight;
	};

	const std::vector<int> BOUNDED_POLYGON{ 0, 0, 0, 0, 0 };
	class transformable_bounded_topology : public fundamentally_square_topology
	{
	public:
		transformable_bounded_topology(int width, int height) :
			fundamentally_square_topology(width, height, BOUNDED_POLYGON ) {}
	};

	const std::vector<int> RING_POLYGON_X{ 0, 3, 0, 1, 0 };
	const std::vector<int> RING_POLYGON_Y{ 0, 0, 4, 0, 2 };
	class ring_topology : public fundamentally_square_topology
	{
	public:
		ring_topology(int width, int height, bool wrap_x=true) : fundamentally_square_topology(width, height,
			wrap_x ? RING_POLYGON_X : RING_POLYGON_Y, NO_WEIRD_MIRRORS) {}
	};


	const std::vector<int> MOEBIUS_POLYGON_X{ 0, -3, 0, -1, 0 };
	const std::vector<int> MOEBIUS_POLYGON_Y{ 0, 0, -4, 0, -2 };
	class moebius_topology : public fundamentally_square_topology
	{
	public:
		moebius_topology(int width, int height, bool wrap_x=true) : fundamentally_square_topology(width, height,
			wrap_x ? MOEBIUS_POLYGON_X : MOEBIUS_POLYGON_Y, NO_WEIRD_MIRRORS) {}
	};

	const std::vector<int> TORUS_POLYGON{ 0, 3, 4, 1, 2 };
	class transformable_torus_topology : public fundamentally_square_topology
	{
	public:
		transformable_torus_topology(int width, int height) : fundamentally_square_topology(width, height,
			TORUS_POLYGON, NO_WEIRD_MIRRORS) {}
	};

	const std::vector<int> KLEIN_POLYGON_X{ 0, 3, -4, 1, -2 };
	const std::vector<int> KLEIN_POLYGON_Y{ 0, -3, 4, -1, 2 };
	class klein_topology : public fundamentally_square_topology
	{
	public:
		klein_topology(int width, int height, bool flip_x = true) : fundamentally_square_topology(width, height,
			flip_x ? KLEIN_POLYGON_X : KLEIN_POLYGON_Y, NO_WEIRD_MIRRORS) {}
	};

	const std::vector<int> RPP_POLYGON{ 0, -3, -4, -1, -2 };
	class real_projective_plane_topology : public fundamentally_square_topology
	{
	public:
		real_projective_plane_topology(int width, int height) : fundamentally_square_topology(width, height,
			RPP_POLYGON, NO_WEIRD_MIRRORS) {}
	};

	const std::vector<int> SPHERE_POLYGON_NW{ 0, 2, 1, 4, 3 };
	const std::vector<int> SPHERE_POLYGON_NE{ 0, -4, -3, -2, -1 };
	class sphere_topology : public fundamentally_square_topology
	{
	public:
		sphere_topology(int width, int height, bool corner_nw = true) : fundamentally_square_topology(width, height,
			corner_nw ? SPHERE_POLYGON_NW : SPHERE_POLYGON_NE, NO_WEIRD_MIRRORS) {}
	};

	const std::vector<int> MIRROR_POLYGON_W{ 0, 1, 0, 0, 0 };
	const std::vector<int> MIRROR_POLYGON_W_E{ 0, 1, 0, 3, 0 };
	const std::vector<int> MIRROR_POLYGON_W_N{ 0, 1, 2, 0, 0 };
	const std::vector<int> MIRROR_POLYGON_W_N_E{ 0, 1, 2, 3, 0 };
	const std::vector<int> FOUR_MIRROR_POLYGON{ 0, 1, 2, 3, 4 };

	template<typename P, typename Dir, typename Del, typename Sym, typename It = typename griddle::direction_set<Dir>::iterator>
	class portal_topology : public transforming_topology<P, Dir, Del, Sym, It>
	{
	public:
		typedef typename Sym sym_t;
		typedef typename position<P> sub_pos_t;
		typedef typename position_facing<sub_pos_t, Sym> pos_sym_t;
		typedef typename std::pair<sub_pos_t, Dir> portal_t;
		typedef typename transforming_topology<P, Dir, Del, Sym, It> base_t;
		typedef typename step_topology<P, Dir, Del, It> sub_topo_t;
		typedef typename chart<P> chart_t;
		portal_topology(const chart_t& source_chart, const base_t& source_topology, const griddle::direction_delta_set<Dir, Del, It>& dirs, sym_t turn_around)
			: m_portals(source_chart), m_source_transforming_topology(source_topology), base_t(source_topology.m_source_topology,dirs), m_turn_around(turn_around) {}

		//virtual pos_t neighbour(const pos_t& pos, const dir_t& direction) const
		//{
		//	dir_t real_dir = direction + pos.face;
		//	if (portal_exists(pos.pos, real_dir))
		//	{
		//		pos_sym_t port = get_portal(pos.pos, real_dir);
		//		sym_t face = pos.face * port.face;
		//		return{ port.pos, face };
		//	}
		//	return step(pos, mDirs.deltas.delta(real_dir));
		//}

		void add_bidirectional_portal(sub_pos_t from, dir_t dir, pos_sym_t to)
		{
			if (!can_add_portal(from, dir, to)) return;
			m_portals(from)[dir] = to;
			portal_t rev = reverse_portal(from, dir, to);
			m_portals(rev.first)[rev.second] = { from, to.face * m_turn_around }; // this needs to be to.face * facing::SOUTH_8
		}
		bool can_add_portal(sub_pos_t from, dir_t dir, pos_sym_t to) const
		{
			if (portal_exists(from, dir)) return false;
			portal_t rev = reverse_portal(from, dir, to);
			return !portal_exists(rev.first, rev.second);
		}

		bool portal_exists(const sub_pos_t& from, const dir_t& dir) const
		{
			if (!m_portals.get_chart().in_range(from))
				return false;
			return m_portals(from).find(dir) != m_portals(from).end();
		}
		virtual std::pair<pos_t, bool> unusual_neighbour(const pos_t& pos, const dir_t& direction) const
		{
			auto newpos_actuallyuse = m_source_transforming_topology.unusual_neighbour(pos, direction);
			if (newpos_actuallyuse.second)
				return newpos_actuallyuse;
			if (!m_portals.get_chart().in_range(pos.pos))
				return{ pos, false };
			auto f = m_portals(pos.pos).find(direction);
			if (f == m_portals(pos.pos).end())
				return{ pos, false };
			pos_sym_t port = (*f).second;
			sym_t face = pos.face * (*f).second.face.inverse();
			return{ { (*f).second.pos,face }, true };
		};

	protected:
		portal_t reverse_portal(sub_pos_t from, dir_t dir, pos_sym_t to) const
		{
			return{ to.pos, (dir + to.face).reverse() };
		}
		//const pos_sym_t& get_portal(const sub_pos_t& from, const dir_t& dir) const
		//{
		//	return m_portals(from).at(dir);
		//}
		const base_t& m_source_transforming_topology;
		container<P, std::map<dir_t, pos_sym_t>> m_portals;
		const sym_t m_turn_around;

	};
}

//
//namespace dsif
//{
//	//class digraph_topology : public topology<size_t, size_t>
//	//{
//	//public:
//	//	digraph_topology(const std::vector<std::vector<size_t>>& source) : mMap(source) {}
//	//	//digraph_topology(const std::vector<std::vector<std::pair<size_t,size_t>>>& source) : mMap(source) {}
//
//	//	virtual void iterate_neighbourhood_stop(pos_t pos, std::function<bool(pos_t)> callback) const = 0;
//	//	virtual pos_t neighbour(pos_t pos, dir_t direction) const { return mMap[pos.pos][direction]; }
//	//protected:
//	//	const std::vector<std::vector<size_t>> mMap;
//	//};
//
//	template<typename P>
//	class union_topology : public step_topology<union_position<P>>
//	{
//	public:
//		union_topology(std::vector<const step_topology<P>*> components, dir_vector dirs) : step_topology(dirs), mComponentTopologies(components) {}
//		virtual pos_t step(const pos_t& lhs, dir_t rhs) const
//		{
//			return union_position<P>(mComponentTopologies[lhs.get_component()]->step(lhs.pos,rhs), lhs.get_component());
//		}
//	protected:
//		const std::vector<const step_topology<P>*> mComponentTopologies;
//	};
//
//}
//
