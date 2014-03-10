#pragma once
#include <set>
#include <memory>
#include <functional>
#include "locateable.h"

namespace griddle
{
	template<typename P, typename T>
	class mover
	{
	public:
		typedef typename position<P> pos_t;
		bool has_lock(const std::shared_ptr<T>& L) const
		{
			return L->has_lock((void*)this);
		}
	protected:
		void moveL(std::shared_ptr<T>& L, pos_t pos)
		{
			L->setLocation(pos, (void*)this);
		}
		virtual void unlockL(std::shared_ptr<T>& L)
		{
			L->unlock((void*)this);
		}
		virtual void lockL(std::shared_ptr<T>& L)
		{
			L->getLock((void*)this);
		}
	};

	template<typename P, typename T>
	class cross_mover : public mover<P,T>
	{
	public:
		cross_mover() : mList(locateable_less<T>) {}
	protected:
		virtual void unlockL(std::shared_ptr<T>& L)
		{
			mover::unlockL(L);
			mList.erase(L);
		}
		virtual void lockL(std::shared_ptr<T>& L)
		{
			mover::lockL(L);
			mList.insert(L);
		}
		std::set<std::shared_ptr<T>, std::function<bool(const std::shared_ptr<T>&, const std::shared_ptr<T>&)>> mList;
	};
}