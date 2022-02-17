#pragma once
#include <iostream>
#include "../../Common/CUMT.h"

#include <atlstr.h>
#include <vector>
#include "../include/mysql.h"

#pragma comment(lib, "../lib/libmysql.lib")


#include "CThreadPool.h"
#include "CMySqlPool.h"
#include "lock.h"


// 全局MYSQL 对象
MYSQL* m_mysql{};
MYSQL* m_conn{};

class DUOCUMT
{
public:
    DUOCUMT() {};
    ~DUOCUMT() {
        m_UTP.Close();
    };

    CUMT* Accept()
    {
        CUMT* nClien = nullptr;
        m_UTP.Accept(&nClien);
        m_conn.push_back(nClien);
        return nClien;
    }

    BOOL socketbind(LPCTSTR szIp, USHORT nPort)
    {
        return m_UTP.socketbind(szIp, nPort);
    }

    void Init()
    {
        m_UTP.Init();
    }
private:
    CUMT m_UTP;
    std::vector<CUMT*> m_conn;
};


CMySQLPool* pPool = new CMySQLPool;


// _Thread(LPVOID lpPrarm)   响应查询
DWORD WINAPI _Thread(LPVOID lpPrarm)
{
    printf("新连接\r\n");
    CUMT* pUtp = (CUMT*)lpPrarm;
    LPBYTE pBuff = new BYTE[0x1000]{};

    // 循环读取数据
    while (true)
    {
        // 读到缓冲区
        memset(pBuff, '\0', 1000);
        pUtp->Recv(pBuff, 0x1000);

        // 执行sql语句，通过回调函数查询，结果无论成功与否，都会进入函数内部
        pPool->ExcuSql((char*)pBuff, [pUtp](CMySQLPool::RES* res, int nErrCode, const char* pszErrMsg)->void
            {
                int a = 0;
                // 组包
                if (nErrCode == 0) // 查询成功
                {
                    // 遍历结果集头：字段
                    std::string sHead = "";
                    for (auto itr = res->head.begin(); itr != res->head.end(); )
                    {
                        sHead += itr->sName;
                        ++itr;
                        if (itr != res->head.end())
                        {
                            sHead += "|";
                        }
                    }
                    sHead += "/"; // "/"前是头，后是数据

                    // 遍历结果集每一列的数据
                    for (auto itr : res->row)
                    {
                        sHead += "{";
                        for (auto itrRow = itr.begin(); itrRow != itr.end();)
                        {
                            sHead += *itrRow;
                            ++itrRow;
                            if (itrRow != itr.end())
                            {
                                sHead += "|";
                            }
                        }
                        sHead += "}";

                    }
                    CString sLen;
                    sLen.Format("%d", sHead.length());
                    pUtp->Send((LPBYTE)(LPCSTR)sLen, 4);   // TODO 发送长度
                    pUtp->Send((BYTE*)sHead.c_str(), sHead.length());   // 发送数据
                }
                else
                {
                    // 查询失败
                    pUtp->Send((BYTE*)(char*)"查询失败", strlen("查询失败"));
                }

            });
    }
    pUtp->Close();
}

// main()   启动服务
int main() {
    // 服务器初始化
    DUOCUMT server;
    server.socketbind("0.0.0.0", 5566);
    server.Init();

    // 初始化mysql线程池
    pPool->InitThreadPool(4);

    // 循环接受链接
    while (true)
    {
        CUMT* pCli = server.Accept();

        // 连接来之后创建线程
        ::CreateThread(FALSE, 0, _Thread, pCli, 0, 0);
    }

    return 0;
}