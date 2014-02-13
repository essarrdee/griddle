#pragma once
#include <stdexcept>
#include "position.h"

namespace griddle
{
	template<typename P>
	class locateable
	{
	public:
		typedef typename position<P> pos_t;
		inline pos_t get_location() const { return mLocation; }
		inline bool placed() const { return !mLocation.nowhere(); }
		inline bool has_lock(const void* lock) const { return lock == mLock; }
		inline bool unlocked() const { return mLock == nullptr; }
	protected:
		template<typename P, typename T>
		friend class mover;
		inline void setLocation(pos_t pos, void* lock)
		{
			getLock(lock);
			mLocation = pos;
		}
		inline void unlock(void* lock)
		{
			getLock(lock);
			if (placed())
			{
				throw new std::runtime_error("Tried to unlock placed locateable.");
				return;
			}
			mLock = nullptr;
		}
		inline void getLock(void* lock)
		{
			if (mLock != nullptr && mLock != lock)
			{
				throw new std::runtime_error("Tried to lock locateable with foreign lock.");
				return;
			}
			mLock = lock;
		}
	protected:
		pos_t mLocation;
		// this will always be a pointer to a mover<T> or nullptr
		void* mLock;
	};

}