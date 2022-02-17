#include "CUMT.h"
#include "time.h"
#pragma omp parallel shared
// Accept���ӽӿ�

BOOL CUMT::socketbind(LPCTSTR szIp, USHORT nPort)
{
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        return FALSE;
    }

    // �󶨶˿�
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
    //     // ������
    //     pCli->m_sock = m_sock;
    //     // ����socket
    // 	
    //     int nRet = 0;
    //     // �շ��˿�
    //     while (true)
    //     {
    //         // �յ�һ����
    //         CPackage pkg;
    //         int nLen = sizeof(pCli->m_siDst);
    //         nRet = recvfrom(pCli->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&pCli->m_siDst, &nLen);
    //         if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != pCli->m_nNextRecvSeq)
    //         {
    //             continue;
    //         }
    // 
    //         // �ص�һ����
    //         CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
    //         nRet = sendto(pCli->m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&pCli->m_siDst, sizeof(pCli->m_siDst));
    //         if (nRet == SOCKET_ERROR)
    //         {
    //             continue;
    //         }
    // 
    //         // �յڶ�����
    //         nRet = recvfrom(pCli->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&pCli->m_siDst, &nLen);
    //         if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != pCli->m_nNextRecvSeq)
    //         {
    //             continue;
    //         }
    // 
    //         // ���ӽ���
    //         break;
    //     }
    //     // ��ʼ��
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

// ��ʼ��
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
    // �������������ݵ��߳�
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

// ��־
VOID CUMT::Log(const char* szFmt, ...)
{
    char szBuff[MAXWORD] = {};
    va_list vl;
    va_start(vl, szFmt);
    vsprintf(szBuff, szFmt, vl);
    va_end(vl);
    OutputDebugString(szBuff);
}

// Connect���ӷ������ӿ�
BOOL CUMT::Connect(LPCTSTR szIp, USHORT nPort)
{
    // ������

   // ����socket
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        return FALSE;
    }
    m_siDst.sin_family = AF_INET;
    m_siDst.sin_addr.S_un.S_addr = inet_addr(szIp);
    m_siDst.sin_port = htons(nPort);

    // ����һ����
    CPackage pkgSend(PT_SYN, m_nNextSendSeq);
    int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
    if (nRet == SOCKET_ERROR)
    {
        Clear();
        return FALSE;
    }
    // �յ�һ����
    CPackage pkg;
    int nLen = sizeof(m_siDst);
    nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
    if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
    {
        Clear();
        return FALSE;
    }
    // �յڶ�����
    CPackage pkgSendAck(PT_SYN | PT_ACK, m_nNextSendSeq);
    nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
    if (nRet == SOCKET_ERROR)
    {
        Clear();
        return FALSE;
    }

    ULONGLONG uuid = ((ULONGLONG)m_siDst.sin_addr.S_un.S_addr << 32) + m_siDst.sin_port;
    m_Connent.insert(pair<ULONGLONG, CUMT*>(uuid, this));
    // ���ӽ���
    return Init();
}
// ��������
DWORD CUMT::Send(LPBYTE pBuff, DWORD dwBufLen)
{
    // ���Ʒ��������еİ����Ϊ100��
    while (true)
    {
        m_lckForSendMp.Lock();
        DWORD dwSize = m_mpSend.size();
        m_lckForSendMp.UnLock();
        if (dwSize > 100)
        {
            // �г��߳�
            Sleep(1);
            continue;
        }
        else
        {
            break;
        }
    }

    // �������������
    m_lckForSendMp.Lock();
    DWORD dwCnt = (dwBufLen % DATALEN == 0 ? dwBufLen / DATALEN : (dwBufLen / DATALEN + 1));// �������������������ֱ�ӳ���������+1
    for (DWORD i = 0; i < dwCnt; ++i)
    {
        DWORD dwLen = DATALEN;
        if (i == dwCnt - 1)
        {
            // ���һ���������������
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
// ��������
DWORD CUMT::Recv(LPBYTE pBuff, DWORD dwBufLen)
{
    while (true)
    {
        m_lckForBufRecv.Lock();
        DWORD dwSize = m_bufRecv.GetSize();
        m_lckForBufRecv.UnLock();

        if (dwSize <= 0)
        {
            // �����ݣ��������г�ȥ
            Sleep(1);
        }
        else
        {
            // ������
            break;
        }
    }
    // ������
    m_lckForBufRecv.Lock();
    DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLen) ? dwBufLen : m_bufRecv.GetSize();
    m_bufRecv.Read(pBuff, dwDataLen);
    m_lckForBufRecv.UnLock();

    return dwDataLen;
}

// �ر�
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

// ������
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

// �����߳�
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
            // û�з���ȥ�İ�������

            // ��ʱ�İ�������
            ULONGLONG tmCurrent = GetTickCount64();
            if (tmCurrent - pi.second.m_tmLastTime > pThis->m_tmElapse) // ��ʱ500ms
            {
                if (pi.second.m_tmLastTime == 0)
                {
                    // ��һ�ΰ�
                    pThis->Log("[umt]:package ==> net first seq:%d", pi.second.m_pkg.m_nSeq);
                }
                else
                {
                    // ��ʱ��
                    pThis->Log("[umt]:package ==> net timeout seq:%d", pi.second.m_pkg.m_nSeq);
                }

                sendto(pThis->m_sock, (char*)&pi.second.m_pkg, sizeof(pi.second.m_pkg),
                    0, (sockaddr*)&pi.second.sDist, sizeof(pi.second.sDist));
                pi.second.m_tmLastTime = tmCurrent;
            }

        }
        pThis->m_lckForSendMp.UnLock();

        // �г��߳�
        Sleep(1);
    }
    return 0;
}

