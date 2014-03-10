#pragma once
#include<functional>
#include "dihedral.h"
#include "utility.h"
namespace griddle
{
	template<unsigned int N>
	class dihedral_subgroup
	{
	public:
		typedef symN<N> sym_t;

		dihedral_subgroup() {}
		dihedral_subgroup(sym_t generator)
		{
			closure(generator);
		}
		dihedral_subgroup(std::initializer_list<sym_t> generators)
		{
			for (auto g : generators)
			{
				closure(g);
			}
		}
		class iterator
		{
		public:
			iterator(dihedral_subgroup<N> subgroup, bool finished=false) : sym_(), group_(subgroup), finished_(finished){}
			const iterator& operator++()
			{
				sym_ = sym_ * group_.example_rotation();
				if (sym_ == sym_t())
				{
					if (group_.has_reflection)
						sym_ = group_.example_reflection();
					else
						finished_ = true;
				}
				else if (sym_ == group_.example_reflection())
				{
					finished_ = true;
				}
				return *this;
			}
			iterator operator++(int) { sym_t copy = sym_; operator++(); return copy(); };
			bool operator==(const iterator& rhs) const
			{
				if (finished_ && rhs.finished_)
					return true;
				else if (finished_ != rhs.finished_)
					return false;
				return sym_ == rhs.sym_;
			} // don't check subgroup equality?
			sym_t operator*() { return sym_; }
			bool operator!=(const iterator& rhs) const { return !(operator==(rhs)); }
		private:
			sym_t sym_;
			dihedral_subgroup<N> group_;
			bool finished_;
		};
		iterator begin() const { return iterator(*this); }
		iterator end() const { return iterator(*this,true); }
		bool contains(sym_t element) const
		{
			if (element.reflection)
			{
				if (has_reflection)
				{
					auto reflected_element = element * example_reflection();
					return reflected_element.rotation % smallest_rotation == 0;
				}
				else
					return false;
			}
			return element.rotation % smallest_rotation == 0;
		}
		bool orbits_equal(dirN<N> dir1, dirN<N> dir2) const
		{
			return minimal_orbit_generator(dir1).isomorphic(minimal_orbit_generator(dir2), true);
		}

		bool is_trivial() const
		{
			return !has_reflection && smallest_rotation == N;
		};
		bool is_full() const
		{
			return has_reflection && smallest_rotation == 1;
		};
		int size() const
		{
			return N * (has_reflection ? 2 : 1) / smallest_rotation;
		}
		sym_t example_reflection() const { return{ smallest_reflection, true }; }
		sym_t example_rotation() const { return{ smallest_rotation, false }; }

		dihedral_subgroup<N> minimal_orbit_generator(dirN<N> dir) const
		{
			dihedral_subgroup<N> stab = subgroup_stabiliser(dir);
			if (stab.is_trivial())
				return *this;
			return dihedral_subgroup<N>(example_rotation());
		}

		dihedral_subgroup<N> subgroup_stabiliser(dirN<N> dir) const
		{
			auto stab = stabiliser(dir);
			return intersection(stab);
		}
		// left and right cosets? quotient groups? TODO
		dihedral_subgroup<N> intersection(dihedral_subgroup<N> rhs) const
		{
			if (has_reflection && rhs.has_reflection)
			{
				int x = 0, y = 0;
				int d = egcd(smallest_rotation, rhs.smallest_rotation, x, y);
				int lcm = smallest_rotation * rhs.smallest_rotation / d;
				int difference = smallest_reflection - rhs.smallest_reflection;
				sym_t base_rotation(lcm);
				if ((difference % d) != 0)
				{
					return dihedral_subgroup<N>(base_rotation);
				}
				else
				{
					int c = difference / d;
					sym_t base_reflection(smallest_reflection + c *x*smallest_rotation, true);
					return dihedral_subgroup<N>({ base_rotation, base_reflection });
				}
			}
			else
			{
				int d = gcd(smallest_rotation, rhs.smallest_rotation);
				int lcm = smallest_rotation * rhs.smallest_rotation / d;
				sym_t base_rotation(lcm);
				return dihedral_subgroup<N>(base_rotation);
			}
		}

		void closure(sym_t new_element)
		{
			if (is_full()) return;
			if (contains(new_element)) return;
			if (new_element.reflection)
			{
				if (has_reflection)
				{
					sym_t new_rotation = new_element * example_reflection();
					smallest_rotation = gcd(smallest_rotation, new_rotation.rotation);
				}
				else
				{
					has_reflection = true;
					smallest_reflection = new_element.rotation;
				}
			}
			else
			{
				smallest_rotation = gcd(smallest_rotation, new_element.rotation);
			}
			normalise();
		}

		void closure(dihedral_subgroup<N> new_elements)
		{
			closure(new_elements.example_rotation());
			closure(new_elements.example_reflection());
		}
		bool isomorphic(dihedral_subgroup<N> other)
		{ return isomorphic_(other, false); }
		bool equal(dihedral_subgroup<N> other)
		{ return isomorphic_(other, true); }
		bool has_isomorphic_subgroup(dihedral_subgroup<N> other)
		{ return has_isomorphic_subgroup_(other, false); }
		bool has_subgroup(dihedral_subgroup<N> other)
		{ return has_isomorphic_subgroup_(other, true); }
	private:
		bool has_reflection{ false };
		int smallest_rotation{ N };
		int smallest_reflection{ N };
		void normalise()
		{
			smallest_rotation = modulo(smallest_rotation, (int)N);
			if (smallest_rotation == 0)
				smallest_rotation = N;
			smallest_reflection = smallest_reflection % smallest_rotation;
		}
		bool has_isomorphic_subgroup_(dihedral_subgroup<N> other, bool strict_equality) const
		{
			if (other.has_reflection)
			{
				if (!has_reflection)
					return false;
				else
				{
					if (strict_equality)
						return other.smallest_rotation % smallest_rotation == 0
						&& other.smallest_reflection % smallest_rotation == smallest_reflection;
					else
					{
						// non-cyclic subgroups are dihedral, hence isomorphic
						return other.smallest_rotation % smallest_rotation == 0;
					}
				}
			}
			else
			{
				return other.smallest_rotation % smallest_rotation == 0;
			}
		}
		bool isomorphic_(dihedral_subgroup<N> other, bool strict_equality) const
		{
			return has_isomorphic_subgroup(other, strict_equality)
				&& other.has_isomorphic_subgroup(*this, strict_equality);
		}
	};

	template<unsigned int N>
	dihedral_subgroup<N> stabiliser(dirN<N> dir)
	{
		int stab = modulo(2 * dir.dir, (int)N);
		return dihedral_subgroup<N>(symN<N>(stab, true));
	}

	template<unsigned int N>
	dihedral_subgroup<N> full_group()
	{
		return dihedral_subgroup<N>({ symN<N>(1), symN<N>(0, true) });
	}

	template<unsigned int N>
	dihedral_subgroup<N> trivial_subgroup()
	{
		return dihedral_subgroup<N>();
	}

}