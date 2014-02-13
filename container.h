#pragma once
#include <vector>
#include "chart.h"
namespace griddle
{
	template<typename P, typename T>
	class container
	{
	public:
		typedef typename chart<P> chart;
		typedef typename chart::pos_t pos_t;
		typedef typename std::vector<T> vector;
		typedef typename vector::iterator iterator;
		typedef typename vector::const_iterator const_iterator;
		typedef typename vector::reverse_iterator reverse_iterator;
		typedef typename vector::const_reverse_iterator const_reverse_iterator;

		container(const chart& c, bool resize=true) : mChart(c) { if(resize) mContents.resize(mChart.size()); }
		const chart& get_chart() const { return mChart; }
		T& operator()(pos_t pos) { return mContents[mChart.index(pos)]; }
		const T& operator()(pos_t pos) const { return mContents[mChart.index(pos)]; }
		//T& operator()(P pos) { return mContents[mChart.index(pos_t(pos))]; }
		//const T& operator()(P pos) const { return mContents[mChart.index(pos_t(pos))]; }

		iterator begin() { return mContents.begin(); }
		iterator end() { return mContents.end(); }
		const_iterator cbegin() const { return mContents.cbegin(); }
		const_iterator cend() const { return mContents.cend(); }

		iterator rbegin() { return mContents.rbegin(); }
		iterator rend() { return mContents.rend(); }
		const_reverse_iterator crbegin() const { return mContents.crbegin(); }
		const_reverse_iterator crend() const { return mContents.crend(); }

	protected:
		const chart& mChart;
		vector mContents;
	};
	template<typename P>
	class range : public container<P,P>
	{
	public:
		range(const chart& c) : container(c,false)
		{
			c.iterate([&](const pos_t& pos)
			{
				mContents.push_back(pos);
				return true;
			}
			);
		}
		size_t chart_inverse(pos_t pos) const { return mContents[pos]; }
		const_iterator begin() const { return mContents.cbegin(); }
		const_iterator end() const { return mContents.cend(); }

		const_iterator rbegin() const { return mContents.crbegin(); }
		const_iterator rend() const { return mContents.crend(); }
	};
}
