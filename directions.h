#pragma once
#include <vector>
#include "vec.h"
#include "position.h"

namespace griddle
{
	//template<typename Dir, typename Tf>
	//class transformed_direction
	//{
	//public:
	//	transformed_direction(Dir dir) : mDir(dir), Tf() {}
	//	transformed_direction() : transformed_direction(Dir()) {}
	//	virtual Dir get_dir() const = 0;
	//	void set_dir(const Dir& dir) { mDir = dir; }
	//	void set_transformation(const Tf& transformation) { mTransformation = transformation; }
	//protected:
	//	Dir mDir;
	//	Tf mTransformation;
	//};
	//template<typename Dir, typename Tf>
	//transformed_direction<Dir, Tf> operator+(const transformed_direction<Dir, Tf>& dir, const Tf& transformation)
	//{
	//	transformed_direction td(dir);
	//	td.set_dir(td.get_dir() + transformation);
	//	return td;
	//}
	//template<typename Dir, typename Tf>
	//transformed_direction<Dir, Tf>& operator+=(const transformed_direction<Dir, Tf>& dir, const Tf& transformation)
	//{
	//	dir = dir+transformation;
	//	return dir;
	//}

	template<typename P,typename Sym>
	class position_facing
	{
	public:
		typedef typename P pos_t;
		typedef typename Sym sym_t;
		position_facing(pos_t pos_, sym_t sym) : pos(pos_), face(sym){}
		position_facing(pos_t pos) : pos(pos), face(){}
		position_facing() : pos(), face(){}
		//pos_t get_position() { return position; }
		pos_t pos;
		sym_t face;
	};


	template<typename Dir, typename It = typename std::vector<Dir>::const_iterator>
	class direction_set
	{
	public:
		typedef typename It iterator;
		virtual iterator begin() const = 0;
		virtual iterator end() const = 0;
	};

	template<typename Del, typename Dir>
	class delta_map
	{
	public:
		virtual Del delta(Dir direction) const = 0;
		bool operator==(const delta_map<Del, Dir>& rhs) const { return this == &rhs; }
	protected:

	};

	template<typename Dir, typename Del, typename It = typename std::vector<Dir>::const_iterator>
	class direction_delta_set
	{
	public:
		typedef typename direction_set<Dir, It> dirs_t;
		typedef typename delta_map<Del, Dir> deltas_t;
		const dirs_t& dirs;
		const deltas_t& deltas;
		direction_delta_set(const dirs_t& dirs_, const deltas_t& deltas_)
			: dirs(dirs_), deltas(deltas_) {}
		class iterator : protected dirs_t::iterator
		{
		public:
			typedef typename dirs_t::iterator base_t;
			iterator(const deltas_t& deltas, base_t it)
				: mDeltas(deltas), base_t(it) {}
			Del operator*(){ return mDeltas.delta(base_t::operator*()); }
			using base_t::operator++;
			bool operator==(const iterator& rhs)
			{ return mDeltas == rhs.mDeltas && base_t::operator==(rhs); }
			bool operator!=(const iterator& rhs) { return !(*this == rhs); }
		protected:
			const deltas_t& mDeltas;
		};
		iterator begin() const { return iterator(deltas, dirs.begin()); }
		iterator end() const{ return iterator(deltas, dirs.end()); }

	};

	template<unsigned int N>
	class symN // specifies a member of the dihedral group with 2N elements. Normally N=8.
		// see http://groupprops.subwiki.org/wiki/Dihedral_group:D16
	{
	public:
		bool reflection; // true if the symmetry is a reflection of the form xa^(rotation)
		int rotation; // the symmetry is of the form a^(rotation) or xa^(rotation)
		symN(int rot, bool refl) : reflection(refl), rotation(modulo(rot,(int)N)) {}
		symN(int rot) : symN(rot, false) {}
		symN(bool refl) : symN(0, refl) {}
		symN() : symN(0, false) {}
		inline bool valid() const { return rotation >= 0 && rotation < (int)N; }
		symN flip() const { return{ rotation, !reflection }; }
		symN inverse() const { return reflection ? symN(rotation, reflection) : symN(-rotation, reflection); }
	};

