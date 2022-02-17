#pragma once

#include "CThreadPool.h"
#include "lock.h"
#include "../include/mysql.h"

#include <functional>
#include <string>


#pragma comment(lib, "../lib/libmysql.lib")

using namespace std;
using namespace TOOL;

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

/*线程池*/
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

	/*执行sql语句*/
	int sql_Execute(MYSQL* m_connect, const char* sql, RES* res = nullptr)
	{
		int nRet = mysql_real_query(m_connect, sql, (unsigned int)strlen(sql));
		if (nRet != 0)
		{
			return nRet;	// 0表成功，非0表失败
		}

		if (res == nullptr)
		{
			return 0;
		}

		// 获取结果集
		MYSQL_RES* mysqlRes = mysql_store_result(m_connect);
		if (mysqlRes == nullptr)
		{
			if (mysql_field_count(m_connect) == 0)
			{
				return 0;
			}
			return -1;
		}
		// 根据结果集获取列头信息
		int nNum = mysql_num_fields(mysqlRes);
		MYSQL_FIELD* field = mysql_fetch_fields(mysqlRes);
		for (int i = 0; i < nNum; i++)
		{
			tagRes::tagHead head;
			head.sName = field[i].name;
			head.sType = field[i].type;
			res->head.push_back(head);
		}
		// 根据列头信息获取行信息
		MYSQL_ROW row = mysql_fetch_row(mysqlRes);
		while (row != NULL)
		{
			// 循环存储
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
		// 循释放结果集
		mysql_free_result(mysqlRes);
		return 0;
	}


	/*初始化线程池传入线程数量*/
	bool InitThreadPool(DWORD size, std::string sDB="db_stu_mag", std::string sHost = "localhost",
		std::string sAccount = "root", std::string sPasswad = "toor", DWORD dwProt = 3306)
	{
		int nNum = (size > 4) ? size : 4;
		for (int i = 0; i < nNum; i++)
		{
			// 初始化库，一次即可
			MYSQL* pSql = mysql_init(NULL);
			if (pSql == nullptr)
			{
				return false;
			}

			// 连接mysql服务器
			MYSQL* pSqlConn = mysql_real_connect(pSql,
				sHost.c_str(),		// 主机IP，"localhost"为本地
				sAccount.c_str(), 	// 用户名
				sPasswad.c_str(), 	// 密码
				sDB.c_str(), 		// 数据库
				dwProt, 			// 端口号
				0, 0);
			if (pSqlConn != pSql)
			{
				// mysql_error显示错误
				const char* pErr = mysql_error(pSql);
				return false;
			}

			// 通过mysql，返回的结果转换成gbk编
			mysql_set_character_set(pSql, "gbk");
			POOLSQL* pNode = new POOLSQL;
			pNode->pSql = pSqlConn;
			pNode->pNext = nullptr;
			FreeSql(pNode);
		}

		CThreadPool::InitThreadPool(size);
		return true;
	}

	/*获取一个数据库连接*/
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

	/*释放一个数据库连接*/
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
