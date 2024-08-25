#pragma once
#include <stdio.h>
#include "Log.h"
#include "Epoll.h"
#include "Thread.h"
#include "LocalSocket.h"

/// <summary>
/// �������ڷ�װ��־������
/// ���а���CEpoll�����ָ��; ���ڶ��߳�ͨ��
/// ���а���CThread��; ���ڵȴ��ͻ��˵Ľ�������ݴ���
/// ���а���CSockBase��ָ��; ���ڱ���ͨ�źͱ���ͨ���������
/// ���а���FILE�ļ�ָ����ļ�·��; ���ڱ�����־������
/// 
/// ��־�����������½ӿ�:
/// ����������:
/// ��ӡ��־(������־):
/// ��־д�뵽���ش���:
/// �ر�:
/// �ȴ��ͻ������Ӻͽ��տͻ��˵�����(��ҵ��,���߳���ʵ��)
/// ����:һЩ���߽ӿ�
/// 
/// </summary>


//TODO:
class CLoggerServer
{
public:
	CLoggerServer();
	CLoggerServer(const CLoggerServer& server) = delete;
	CLoggerServer& operator=(const CLoggerServer& server) = delete;
	~CLoggerServer();

	//����
	int Start();

	//��ӡ
	static void Trace(const CBuffer& buff);

	//д�ļ�
	void WriteLog(const CBuffer& buff);
	//�ر�
	void Close();

private:
	//�̺߳���:ʵ��������
	void ThreadFunc(void* args);

	//��ȡ��ǰʱ��
	CBuffer GetTimeStr();

private:
	CEpoll		m_epoll;			//epoll
	CThread		m_thread;			//�߳�

	CSockBase* m_socket;			//����ͨ�Ŷ�
	CBuffer		m_bufPath;			//�ļ�·��
	FILE* m_file;					//д���ļ����ļ�ָ��
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
