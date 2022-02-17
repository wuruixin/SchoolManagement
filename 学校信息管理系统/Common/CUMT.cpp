#include "CUMT.h"
#include "time.h"
#pragma omp parallel shared
// Accept连接接口

BOOL CUMT::socketbind(LPCTSTR szIp, USHORT nPort)
{
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        return FALSE;
    }

    // 绑定端口
    sockaddr_in siServer = {};
    siServer.sin_family = AF_INET;
    siServer.sin_addr.S_un.S_addr = inet_addr(szIp);
    siServer.sin_port = htons(nPort);

    int nRet = bind(m_sock, (sockaddr*)&siServer, sizeof(siServer));
    if (nRet == SOCKET_ERROR)
    {
        closesocket(m_sock);
        return FALSE;
    }
    return TRUE;
}

BOOL CUMT::Accept(CUMT** pCli)
{
    //     // 服务器
    //     pCli->m_sock = m_sock;
    //     // 创建socket
    // 	
    //     int nRet = 0;
    //     // 收发端口
    //     while (true)
    //     {
    //         // 收第一个包
    //         CPackage pkg;
    //         int nLen = sizeof(pCli->m_siDst);
    //         nRet = recvfrom(pCli->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&pCli->m_siDst, &nLen);
    //         if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != pCli->m_nNextRecvSeq)
    //         {
    //             continue;
    //         }
    // 
    //         // 回第一个包
    //         CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
    //         nRet = sendto(pCli->m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&pCli->m_siDst, sizeof(pCli->m_siDst));
    //         if (nRet == SOCKET_ERROR)
    //         {
    //             continue;
    //         }
    // 
    //         // 收第二个包
    //         nRet = recvfrom(pCli->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&pCli->m_siDst, &nLen);
    //         if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != pCli->m_nNextRecvSeq)
    //         {
    //             continue;
    //         }
    // 
    //         // 连接建立
    //         break;
    //     }
    //     // 初始化
    // 
    //     return pCli->Init();
    while (m_bWorking)
    {
        ULONGLONG uuid = 0;
        m_accp_lock.Lock();
        {
            if (m_accp.size() > 0)
            {
                uuid = m_accp.front().first;
                *pCli = m_accp.front().second;
                m_accp.pop();

                m_Connent.insert(pair<ULONGLONG, CUMT*>(uuid, *pCli));
                m_accp_lock.UnLock();
                return TRUE;
            }
        }
        m_accp_lock.UnLock();
        Sleep(10);
    }
    return FALSE;
}

// 初始化
BOOL CUMT::Init()
{
    m_nNextRecvSeq++;
    m_nNextSendSeq++;
    m_bWorking = TRUE;

    m_hSendThread = CreateThread(NULL, 0, SendThread, this, 0, NULL);
    if (m_hSendThread == NULL)
    {
        Clear();
        return FALSE;
    }
    // 开两个接收数据的线程
    m_hRecvThread = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
    if (m_hRecvThread == NULL)
    {
        Clear();
        return FALSE;
    }
    m_hRecvThread = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
    if (m_hRecvThread == NULL)
    {
        Clear();
        return FALSE;
    }
    m_hHandleThread = CreateThread(NULL, 0, HandleRecvPkgsThread, this, 0, NULL);
    if (m_hHandleThread == NULL)
    {
        Clear();
        return FALSE;
    }
    return TRUE;
}

// 日志
VOID CUMT::Log(const char* szFmt, ...)
{
    char szBuff[MAXWORD] = {};
    va_list vl;
    va_start(vl, szFmt);
    vsprintf(szBuff, szFmt, vl);
    va_end(vl);
    OutputDebugString(szBuff);
}

