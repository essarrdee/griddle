#pragma once
#include <set>
#include "mover.h"
#include "position.h"
#include "object_container_base.h"

namespace griddle
{
	template<typename P, typename T, typename MOVER_TYPE>
	class mover_container : public object_container_base<P, T>, public MOVER_TYPE
	{
	public:
		typedef typename MOVER_TYPE::pos_t pos_t;
		//mover_container(const chart& c) : object_container_base(c){}
		virtual void claim(s_ptr& p) { if (claim_possible(p)) lockL(p); }
		virtual void release(s_ptr& p) { if (release_possible(p)) unlockL(p); }

		virtual bool claim_possible(const s_ptr& p) const { return p->unlocked() || has_lock(p); }
		virtual bool release_possible(const s_ptr& p) const { return claim_possible(p) && !p->placed(); }

		virtual bool has_lock(const s_ptr& p) const { return MOVER_TYPE::has_lock(p); }
	};

	template<typename P, typename T>
	using raw_container = mover_container<P, T, mover<P,T>>;
	
	template<typename P, typename T>
	using cross_container = mover_container<P, T, cross_mover<P,T>>;
	
	template<typename P, typename T, typename MOVER_CONTAINER_TYPE>
	class unique_container : public MOVER_CONTAINER_TYPE, protected container<P, typename MOVER_CONTAINER_TYPE::s_ptr>
	{
	public:
		typedef typename MOVER_CONTAINER_TYPE::s_ptr s_ptr;
		typedef typename MOVER_CONTAINER_TYPE::pos_t pos_t;
		unique_container(const chart& c) : container(c){}

		virtual void place(s_ptr& p, pos_t pos) { if (place_possible(p, pos)) { (*this)(pos) = p; moveL(p, pos); } }
		virtual void displace(s_ptr& p) { if (displace_possible(p)) { (*this)(p->get_location()) = s_ptr(); moveL(p, pos_t()); } }
		virtual void move(s_ptr& from, pos_t to) { if (move_possible(from, to)) { displace(from); place(from, to); } }
	
		virtual s_ptr displace(pos_t from) { s_ptr& p = (*this)(from); displace(p); return p; }
		virtual void move(pos_t from, pos_t to) { move((*this)(from), to); }
	
		virtual bool place_possible(const s_ptr& p, pos_t pos, bool after_displace = false) const
		{
			if (!mChart.in_range(pos))
				return false;
			if (!has_lock(p))
				return false;
			if (contains(pos, p))
				return true;
			if (p->placed() && !after_displace)
				return false;
			if (full(pos))
				return false;
			return true;
		}
		virtual bool displace_possible(const s_ptr& p) const { return claim_possible(p); }
		virtual bool move_possible(const s_ptr& from, pos_t to) const { return displace_possible(from) && place_possible(from, to, true); }
	
		virtual bool contains(pos_t pos, const s_ptr& p) const { return mChart.in_range(pos) && (*this)(pos) == p; }
		virtual bool full(pos_t pos) const { return mChart.in_range(pos) && (bool)(*this)(pos); }
	protected:
	
	};
	
	template<typename P, typename T, typename MOVER_CONTAINER_TYPE>
	class pile_container : public MOVER_CONTAINER_TYPE, protected container<P, std::set<typename MOVER_CONTAINER_TYPE::s_ptr>>
	{
	public:
		pile_container(const chart& c, unsigned int capacity) : container(c), mCapacity(capacity){}
		typedef typename MOVER_CONTAINER_TYPE::s_ptr s_ptr;
		typedef typename MOVER_CONTAINER_TYPE::pos_t pos_t;
		virtual void place(s_ptr& p, pos_t pos)
		{
			if (place_possible(p, pos))
			{
				(*this)(pos).insert(p);
				moveL(p, pos);
			}
		}
		virtual void displace(s_ptr& p) { if (displace_possible(p)) (*this)(p->get_location()).erase(p); moveL(p, pos_t()); }
		virtual void move(s_ptr& from, pos_t to) { if (move_possible(from, to)) { displace(from); place(from, to); } }
	
		virtual bool place_possible(const s_ptr& p, pos_t pos, bool after_displace = false) const
		{
			if (!mChart.in_range(pos))
				return false;
			if (!has_lock(p))
				return false;
			if (contains(pos, p))
				return true;
			if (p->placed() && !after_displace)
				return false;
			if (full(pos))
				return false;
			return true;
		}
		virtual bool displace_possible(const s_ptr& p) const { return claim_possible(p); }
		virtual bool move_possible(const s_ptr& from, pos_t to) const { return displace_possible(from) && place_possible(from, to, true); }
	
		virtual bool contains(pos_t pos, const s_ptr& p) const { return mChart.in_range(pos) && (*this)(pos).find(p) != (*this)(pos).end(); }
		virtual bool full(pos_t pos) const{ return !mChart.in_range(pos) || (*this)(pos).size() >= mCapacity; }
		unsigned int get_capacity() { return mCapacity; }
	protected:
		const unsigned int mCapacity;
	};
	
	template<typename P, typename T>
	using raw_unique_container = unique_container<P, T, raw_container<P,T>>;
	
	template<typename P, typename T>
	using cross_unique_container = unique_container<P, T, cross_container<P, T>>;
	
	template<typename P, typename T>
	using raw_pile_container = pile_container<P, T, raw_container<P,T>>;
	
	template<typename P, typename T>
	using cross_pile_container = pile_container<P, T, cross_container<P,T>>;
	
	
}