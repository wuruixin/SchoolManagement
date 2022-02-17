#pragma once
#include <windows.h>

namespace TOOL
{
	/*锁相关，自旋锁，临界区*/
	namespace LOCKS
	{

		/*自旋锁*/
		class SpinLock
		{
		public:

			SpinLock() : m_lock(0) {}
			~SpinLock() {}
			void Lock()
			{
				while (InterlockedExchange(&m_lock, 1) == 1)
				{
					Sleep(1);
				}
			}

			void Unlock()
			{
				InterlockedExchange(&m_lock, 0);
			}
		private:
			volatile unsigned int m_lock;
		};

		/*临界区*/
		class CriticalSection
		{
		public:
			CriticalSection()
			{
				InitializeCriticalSection(&m_cs);
			}

			~CriticalSection()
			{
				DeleteCriticalSection(&m_cs);
			}

			void Lock()
			{
				EnterCriticalSection(&m_cs);
			}

			void Unlock()
			{
				LeaveCriticalSection(&m_cs);
			}
		private:
			CriticalSection(const CriticalSection&);
			CRITICAL_SECTION m_cs;
		};
	}//LOCK
}//TOOL