// Connect连接服务器接口
BOOL CUMT::Connect(LPCTSTR szIp, USHORT nPort)
{
    // 服务器

   // 创建socket
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        return FALSE;
    }
    m_siDst.sin_family = AF_INET;
    m_siDst.sin_addr.S_un.S_addr = inet_addr(szIp);
    m_siDst.sin_port = htons(nPort);

    // 发第一个包
    CPackage pkgSend(PT_SYN, m_nNextSendSeq);
    int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
    if (nRet == SOCKET_ERROR)
    {
        Clear();
        return FALSE;
    }
    // 收第一个包
    CPackage pkg;
    int nLen = sizeof(m_siDst);
    nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
    if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
    {
        Clear();
        return FALSE;
    }
    // 收第二个包
    CPackage pkgSendAck(PT_SYN | PT_ACK, m_nNextSendSeq);
    nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
    if (nRet == SOCKET_ERROR)
    {
        Clear();
        return FALSE;
    }

    ULONGLONG uuid = ((ULONGLONG)m_siDst.sin_addr.S_un.S_addr << 32) + m_siDst.sin_port;
    m_Connent.insert(pair<ULONGLONG, CUMT*>(uuid, this));
    // 连接建立
    return Init();
}
// 发送数据
DWORD CUMT::Send(LPBYTE pBuff, DWORD dwBufLen)
{
    // 限制发包容器中的包大概为100个
    while (true)
    {
        m_lckForSendMp.Lock();
        DWORD dwSize = m_mpSend.size();
        m_lckForSendMp.UnLock();
        if (dwSize > 100)
        {
            // 切出线程
            Sleep(1);
            continue;
        }
        else
        {
            break;
        }
    }

    // 拆包，包进容器
    m_lckForSendMp.Lock();
    DWORD dwCnt = (dwBufLen % DATALEN == 0 ? dwBufLen / DATALEN : (dwBufLen / DATALEN + 1));// 计算包的数量：整数倍直接除，有余数+1
    for (DWORD i = 0; i < dwCnt; ++i)
    {
        DWORD dwLen = DATALEN;
        if (i == dwCnt - 1)
        {
            // 最后一个包，计算包长度
            dwLen = (dwBufLen - i * DATALEN);
        }
        CPackage pkg(PT_DATA, m_nNextSendSeq, pBuff + i * DATALEN, dwLen);
        if (!this->m_bIsCli)
        {
            this->pServer->m_mpSend[m_nNextSendSeq] = CPackageInfo(0, pkg, m_siDst);
        }
        else
        {
            m_mpSend[m_nNextSendSeq] = CPackageInfo(0, pkg, m_siDst);
        }

        Log("[umt]:package ==> map seq:%d", m_nNextSendSeq);
        ++m_nNextSendSeq;
    }
    m_lckForSendMp.UnLock();
    return dwBufLen;
}
// 接收数据
DWORD CUMT::Recv(LPBYTE pBuff, DWORD dwBufLen)
{
    while (true)
    {
        m_lckForBufRecv.Lock();
        DWORD dwSize = m_bufRecv.GetSize();
        m_lckForBufRecv.UnLock();

        if (dwSize <= 0)
        {
            // 无数据，阻塞，切出去
            Sleep(1);
        }
        else
        {
            // 有数据
            break;
        }
    }
    // 有数据
    m_lckForBufRecv.Lock();
    DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLen) ? dwBufLen : m_bufRecv.GetSize();
    m_bufRecv.Read(pBuff, dwDataLen);
    m_lckForBufRecv.UnLock();

    return dwDataLen;
}

// 关闭
VOID CUMT::Close()
{
    m_bWorking = FALSE;
    shutdown(m_sock, SD_BOTH);
    closesocket(m_sock);
    if (m_hSendThread != NULL)
    {
        CloseHandle(m_hSendThread);
    }
    if (m_hRecvThread != NULL)
    {
        CloseHandle(m_hRecvThread);
    }
    if (m_hHandleThread != NULL)
    {
        CloseHandle(m_hHandleThread);
    }
    return;
}

// 清理函数
VOID CUMT::Clear()
{
    closesocket(m_sock);
    m_bWorking = FALSE;
    if (m_hSendThread != NULL)
    {
        CloseHandle(m_hSendThread);
    }
    if (m_hRecvThread != NULL)
    {
        CloseHandle(m_hRecvThread);
    }
    if (m_hHandleThread != NULL)
    {
        CloseHandle(m_hHandleThread);
    }
}

// 发包线程
DWORD CUMT::SendThread(LPVOID lpParam)
{
    CUMT* pThis = (CUMT*)lpParam;
    while (pThis->m_bWorking)
    {
        pThis->m_lckForSendMp.Lock();
        if (!pThis->m_bWorking)
        {
            pThis->m_lckForSendMp.UnLock();
            break;
        }

        for (auto& pi : pThis->m_mpSend)
        {
            // 没有发出去的包，发送

            // 超时的包，发送
            ULONGLONG tmCurrent = GetTickCount64();
            if (tmCurrent - pi.second.m_tmLastTime > pThis->m_tmElapse) // 超时500ms
            {
                if (pi.second.m_tmLastTime == 0)
                {
                    // 第一次包
                    pThis->Log("[umt]:package ==> net first seq:%d", pi.second.m_pkg.m_nSeq);
                }
                else
                {
                    // 超时包
                    pThis->Log("[umt]:package ==> net timeout seq:%d", pi.second.m_pkg.m_nSeq);
                }

                sendto(pThis->m_sock, (char*)&pi.second.m_pkg, sizeof(pi.second.m_pkg),
                    0, (sockaddr*)&pi.second.sDist, sizeof(pi.second.sDist));
                pi.second.m_tmLastTime = tmCurrent;
            }

        }
        pThis->m_lckForSendMp.UnLock();

        // 切出线程
        Sleep(1);
    }
    return 0;
}

