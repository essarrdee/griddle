#pragma once
#include <queue>
#include "topology.h"
#include "container.h"

namespace griddle
{
	template<typename T>
	T IDENTITY(const T& t) { return t; }

	template<typename P, typename D, typename TP>
	class distance_field
	{
	public:
		typedef typename chart<P>::pos_t pos_t;
		typedef typename griddle::topology<TP, D>::pos_t topo_pos_t;
		distance_field(const topology<TP, D>& base_topology, const chart<P>& base_chart)
			: mBaseTopology(base_topology), mBaseChart(base_chart),
			distances(base_chart), previous(base_chart)
		{
			zero();
		}
		void zero()
		{
			for (auto& d : distances)
				d = SIZE_MAX;
			for (auto& p : previous)
				p = pos_t();
		}
		void initialise(pos_t p)
		{
			if (!mBaseChart.in_range(p))
				return;
			boundary.push(p);
			distances(p) = 0;
		}
		void initialise(std::initializer_list<pos_t> ps)
		{
			for (const auto p : ps)
			{
				if (!mBaseChart.in_range(p))
					continue;
				boundary.push(p);
				distances(p) = 0;
			}
		}
		void update(std::function<TP(P)> ChartToTopo = IDENTITY<P>, std::function<P(TP)> TopoToChart = IDENTITY<P>)
		{
			while (!boundary.empty())
			{
				pos_t current = boundary.front();
				boundary.pop();
				size_t dist;
				if (mBaseChart.in_range(current))
				{
					dist = distances(current);
				}
				else
				{
					continue;
				}
				mBaseTopology.iterate_neighbourhood(ChartToTopo(current),
					[&](topo_pos_t topo_pos)
				{
					pos_t pos(TopoToChart(topo_pos));
					if (mBaseChart.in_range(pos) && dist + 1 < distances(pos))
					{
						distances(pos) = dist + 1;
						previous(pos) = current;
						boundary.push(pos);
					}
				});
			}
		}
		const container<P, size_t>& get_distances() const { return distances; }
		const container<P, pos_t>& get_previous() const { return previous; }
	protected:
		const topology<TP, D>& mBaseTopology;
		const chart<P>& mBaseChart;
		container<P, size_t> distances;
		container<P, pos_t> previous;
		std::queue<pos_t> boundary;
	};
}