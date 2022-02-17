#pragma once
#include <windows.h>
#include <functional>
#include <queue>
#include <vector>

#include "lock.h"

namespace TOOL
{
	/*线程池*/
	namespace POOL
	{
		class CThreadPool
		{
		public:
			using TASK = std::function<void(void)>;

			CThreadPool()
			{
				m_semapWork = NULL;
				m_nStop = 0;
			}

			~CThreadPool()
			{

			}

			/*关闭线程池*/
			void clos()
			{
				if (m_hThreadVec.size() > 0)
				{
					m_nStop = 1;

					::ReleaseSemaphore(m_semapWork, 100, 0);

					::WaitForMultipleObjects(m_hThreadVec.size(), m_hThreadVec.data(), TRUE, 3000);
					for (auto itr : m_hThreadVec)
					{
						CloseHandle(itr);
					}
				}
			}

			/*初始化线程池传入线程数量*/
			bool InitThreadPool(DWORD size)
			{
				m_semapWork = ::CreateSemaphore(FALSE, 0, 0x7fffffff, NULL);
				if (m_semapWork == NULL)
				{
					return false;
				}

				int nNum = (size > 4) ? size : 4;

				for (int i = 0; i < nNum; i++)
				{
					HANDLE hThread = ::CreateThread(FALSE, 0, _ThreadWork, this, 0, NULL);
					if (hThread == NULL)
					{
						return false;
					}
					m_hThreadVec.push_back(hThread);
				}
				return true;
			}

			/*往线程池增加任务*/
			void AddTask(TASK fun)
			{
				m_workQueLock.Lock();
				{
					m_workQue.push(fun);
					::ReleaseSemaphore(m_semapWork, 1, 0);
				}
				m_workQueLock.Unlock();
			}

		private:
			/*工作线程*/
			static DWORD WINAPI _ThreadWork(LPVOID lpParam)
			{
				CThreadPool* pThis = (CThreadPool*)lpParam;

				while (pThis->m_nStop == 0)
				{
					::WaitForSingleObject(pThis->m_semapWork, INFINITE);
					if (pThis->m_nStop == 1)
					{
						break;
					}
					TASK task;
					pThis->m_workQueLock.Lock();
					{
						task = pThis->m_workQue.front();
						pThis->m_workQue.pop();
					}
					pThis->m_workQueLock.Unlock();
					task();
				}
				return 0;
			}

		private:
			/*工作队列*/
			std::queue<TASK> m_workQue;
			TOOL::LOCKS::CriticalSection m_workQueLock;

			/*工作信号*/
			HANDLE m_semapWork;

			/*用于停止*/
			long m_nStop;

			/*所有启动线程的数组*/
			std::vector<HANDLE> m_hThreadVec;
		};
	}
}