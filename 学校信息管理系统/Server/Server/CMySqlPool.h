#pragma once

#include "CThreadPool.h"
#include "lock.h"
#include "../include/mysql.h"

#include <functional>
#include <string>


#pragma comment(lib, "../lib/libmysql.lib")

using namespace std;
using namespace TOOL;

/*�ٽ���*/
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

/*�̳߳�*/
typedef struct tagMysql
{
	MYSQL* pSql;
	tagMysql* pNext;
}POOLSQL, * PPOOLSQL;

class CMySQLPool : public POOL::CThreadPool
{
public:
	CMySQLPool()
	{
		m_sqlFreeList = nullptr;
	}
	~CMySQLPool() {}

	struct tagRes
	{
		struct tagHead
		{
			std::string sName;
			enum_field_types sType;
		};
		std::vector<tagHead> head;
		std::vector<std::vector<std::string>> row;
	};

	using RES = tagRes;

	/*ִ��sql���*/
	int sql_Execute(MYSQL* m_connect, const char* sql, RES* res = nullptr)
	{
		int nRet = mysql_real_query(m_connect, sql, (unsigned int)strlen(sql));
		if (nRet != 0)
		{
			return nRet;	// 0��ɹ�����0��ʧ��
		}

		if (res == nullptr)
		{
			return 0;
		}

		// ��ȡ�����
		MYSQL_RES* mysqlRes = mysql_store_result(m_connect);
		if (mysqlRes == nullptr)
		{
			if (mysql_field_count(m_connect) == 0)
			{
				return 0;
			}
			return -1;
		}
		// ���ݽ������ȡ��ͷ��Ϣ
		int nNum = mysql_num_fields(mysqlRes);
		MYSQL_FIELD* field = mysql_fetch_fields(mysqlRes);
		for (int i = 0; i < nNum; i++)
		{
			tagRes::tagHead head;
			head.sName = field[i].name;
			head.sType = field[i].type;
			res->head.push_back(head);
		}
		// ������ͷ��Ϣ��ȡ����Ϣ
		MYSQL_ROW row = mysql_fetch_row(mysqlRes);
		while (row != NULL)
		{
			// ѭ���洢
			std::vector<std::string> vec;
			for (int i = 0; i < nNum; i++)
			{
				std::string sOne = "";
				if (row[i] != nullptr)
				{
					sOne = row[i];
				}
				vec.push_back(sOne);
			}
			res->row.push_back(vec);
			row = mysql_fetch_row(mysqlRes);
		}
		// ѭ�ͷŽ����
		mysql_free_result(mysqlRes);
		return 0;
	}


	/*��ʼ���̳߳ش����߳�����*/
	bool InitThreadPool(DWORD size, std::string sDB="db_stu_mag", std::string sHost = "localhost",
		std::string sAccount = "root", std::string sPasswad = "toor", DWORD dwProt = 3306)
	{
		int nNum = (size > 4) ? size : 4;
		for (int i = 0; i < nNum; i++)
		{
			// ��ʼ���⣬һ�μ���
			MYSQL* pSql = mysql_init(NULL);
			if (pSql == nullptr)
			{
				return false;
			}

			// ����mysql������
			MYSQL* pSqlConn = mysql_real_connect(pSql,
				sHost.c_str(),		// ����IP��"localhost"Ϊ����
				sAccount.c_str(), 	// �û���
				sPasswad.c_str(), 	// ����
				sDB.c_str(), 		// ���ݿ�
				dwProt, 			// �˿ں�
				0, 0);
			if (pSqlConn != pSql)
			{
				// mysql_error��ʾ����
				const char* pErr = mysql_error(pSql);
				return false;
			}

			// ͨ��mysql�����صĽ��ת����gbk��
			mysql_set_character_set(pSql, "gbk");
			POOLSQL* pNode = new POOLSQL;
			pNode->pSql = pSqlConn;
			pNode->pNext = nullptr;
			FreeSql(pNode);
		}

		CThreadPool::InitThreadPool(size);
		return true;
	}

	/*��ȡһ�����ݿ�����*/
	POOLSQL* AllocSql()
	{
		POOLSQL* pRet = nullptr;
		m_lock.Lock();
		{
			while (true)
			{
				if (m_sqlFreeList != nullptr)
				{
					break;
				}
				Sleep(1);
			}
			pRet = m_sqlFreeList;

			m_sqlFreeList = m_sqlFreeList->pNext;
			pRet->pNext = nullptr;
		}
		m_lock.Unlock();
		return pRet;
	}

	/*�ͷ�һ�����ݿ�����*/
	void FreeSql(POOLSQL* pSqlNode)
	{
		m_lock.Lock();
		{
			pSqlNode->pNext = m_sqlFreeList;
			m_sqlFreeList = pSqlNode;
		}
		m_lock.Unlock();
	}

	void ExcuSql(std::string sSql, std::function<void(RES* res, int nErrCode, const char* pszErrMsg)> fun = nullptr)
	{
		CThreadPool::AddTask([this, sSql, fun]()->void
			{
				POOLSQL* pNode = this->AllocSql();
				if (fun == nullptr)
				{
					int nRet = this->sql_Execute(pNode->pSql, sSql.c_str(), nullptr);
					if (nRet != 0)
					{
						const char* pErr = mysql_error(pNode->pSql);
						OutputDebugString(pErr);
					}
					this->FreeSql(pNode);
					return;
				}
				RES* pRes = new RES{};
				int nRet = this->sql_Execute(pNode->pSql, sSql.c_str(), pRes);
				char* pErr = nullptr;
				if (nRet != 0)
				{
					pErr = (char*)mysql_error(pNode->pSql);
				}
				this->FreeSql(pNode);
				fun(pRes, nRet, pErr);
				delete pRes;
			});
	}
private:
	POOLSQL* m_sqlFreeList;
	CriticalSection m_lock;
};