// 收包线程
DWORD CUMT::RecvThread(LPVOID lpParam)
{
    CUMT* pThis = (CUMT*)lpParam;
    while (pThis->m_bWorking)
    {
        // 收包
        sockaddr_in si = {};
        int nLen = sizeof(si);
        CPackage pkg;
        int nRet = recvfrom(pThis->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&si, &nLen);
        if (nRet == 0 || nRet == SOCKET_ERROR)
        {
            continue;
        }
        // 判断包的类型
        switch (pkg.m_nPt)
        {
        case PT_SYN:
        {
            CPackage pkgSend(PT_SYN | PT_ACK, 0);
            nRet = sendto(pThis->m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&si, sizeof(si));
            break;
        }
        case (PT_SYN | PT_ACK):
        {
            ULONGLONG uuid = ((ULONGLONG)si.sin_addr.S_un.S_addr << 32) + si.sin_port;
            pThis->m_accp_lock.Lock();
            {
                CUMT* pCli = new CUMT{};
                pCli->m_sock = pThis->m_sock;
                memcpy(&pCli->m_siDst, &si, sizeof(sockaddr_in));
                pCli->m_nNextRecvSeq++;
                pCli->m_nNextSendSeq++;
                pCli->m_bIsCli = false;
                pCli->pServer = pThis;
                pThis->m_accp.push(pair<ULONGLONG, CUMT*>(uuid, pCli));
            }
            pThis->m_accp_lock.UnLock();
            break;
        }
        case PT_ACK:    // 收到ACK包，将对应seq的包从发送容器中移除
        {
            pThis->m_lckForSendMp.Lock();
            pThis->Log("[umt]:package ==> ack seq:%d", pkg.m_nSeq);
            pThis->m_mpSend.erase(pkg.m_nSeq);
            pThis->m_lckForSendMp.UnLock();
            break;
        }
        case PT_DATA:   // 收到数据包，则进行校验
        {
            ULONGLONG uuid = ((ULONGLONG)si.sin_addr.S_un.S_addr << 32) + si.sin_port;
            CUMT* pCli = nullptr;
            pThis->m_accp_lock.Lock();
            {
                auto itr = pThis->m_Connent.find(uuid);
                if (itr != pThis->m_Connent.end())
                {
                    pCli = itr->second;
                }
            }
            pThis->m_accp_lock.UnLock();
            if (pCli != nullptr)
            {
                // 校验：成功收，失败丢
                DWORD dwCheck = CCrc32::crc32(pkg.m_aryData, pkg.m_nLen);
                // 校验失败，丢弃包
                if (dwCheck != pkg.m_nCheck)
                {
                    break;
                }
                // 校验成功回复ACK，将包丢到收包容器
                CPackage pkgAck(PT_ACK, pkg.m_nSeq);
                sendto(pThis->m_sock, (char*)&pkgAck, sizeof(pkgAck), 0, (sockaddr*)&si, sizeof(si));
                // 包进容器
                pCli->m_lckForRecvMp.Lock();
                // 判断包序号：容器中此序号的包已经存在 ||  此序号的包中数据已经进入缓冲区
                if (pCli->m_mpRecv.find(pkg.m_nSeq) != pCli->m_mpRecv.end() || pkg.m_nSeq < pCli->m_nNextRecvSeq)
                {
                    pCli->m_lckForRecvMp.UnLock();
                    break;
                }
                pCli->m_mpRecv[pkg.m_nSeq] = pkg;      // 包收进包容器
                pCli->Log("[umt]:package net ==> map seq:%d", pkg.m_nSeq);
                pCli->m_lckForRecvMp.UnLock();
            }
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

// 将包从收包容器放入缓冲区的线程
DWORD CUMT::HandleRecvPkgsThread(LPVOID lpParam)
{
    CUMT* pThis = (CUMT*)lpParam;
    while (pThis->m_bWorking)
    {
        std::vector<CUMT*> vec;
        pThis->m_accp_lock.Lock();
        {
            for (auto itr = pThis->m_Connent.begin(); itr != pThis->m_Connent.end(); ++itr)
            {
                vec.push_back(itr->second);
            }
        }
        pThis->m_accp_lock.UnLock();

        for (auto pCli : vec)
        {
            pCli->m_lckForBufRecv.Lock();
            while (true)
            {
                pCli->m_lckForRecvMp.Lock();
                // 判断序号包是否已经存在
                if (pCli->m_mpRecv.find(pCli->m_nNextRecvSeq) != pCli->m_mpRecv.end())
                {
                    // 存在的话，将包从容器取出丢进缓冲区
                    auto& pkg = pCli->m_mpRecv[pCli->m_nNextRecvSeq];
                    pCli->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);  // 数据包进缓冲区
                    pCli->Log("[umt]:package ==> buf seq:%d", pCli->m_nNextRecvSeq);

                    // 从容器中移除包
                    pCli->m_mpRecv.erase(pCli->m_nNextRecvSeq);

                    // 序号更新
                    ++pCli->m_nNextRecvSeq;
                }
                else
                {
                    pCli->m_lckForRecvMp.UnLock();
                    break;
                }
                pCli->m_lckForRecvMp.UnLock();
            }
            pCli->m_lckForBufRecv.UnLock();
        }
        // 切出当前线程
        Sleep(1);
    }
    return 0;
}

CUMT::CPackage::CPackage(WORD nPt, DWORD dwSeq, LPBYTE pData, WORD nLen)
{
    m_nPt = nPt;
    m_nSeq = dwSeq;
    m_nLen = 0;
    m_nCheck = 0;
    if (pData != NULL)
    {
        memcpy(m_aryData, pData, nLen);
        m_nLen = nLen;
        m_nCheck = CCrc32::crc32(pData, nLen);
    }
};
