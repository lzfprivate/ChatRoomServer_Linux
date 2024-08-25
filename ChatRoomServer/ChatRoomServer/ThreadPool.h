#pragma once
/// <summary>
/// �̳߳���:���ڶ��߳����ݴ���
/// 
/// 
/// 
/// 
/// </summary>
#include <vector>
#include "Epoll.h"
#include "Thread.h"
#include "FuncionBase.h"
#include "LocalSocket.h"
#include "Buffer.h"

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();
	CThreadPool& operator=(const CThreadPool& ) = delete;
	CThreadPool(const CThreadPool&) = delete;
	//����
	int Start(unsigned int nCount = 10);
	//�������
	template <typename _FUNCTION_ ,typename ..._ARGS_>
	int AddTask(_FUNCTION_ func, _ARGS_... args) {
		static thread_local CLocalSocket client;
		client.InitSocket(CSockParam(m_bufSockPath, 0));
		client.Link();

		CFunctionBase* base = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
		CBuffer funcName(1024);
		memcpy((void*)funcName.c_str(), base, sizeof(CFunctionBase));
		client.Send(funcName);
		return 0;
	}
	//���зַ�����
	int DispatchTask();
	//�ر�
	int Close();

private:
	CEpoll		m_epoll;					

	std::vector<CThread*> m_vecThreads;

	CSockBase* m_server;
	CBuffer		m_bufSockPath;


};


/// <summary>
/// ����ҵ��������̳߳�
/// </summary>
class CCommonThreadPool {
	CCommonThreadPool();
	~CCommonThreadPool();
	CCommonThreadPool operator=(const CCommonThreadPool&) = delete;
	CCommonThreadPool(const CCommonThreadPool&) = delete;
};

