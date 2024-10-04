#include "LoggerServer.h"
#include <map>
#include <cstddef>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "LocalSocket.h"


#ifndef ERR_RETURN(err,ret) 
#define ERR_RETURN(err,ret) if(err!= 0) {printf("%s(%d):<%s> ret = %d errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, ret,errno,strerror(errno));return ret;}
#endif

#ifndef ERR_CONTINUE(err)
#define ERR_CONTINUE(err) if(err!= 0){printf("%s(%d):<%s> ret = %d errno=%d errmsg=%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));continue; }
#endif // !ERR_CONTINUE(err)




CLoggerServer::CLoggerServer()
	: m_thread(&CLoggerServer::ThreadFunc, this)
	,m_socket(nullptr)
{
	char absPath[128];
	getcwd(absPath, 128);
	m_bufPath = absPath;
	m_bufPath += ("/log/" + GetTimeStr() + ".log");
}

CLoggerServer::~CLoggerServer()
{
	Close();
}

int CLoggerServer::Start()
{
	if (m_socket) return -1;
	//log目录.专门用于日志文件的写入
	if (access("log", R_OK | W_OK) != 0)
	{
		//用户及用户组内的其他成员可读可写,其余只可读
		mkdir("log", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	}

	m_file = fopen(m_bufPath, "w+");
	if (!m_file) {
		printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
		return -2;
	}

	int ret = m_epoll.Create(1);
	ERR_RETURN(ret, -3);
	m_socket = new CLocalSocket();
	if (m_socket == nullptr) {
		Close();
		printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
		return -4;
	}
	ret = m_socket->InitSocket(CSockParam("./log/logger.sock", EnServer | EnNonBlock));
	if (ret != 0) {
		Close();
		printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
		return -5;
	}

	ret = m_epoll.Add(*m_socket, CEpoll_Data((void*)m_socket), EPOLLIN | EPOLLERR);
	if (ret != 0) {
		Close();
		printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
		return -6;
	}
	printf("%s(%d):<%s> epoll add server ret=%d server=%p server fd=%d\n", __FILE__, __LINE__, __FUNCTION__, ret, m_socket, (int)*m_socket);
	ret = m_thread.Start();
	if (ret != 0) {
		Close();
		printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
		return -7;
	}
	printf("%s(%d):<%s> logger server start finish!!!\n", __FILE__, __LINE__, __FUNCTION__, ret);
	return ret;
}

void CLoggerServer::Trace(const CBuffer& buff)
{
	static thread_local CLocalSocket client;
	int ret = -1;
	if (client == -1) {
		ret = client.InitSocket(CSockParam("./log/logger.sock", 0));
		if (ret != 0)
		{
			printf("%s(%d):<%s> ret = %d errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
			return;
		}
		ret = client.Link();
		if (ret != 0)
		{
			printf("%s(%d):<%s> ret = %d errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
			return;
		}
	}
	ret = client.Send(buff);
	if (ret != 0)
	{
		printf("%s(%d):<%s> ret = %d errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
		return;
	}
	printf("%s(%d):<%s> client send buff = %s\n", __FILE__, __LINE__, __FUNCTION__, ret == 0 ? "send succeed" : "send failed");
}

void CLoggerServer::WriteLog(const CBuffer& buff)
{
	if (m_file) {
		fwrite(buff, 1, buff.size(),m_file);
		fflush(m_file);
#ifdef _DEBUG
		printf("%s", data);
#endif // _DEBUG
	}
}

void CLoggerServer::Close()
{
	if (m_socket) {
		CSockBase* socket = m_socket;
		m_socket = nullptr;
		delete socket;
	}
	m_epoll.Close();
	m_thread.Stop();
}

int CLoggerServer::ThreadFunc()
{
	EPEVENTS events;
	printf("%s(%d):<%s> m_thread = %d\n", __FILE__, __LINE__, __FUNCTION__, m_thread.IsVaild() ? 1 : 0);
	printf("%s(%d):<%s> m_epoll = %d\n", __FILE__, __LINE__, __FUNCTION__, m_epoll == -1 ? 0 : 1);
	printf("%s(%d):<%s> m_socket = %d\n", __FILE__, __LINE__, __FUNCTION__, m_socket == nullptr ? 0 : 1);
	std::map<int, CSockBase*> mpClients;
	while (m_thread.IsVaild() && m_epoll != -1 && m_socket)
	{
		int ret = m_epoll.WaitEvents(events,1);
		if (ret < 0) {
			printf("%s(%d):<%s> ret = %d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, ret,errno,strerror(errno));
			continue;
		}
		if (ret > 0)
		{
			printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
			size_t i = 0;
			for (; i < events.size(); ++i)
			{
				if (events[i].events & EPOLLERR) {
					break;
				}
				else if (events[i].events & EPOLLIN)
				{
					if (events[i].data.ptr == m_socket)
					{
						//客户端接入
						CSockBase* client = nullptr;
						m_socket->Link(&client);
						if (client == nullptr) {
							printf("%s(%d):<%s> ret = %d errno=%d errmsg=%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
							continue;
						}
						ret = client->InitSocket(CSockParam("./log/logger.sock", 0));
						if (ret != 0) {
							printf("%s(%d):<%s> ret = %d errno=%d errmsg=%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
							if(client) delete client;
							continue;
						}
						printf("%s(%d):<%s> ret = %d server = %p server fd = %d,client = %p client socket = %d\n", __FILE__, __LINE__, __FUNCTION__, ret, m_socket, (int)*m_socket, client, (int)*client);
						ret = m_epoll.Add((int)*client, CEpoll_Data((void*)client), EPOLLIN | EPOLLERR);
						if (ret != 0) {
							printf("%s(%d):<%s> ret = %d errno=%d errmsg=%s\n", __FILE__, __LINE__, __FUNCTION__, ret, errno, strerror(errno));
							if (client) delete client;
							continue;
						}
						printf("%s(%d):<%s> epoll add client ret = %d \n", __FILE__, __LINE__, __FUNCTION__, ret);

						auto it = mpClients.find(*client);
						if (it != mpClients.end())
						{
							delete it->second;
							it->second = NULL;
						}
						mpClients[*client] = client;
						printf("%s(%d):<%s> update client cache ret = %d \n", __FILE__, __LINE__, __FUNCTION__, ret);
					}
					else {
						//数据传入
						printf("%s(%d):<%s> recv ret = %p\n", __FILE__, __LINE__, __FUNCTION__, events[i].data.ptr);
						CSockBase* client = (CLocalSocket*)events[i].data.ptr;
						if (client == nullptr)
						{
							break;
						}
						CBuffer bufRecv(1024 * 1024);
						printf("%s(%d):<%s> recv ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
						ret = client->Recv(bufRecv);
						printf("%s(%d):<%s> recv ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
						if (ret < 0)
						{
							auto it = mpClients.find(*client);
							if (it != mpClients.end())
							{
								delete it->second;
								it->second = NULL;
							}
						}
						else {
							printf("%s(%d):<%s> ret = %d recv content=%s\n", __FILE__, __LINE__, __FUNCTION__, ret, bufRecv);
							WriteLog(bufRecv);
						}
					}
				}
			}
			if (i != ret)
			{
				break;
			}
		}
	}
	for (auto iter = mpClients.begin(); iter != mpClients.end(); ++iter)
	{
		delete iter->second;
		iter->second = NULL;
	}
	mpClients.clear();
	return 0;
}

CBuffer CLoggerServer::GetTimeStr()
{
	CBuffer bufTime(128);
	timeb tmb;
	ftime(&tmb);
	tm* time = localtime(&tmb.time);

	//拼接时间字符串
	int nSize = snprintf(bufTime, bufTime.size(), 
		"%04d-02%d-%02d-%02d_%02d_%02d_%02d_%03d", 
		time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
		time->tm_hour, time->tm_min, time->tm_sec,
		tmb.millitm);
	bufTime.resize(nSize);
	return bufTime;
}