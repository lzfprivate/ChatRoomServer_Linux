#pragma once
#include "Process.h"
#include "Bussiness.h"
#include "ThreadPool.h"
/// <summary>
/// 主模块
/// 创建日志服务器进程
/// 创建客户端处理进程
/// 创建网络服务器
/// </summary>
class CServer
{
public:
	CServer();
	~CServer();
	//初始化
	int Init(CBussiness* bussiness, CBuffer bufIP, short port);
	//运行
	int Run();
	//关闭
	int Close();

private:
	//线程回调函数
	int ThreadFunc();


private:
	CProcess	m_process;
	CBussiness* m_bussiness;
	CSockBase* m_server;
	CEpoll		m_epoll;
	CThreadPool m_pool;

};

