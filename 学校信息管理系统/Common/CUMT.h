#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <map>
#include <queue>
#include <vector>
#include "ByteStreamBuff.h"
#include "CLock.h"					   // 同步
#include "CCrc32.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class CUMT
{
public:
	// 功能接口
	BOOL Accept(CUMT** pCli);	  // Accept连接接口
	BOOL Connect(LPCTSTR szIp, USHORT nPort);	  // Connect连接服务器接口
	DWORD Send(LPBYTE pBuff, DWORD dwBufLen);	  // 发送数据
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLen);	  // 接收数据
	BOOL socketbind(LPCTSTR szIp, USHORT nPort);
	VOID Close();								  // 关闭

	BOOL Init();						// 初始化
private:
#pragma region 结构体定义

	// 包格式
#define DATALEN 1460
	enum PackageType { PT_SYN = 4, PT_ACK = 2, PT_DATA = 1, PT_FIN };

#pragma pack(push) // 保存对齐值
#pragma pack(1)		//对齐值设为1b，考虑兼容性
	struct CPackage
	{
		CPackage() {};
		CPackage(WORD nPt, DWORD dwSeq, LPBYTE pData = NULL, WORD nLen = 0);

		WORD m_nPt; 			 // 包的类型
		WORD m_nLen;			 // 数据的长度
		DWORD m_nSeq; 			 // 包的序号
		DWORD m_nCheck;			 // 包数据的Crc32校验值
		BYTE m_aryData[DATALEN]; // 包数据1460b=1472- (WORD*2)-(DWORD*2)
	};
#pragma pack(pop)	// 恢复对齐值

	// 包信息
	const ULONGLONG m_tmElapse = 500;	// 超时时间500ms
	struct CPackageInfo
	{
		CPackageInfo() {};
		CPackageInfo(ULONGLONG tm, CPackage pkg, sockaddr_in saddr) : m_tmLastTime(tm), m_pkg(pkg), sDist(saddr) {};

		sockaddr_in sDist;
		ULONGLONG m_tmLastTime;  // 时间
		CPackage m_pkg;			 // 数据包
	};
#pragma endregion 结构体定义

	// 数据成员
	SOCKET  m_sock;
	sockaddr_in m_siDst = {};			// 对方的地址
	sockaddr_in m_siSrc = {};			// 自己的地址

	DWORD m_nNextSendSeq = 0;			// 下一次拆包的开始序号
	DWORD m_nNextRecvSeq = 0;			// 下一次存入缓冲区的包的序号


	map<DWORD, CPackageInfo> m_mpSend;	// 存储发送包的容器
	CLock m_lckForSendMp;			    // 同步对象，用于m_mpSend的多线程同步
	map<DWORD, CPackage> m_mpRecv;		// 存储收到的包的容器
	CLock m_lckForRecvMp;			    // 同步对象，用于m_mpRecv的多线程同步
	CByteStreamBuff m_bufRecv;			// 接收数据的缓冲区
	CLock m_lckForBufRecv;			    // 同步对象，用于m_bufRecv的多线程同步

	HANDLE m_hSendThread = NULL;		// 发包线程句柄
	HANDLE m_hRecvThread = NULL;		// 收包线程句柄
	HANDLE m_hHandleThread = NULL;		// 将包从收包容器放入缓冲区的线程句柄
	BOOL m_bWorking = FALSE;			// 退线程标志

// 成员函数
	VOID Clear();						// 清理函数

	VOID Log(const char* szFmt, ...);	// 日志

	static DWORD CALLBACK SendThread(LPVOID lpParam);	// 发包线程
	static DWORD CALLBACK RecvThread(LPVOID lpParam);	// 收包线程
	static DWORD CALLBACK HandleRecvPkgsThread(LPVOID lpParam);	// 将包从收包容器放入缓冲区的线程


	std::map<ULONGLONG, CUMT*> m_Connent;
	std::queue<pair<ULONGLONG, CUMT*>> m_accp;
	CLock m_accp_lock;
	bool m_bIsCli = true;			//是否是客户端套接字
	CUMT* pServer = nullptr;
};

