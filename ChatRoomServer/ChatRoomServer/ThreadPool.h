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
	//获取线程个数
	int Size() const;


private:
	CEpoll		m_epoll;					

	std::vector<CThread*> m_vecThreads;

	CSockBase* m_server;
	CBuffer		m_bufSockPath;


};


/// <summary>
/// 不与业务关联的线程池
///	核心思想:
/// 1.将任务放入阻塞队列
/// 2.线程池中的空闲线程去阻塞队列中获取任务执行
/// 3.执行完成后,再去队列中获取任务执行
/// 
/// 特性:
/// 任务队列
/// 线程数量控制
/// 线程增加和回收
///
/// 
/// </summary>
class CCommonThreadPool {
	CCommonThreadPool();
	~CCommonThreadPool();
	CCommonThreadPool operator=(const CCommonThreadPool&) = delete;
	CCommonThreadPool(const CCommonThreadPool&) = delete;
	CCommonThreadPool(CCommonThreadPool&&) = delete;
};

