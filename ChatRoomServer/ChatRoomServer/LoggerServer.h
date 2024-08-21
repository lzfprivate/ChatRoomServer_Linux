#pragma once
#include <stdio.h>
#include "Buffer.h"
#include "Log.h"
#include "Epoll.h"
#include "Thread.h"
#include "LocalSocket.h"
class CLoggerServer
{
public:
	CLoggerServer();
	CLoggerServer(const CLoggerServer& server) = delete;
	CLoggerServer& operator=(const CLoggerServer& server) = delete;
	~CLoggerServer();

	//启动
	int Start();

	//打印
	static void Trace(const CBuffer& buff);

	//写文件
	void WriteLog(const CBuffer& buff);
	//关闭
	void Close();

private:
	//线程函数:实现主功能
	void ThreadFunc(void* args);

	//获取当前时间
	CBuffer GetTimeStr();



private:
	CEpoll		m_epoll;			//
	CThread		m_thread;			//线程

	CSockBase* m_socket;			//本地通信端
	CBuffer		m_bufPath;			//文件路径
	FILE* m_file;					//写入文件的文件指针
};

#ifndef TRACE
#define TRACEI(...)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnMsg,__VA_ARGS__))
#define TRACED(...)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnDebug,__VA_ARGS__))
#define TRACEW(...)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnWarning,__VA_ARGS__))
#define TRACEE(...)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnError,__VA_ARGS__))
#define TRACEF(...)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnFatal,__VA_ARGS__))

#define LOGI		CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnMsg)
#define LOGD		CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnDebug)
#define LOGW		CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnWarning)
#define LOGE		CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnError)
#define LOGF		CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnFatal)

#define DUMPI(data,size)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnMsg,data,size))
#define DUMPD(data,size)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnDebug,data,size))
#define DUMPW(data,size)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnWarning,data,size))
#define DUMPE(data,size)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnError,data,size))
#define DUMPF(data,size)		CLoggerServer::Trace(CLogMessage(__FILE__,__LINE__,__FUNCTION__,getpid(),pthread_self(),EnFatal,data,size))

#endif // !TRACEI(...)