	template<unsigned int N>
	symN<N> operator*(const symN<N>& lhs, const symN<N>& rhs)
	{
		return symN<N>(
			(rhs.reflection? -lhs.rotation : lhs.rotation) + rhs.rotation,
			lhs.reflection != rhs.reflection);
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

	template<unsigned int N>
	class dirN
	{
	public:
		typedef typename symN<N> sym_t;
		int dir;
		dirN(int d = 0) : dir(modulo( d, (int)N)) {}
		dirN flip() const { return (int)N - dir; }
		dirN reverse() const { return (int)(N/2) + dir; }

		bool operator==(const dirN& rhs) const { return dir == rhs.dir; }
		bool operator<(const dirN& rhs) const { return dir < rhs.dir; }

		dirN operator+(int steps) const { return dir + steps }
		dirN operator-(int steps) const { return dir + (-steps); }
		dirN operator+(sym_t transformation) const { return (transformation.reflection ? -dir : dir) + transformation.rotation; }

		dirN& operator+=(int steps) { *this = *this + steps; return *this; }
		dirN& operator-=(int steps) { *this = *this - steps; return *this; }
		dirN& operator+=(sym_t transformation) { *this = *this + transformation; return *this; }
	};

	typedef dirN<4> dir4;
	typedef dirN<6> dir6;
	typedef dirN<8> dir8;

	namespace walking
	{
		static const dir4 FORWARD_4(0);
		static const dir4 RIGHT_4(1);
		static const dir4 BACK_4(2);
		static const dir4 LEFT_4(3);

		static const dir8 FORWARD_8(0);
		static const dir8 FORWARD_RIGHT_8(1);
		static const dir8 RIGHT_8(2);
		static const dir8 BACK_RIGHT_8(3);
		static const dir8 BACK_8(4);
		static const dir8 BACK_LEFT_8(5);
		static const dir8 LEFT_8(6);
		static const dir8 FORWARD_LEFT_8(7);
	}

	template<unsigned int N>
	dirN<N> get_dirN(int x, int y);

	template<>
	dir8 get_dirN<8>(int x, int y)
	{
		switch (y)
		{
		case -1:
			switch (x)
			{
			case -1: return walking::FORWARD_LEFT_8;
			case 0: return walking::FORWARD_8;
			case 1: return walking::FORWARD_RIGHT_8;
			default: return SIZE_MAX;
			}
		case 0:
			switch (x)
			{
			case -1: return walking::LEFT_8;
			case 0: return SIZE_MAX;
			case 1: return walking::RIGHT_8;
			default: return SIZE_MAX;
			}
		case 1:
			switch (x)
			{
			case -1: return walking::BACK_LEFT_8;
			case 0: return walking::BACK_8;
			case 1: return walking::BACK_RIGHT_8;
			default: return SIZE_MAX;
			}
		default:
			return SIZE_MAX;
		}
	}

	template<>
	dir4 get_dirN<4>(int x, int y)
	{
		if (x == -1 && y == 0)
			return walking::FORWARD_4;
		if (x == 1 && y == 0)
			return walking::BACK_4;
		if (x == 0 && y == -1)
			return walking::LEFT_4;
		if (x == 0 && y == 1)
			return walking::RIGHT_4;
		return SIZE_MAX;
	}

	template<unsigned int N>
	class all_dirsN :virtual public direction_set<dirN<N>>
	{
	public:
		typedef typename direction_set<dirN<N>> base_t;
		typedef typename base_t::iterator iterator;
		typedef typename dirN<N> dir_t;
		typedef typename std::vector<dir_t> vector_t;
		virtual iterator begin() const { return mDirs.cbegin(); }
		virtual iterator end() const { return mDirs.cend(); }
		all_dirsN() : mDirs(build()) {}
	protected:
		static vector_t build()
		{
			vector_t r;
			for (int i = 0; i < N; i++)
				r.push_back(i);
			return r;
		}
		const vector_t mDirs;
	};

	template<unsigned int N>
	class all_deltasN : public delta_map<ivec, typename dirN<N>>
	{
	public:
		typedef typename ivec del_t;
		typedef typename dirN<N> dir_t;
		virtual del_t delta(dir_t dir) const { return mDeltas[dir.dir]; }
		all_deltasN(std::vector<del_t> deltas) : mDeltas(deltas)
		{ assert(mDeltas.size() == N); }
	protected:
		const std::vector<del_t> mDeltas;
	};


	typedef all_dirsN<4> all_dirs4;
	typedef all_dirsN<6> all_dirs6;
	typedef all_dirsN<8> all_dirs8;

	static const all_dirs4 DIRS_4;
	static const all_dirs6 DIRS_6;
	static const all_dirs8 DIRS_8;

	typedef all_deltasN<4> all_deltas4;
	typedef all_deltasN<6> all_deltas6;
	typedef all_deltasN<8> all_deltas8;

	static const all_deltas4 CARDINAL_DIRECTIONS({ { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } });
	static const all_deltas6 HEX_DIRECTIONS_A({ { 0, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 0 }, { -1, -1 } });
	static const all_deltas6 HEX_DIRECTIONS_B({ { 0, -1 }, { 1, -1 }, { 1, 0 }, { 0, 1 }, { -1, 1 }, { -1, 0 } });
	static const all_deltas8 ALL_DIRECTIONS({ { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 } });

	template<unsigned int N>
	using standard_direction_deltasN = direction_delta_set<dirN<N>, ivec>;

	typedef standard_direction_deltasN<4> standard_direction_deltas4;
	typedef standard_direction_deltasN<6> standard_direction_deltas6;
	typedef standard_direction_deltasN<8> standard_direction_deltas8;

	static const standard_direction_deltas4 CARDINAL_DELTAS(DIRS_4, CARDINAL_DIRECTIONS);
	static const standard_direction_deltas6 HEX_DELTAS_A(DIRS_6, HEX_DIRECTIONS_A);
	static const standard_direction_deltas6 HEX_DELTAS_B(DIRS_6, HEX_DIRECTIONS_B);
	static const standard_direction_deltas8 ALL_DELTAS(DIRS_8, ALL_DIRECTIONS);

	// augmented direction needs to take on additional values like rest, up, down, even actions
	// direction_set of augmented direction needs to take on all these values

	//template<typename Dir, typename Aug>
	//class augmented_direction
	//{
	//public:
	//	augmented_direction(Dir direction, Aug augment) : dir(direction), aug(augment){}
	//	Dir dir;
	//	Aug aug;
	//};

}
