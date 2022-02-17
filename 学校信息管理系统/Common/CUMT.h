#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <map>
#include <queue>
#include <vector>
#include "ByteStreamBuff.h"
#include "CLock.h"					   // ͬ��
#include "CCrc32.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class CUMT
{
public:
	// ���ܽӿ�
	BOOL Accept(CUMT** pCli);	  // Accept���ӽӿ�
	BOOL Connect(LPCTSTR szIp, USHORT nPort);	  // Connect���ӷ������ӿ�
	DWORD Send(LPBYTE pBuff, DWORD dwBufLen);	  // ��������
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLen);	  // ��������
	BOOL socketbind(LPCTSTR szIp, USHORT nPort);
	VOID Close();								  // �ر�

	BOOL Init();						// ��ʼ��
private:
#pragma region �ṹ�嶨��

	// ����ʽ
#define DATALEN 1460
	enum PackageType { PT_SYN = 4, PT_ACK = 2, PT_DATA = 1, PT_FIN };

#pragma pack(push) // �������ֵ
#pragma pack(1)		//����ֵ��Ϊ1b�����Ǽ�����
	struct CPackage
	{
		CPackage() {};
		CPackage(WORD nPt, DWORD dwSeq, LPBYTE pData = NULL, WORD nLen = 0);

		WORD m_nPt; 			 // ��������
		WORD m_nLen;			 // ���ݵĳ���
		DWORD m_nSeq; 			 // �������
		DWORD m_nCheck;			 // �����ݵ�Crc32У��ֵ
		BYTE m_aryData[DATALEN]; // ������1460b=1472- (WORD*2)-(DWORD*2)
	};
#pragma pack(pop)	// �ָ�����ֵ

	// ����Ϣ
	const ULONGLONG m_tmElapse = 500;	// ��ʱʱ��500ms
	struct CPackageInfo
	{
		CPackageInfo() {};
		CPackageInfo(ULONGLONG tm, CPackage pkg, sockaddr_in saddr) : m_tmLastTime(tm), m_pkg(pkg), sDist(saddr) {};

		sockaddr_in sDist;
		ULONGLONG m_tmLastTime;  // ʱ��
		CPackage m_pkg;			 // ���ݰ�
	};
#pragma endregion �ṹ�嶨��

	// ���ݳ�Ա
	SOCKET  m_sock;
	sockaddr_in m_siDst = {};			// �Է��ĵ�ַ
	sockaddr_in m_siSrc = {};			// �Լ��ĵ�ַ

	DWORD m_nNextSendSeq = 0;			// ��һ�β���Ŀ�ʼ���
	DWORD m_nNextRecvSeq = 0;			// ��һ�δ��뻺�����İ������


	map<DWORD, CPackageInfo> m_mpSend;	// �洢���Ͱ�������
	CLock m_lckForSendMp;			    // ͬ����������m_mpSend�Ķ��߳�ͬ��
	map<DWORD, CPackage> m_mpRecv;		// �洢�յ��İ�������
	CLock m_lckForRecvMp;			    // ͬ����������m_mpRecv�Ķ��߳�ͬ��
	CByteStreamBuff m_bufRecv;			// �������ݵĻ�����
	CLock m_lckForBufRecv;			    // ͬ����������m_bufRecv�Ķ��߳�ͬ��

	HANDLE m_hSendThread = NULL;		// �����߳̾��
	HANDLE m_hRecvThread = NULL;		// �հ��߳̾��
	HANDLE m_hHandleThread = NULL;		// �������հ��������뻺�������߳̾��
	BOOL m_bWorking = FALSE;			// ���̱߳�־

// ��Ա����
	VOID Clear();						// ������

	VOID Log(const char* szFmt, ...);	// ��־

	static DWORD CALLBACK SendThread(LPVOID lpParam);	// �����߳�
	static DWORD CALLBACK RecvThread(LPVOID lpParam);	// �հ��߳�
	static DWORD CALLBACK HandleRecvPkgsThread(LPVOID lpParam);	// �������հ��������뻺�������߳�


	std::map<ULONGLONG, CUMT*> m_Connent;
	std::queue<pair<ULONGLONG, CUMT*>> m_accp;
	CLock m_accp_lock;
	bool m_bIsCli = true;			//�Ƿ��ǿͻ����׽���
	CUMT* pServer = nullptr;
};

