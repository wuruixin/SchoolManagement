#pragma once
#include <windows.h>
#include <functional>
#include <queue>
#include <vector>

#include "lock.h"

namespace TOOL
{
	/*�̳߳�*/
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

			/*�ر��̳߳�*/
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

			/*��ʼ���̳߳ش����߳�����*/
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

			/*���̳߳���������*/
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
			/*�����߳�*/
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
			/*��������*/
			std::queue<TASK> m_workQue;
			TOOL::LOCKS::CriticalSection m_workQueLock;

			/*�����ź�*/
			HANDLE m_semapWork;

			/*����ֹͣ*/
			long m_nStop;

			/*���������̵߳�����*/
			std::vector<HANDLE> m_hThreadVec;
		};
	}
}