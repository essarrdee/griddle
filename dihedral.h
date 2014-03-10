#pragma once
#include <vector>
#include "vec.h"
#include "position.h"
#include "directions.h"

namespace griddle
{

	//template<typename P, typename Sym>
	//class position_facing
	//{
	//public:
	//	typedef typename P pos_t;
	//	typedef typename Sym sym_t;
	//	position_facing(pos_t pos_, sym_t sym) : pos(pos_), face(sym){}
	//	position_facing(pos_t pos) : pos(pos), face(){}
	//	position_facing() : pos(), face(){}
	//	//pos_t get_position() { return position; }
	//	pos_t pos;
	//	sym_t face;
	//};

	template<unsigned int N>
	class symN // specifies a member of the dihedral group with 2N elements. Normally N=8.
		// see http://groupprops.subwiki.org/wiki/Dihedral_group:D16
	{
	public:
		bool reflection; // true if the symmetry is a reflection of the form xa^(rotation)
		int rotation; // the symmetry is of the form a^(rotation) or xa^(rotation)
		symN(int rot, bool refl) : reflection(refl), rotation(modulo(rot, (int)N)) {}
		symN(int rot) : symN(rot, false) {}
		symN(bool refl) : symN(0, refl) {}
		symN() : symN(0, false) {}
		inline bool valid() const { return rotation >= 0 && rotation < (int)N; }
		symN flip() const { return{ rotation, !reflection }; }
		symN inverse() const { return reflection ? symN(rotation, reflection) : symN(-rotation, reflection); }
	};

	template<unsigned int N>
	bool operator==(const symN<N>& lhs, const symN<N>& rhs)
	{
		return rhs.reflection == lhs.reflection  && rhs.rotation == lhs.rotation;
	}
	template<unsigned int N>
	bool operator<(const symN<N>& lhs, const symN<N>& rhs)
	{
		if ((lhs.reflection ? 1 : 0) < (rhs.reflection ? 1 : 0))
			return true;
		if ((lhs.reflection ? 1 : 0) > (rhs.reflection ? 1 : 0))
			return false;
		return lhs.rotation < rhs.rotation;
	}

	template<unsigned int N>
	bool operator!=(const symN<N>& lhs, const symN<N>& rhs)
	{
		return !(lhs == rhs);
	}

	template<unsigned int N>
	symN<N> operator*(const symN<N>& lhs, const symN<N>& rhs)
	{
		return symN<N>(
			(rhs.reflection ? -lhs.rotation : lhs.rotation) + rhs.rotation,
			lhs.reflection != rhs.reflection);
	}
	template<unsigned int N>
	dirN<N> operator+(const dirN<N>& lhs, const symN<N>& rhs)
	{
		return (rhs.reflection ? -lhs.dir : lhs.dir) + rhs.rotation;
	}

	typedef symN<4> sym4;
	typedef symN<6> sym6;
	typedef symN<8> sym8;
	namespace facing
	{
		static const sym4 NORTH_4(0, false);
		static const sym4 EAST_4(1, false);
		static const sym4 SOUTH_4(2, false);
		static const sym4 WEST_4(3, false);

		static const sym8 NORTH_8(0, false);
		static const sym8 NORTH_EAST_8(1, false);
		static const sym8 EAST_8(2, false);
		static const sym8 SOUTH_EAST_8(3, false);
		static const sym8 SOUTH_8(4, false);
		static const sym8 SOUTH_WEST_8(5, false);
		static const sym8 WEST_8(6, false);
		static const sym8 NORTH_WEST_8(7, false);

		static const sym4 NORTH_FLIPPED_4(0, true);
		static const sym4 EAST_FLIPPED_4(1, true);
		static const sym4 SOUTH_FLIPPED_4(2, true);
		static const sym4 WEST_FLIPPED_4(3, true);

		static const sym8 NORTH_FLIPPED_8(0, true);
		static const sym8 NORTH_EAST_FLIPPED_8(1, true);
		static const sym8 EAST_FLIPPED_8(2, true);
		static const sym8 SOUTH_EAST_FLIPPED_8(3, true);
		static const sym8 SOUTH_FLIPPED_8(4, true);
		static const sym8 SOUTH_WEST_FLIPPED_8(5, true);
		static const sym8 WEST_FLIPPED_8(6, true);
		static const sym8 NORTH_WEST_FLIPPED_8(7, true);
	}
}
