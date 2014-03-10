#pragma once
//#include "vec.h"
namespace griddle
{
	template<typename P>
	class position : public P
	{
	public:
		typedef P base_t;
		using P::P;
		position() : base_t(), mNowhere(true){}
		position(base_t pos) : base_t(pos){}
		void invalidate() { mNowhere = true; }
		inline bool nowhere() const { return mNowhere; }
		inline operator bool() const { return !nowhere(); }
		virtual bool operator==(const position<P>& rhs) const
		{
			if (nowhere() || rhs.nowhere()) return false;
			return (const base_t&)(*this) == (const base_t&)rhs;
		}
		inline bool operator!=(const position<P>& rhs) const { return !((*this) == rhs); }
		virtual bool operator<(const position<P>& rhs) const
		{
			return !nowhere() &&
				(const base_t&)(*this) < (const base_t&)rhs;
		}
		//const P& base_ref() const { return (const P&)(*this); }
	private:
		bool mNowhere{ false };
	};
	template<typename P>
	inline static position<P> operator+(const position<P>& lhs, const position<P>& rhs)
	{
		if (lhs.nowhere() || rhs.nowhere())
			return position<P>();
		return (P)lhs + (P)rhs;
	}






	//typedef pposition<vec<int,INT_MAX>> ppivec;

	//template<typename P>
	//class position
	//{
	//public:
	//	typedef P base_t;
	//	position() : pos(), mNowhere(true){}
	//	position(P init) : pos(init), mNowhere(false){}
	//	void invalidate() { mNowhere = true; }
	//	inline bool nowhere() const { return mNowhere; }
	//	inline bool operator==(const position& rhs) const
	//	{
	//		if (nowhere() || rhs.nowhere()) return false;
	//		return pos == rhs.pos;
	//	}
	//	inline bool operator!=(const position<P>& rhs) const { return !((*this) == rhs); }
	//	inline bool operator<(const position<P>& rhs) const { return !nowhere() && pos < rhs.pos; }
	//	P pos;
	//private:
	//	bool mNowhere;
	//};

	//template<typename P>
	//inline static position<P> operator+(const position<P>& lhs, const position<P>& rhs)
	//{
	//	if (lhs.nowhere() || rhs.nowhere())
	//		return position<P>();
	//	return position<P>(lhs.pos + rhs.pos);
	//}

	//template<typename P>
	//class union_position
	//{
	//public:
	//	typedef P base_t;
	//	typedef position<base_t> pos_t;
	//	union_position() {}
	//	union_position(pos_t pos_, size_t component) : pos(pos_), mComponent(component) {}
	//	union_position(P pos_, size_t component) : pos(pos_), mComponent(component) {}
	//	pos_t pos;
	//	size_t get_component() const { return mComponent; }
	//	inline bool operator<(const union_position<P>& rhs) const { return !pos.nowhere() && get_component() < rhs.get_component(); }
	//protected:
	//	size_t mComponent;
	//};

	//template<typename P>
	//class union_position
	//{
	//public:
	//	typedef P base_t;
	//	typedef position<base_t> pos_t;
	//	union_position() {}
	//	union_position(pos_t pos_, size_t component) : pos(pos_), mComponent(component) {}
	//	pos_t pos;
	//	size_t get_component() const { return mComponent; }
	//	inline bool operator<(const union_position<P>& rhs) const { return !pos.nowhere() && get_component() < rhs.get_component(); }
	//protected:
	//	size_t mComponent;
	//};
}