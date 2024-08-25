#pragma once
/// <summary>
/// 线程池类:用于多线程数据处理
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
	//开启
	int Start(unsigned int nCount = 10);
	//添加任务
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
	//队列分发任务
	int DispatchTask();
	//关闭
	int Close();

private:
	CEpoll		m_epoll;					

	std::vector<CThread*> m_vecThreads;

	CSockBase* m_server;
	CBuffer		m_bufSockPath;


};


/// <summary>
/// 不与业务关联的线程池
/// </summary>
class CCommonThreadPool {
	CCommonThreadPool();
	~CCommonThreadPool();
	CCommonThreadPool operator=(const CCommonThreadPool&) = delete;
	CCommonThreadPool(const CCommonThreadPool&) = delete;
};

