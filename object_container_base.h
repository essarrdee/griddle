#pragma once
#include <memory>
#include "container.h"

namespace griddle
{
	template<typename P, typename T>
	class object_container_base// : protected container<P, std::shared_ptr<T>>
	{
	public:
		typedef std::shared_ptr<T> s_ptr;
		typedef typename container<P, s_ptr>::pos_t pos_t;

		//object_container_base(const chart& c) : container(c){}

		virtual void claim(s_ptr& p) = 0;
		virtual void release(s_ptr& p) = 0;
		virtual void place(s_ptr& p, pos_t pos) = 0;
		virtual void displace(s_ptr& p) = 0;
		virtual void move(s_ptr from, pos_t pos) = 0;

		virtual bool claim_possible(const s_ptr& p) const = 0;
		virtual bool release_possible(const s_ptr& p) const = 0;
		virtual bool place_possible(const s_ptr& p, pos_t pos, bool after_displace = false) const = 0;
		virtual bool displace_possible(const s_ptr& p) const = 0;
		virtual bool move_possible(const s_ptr& from, pos_t pos) const = 0;

		virtual bool contains(pos_t pos, const s_ptr& p) const = 0;
		virtual bool full(pos_t pos) const = 0;
		virtual bool has_lock(const s_ptr& p) const = 0;
	};
}