// �հ��߳�
DWORD CUMT::RecvThread(LPVOID lpParam)
{
    CUMT* pThis = (CUMT*)lpParam;
    while (pThis->m_bWorking)
    {
        // �հ�
        sockaddr_in si = {};
        int nLen = sizeof(si);
        CPackage pkg;
        int nRet = recvfrom(pThis->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&si, &nLen);
        if (nRet == 0 || nRet == SOCKET_ERROR)
        {
            continue;
        }
        // �жϰ�������
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
        case PT_ACK:    // �յ�ACK��������Ӧseq�İ��ӷ����������Ƴ�
        {
            pThis->m_lckForSendMp.Lock();
            pThis->Log("[umt]:package ==> ack seq:%d", pkg.m_nSeq);
            pThis->m_mpSend.erase(pkg.m_nSeq);
            pThis->m_lckForSendMp.UnLock();
            break;
        }
        case PT_DATA:   // �յ����ݰ��������У��
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
                // У�飺�ɹ��գ�ʧ�ܶ�
                DWORD dwCheck = CCrc32::crc32(pkg.m_aryData, pkg.m_nLen);
                // У��ʧ�ܣ�������
                if (dwCheck != pkg.m_nCheck)
                {
                    break;
                }
                // У��ɹ��ظ�ACK�����������հ�����
                CPackage pkgAck(PT_ACK, pkg.m_nSeq);
                sendto(pThis->m_sock, (char*)&pkgAck, sizeof(pkgAck), 0, (sockaddr*)&si, sizeof(si));
                // ��������
                pCli->m_lckForRecvMp.Lock();
                // �жϰ���ţ������д���ŵİ��Ѿ����� ||  ����ŵİ��������Ѿ����뻺����
                if (pCli->m_mpRecv.find(pkg.m_nSeq) != pCli->m_mpRecv.end() || pkg.m_nSeq < pCli->m_nNextRecvSeq)
                {
                    pCli->m_lckForRecvMp.UnLock();
                    break;
                }
                pCli->m_mpRecv[pkg.m_nSeq] = pkg;      // ���ս�������
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

// �������հ��������뻺�������߳�
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
                // �ж���Ű��Ƿ��Ѿ�����
                if (pCli->m_mpRecv.find(pCli->m_nNextRecvSeq) != pCli->m_mpRecv.end())
                {
                    // ���ڵĻ�������������ȡ������������
                    auto& pkg = pCli->m_mpRecv[pCli->m_nNextRecvSeq];
                    pCli->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);  // ���ݰ���������
                    pCli->Log("[umt]:package ==> buf seq:%d", pCli->m_nNextRecvSeq);

                    // ���������Ƴ���
                    pCli->m_mpRecv.erase(pCli->m_nNextRecvSeq);

                    // ��Ÿ���
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
        // �г���ǰ�߳�
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
