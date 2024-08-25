#include "LoggerServer.h"
#include <unordered_map>
#include <cstddef>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "LocalSocket.h"


#ifndef ERR_RETURN(err,ret) 
#define ERR_RETURN(err,ret) if(err == 0) return ret; 
#endif

CLoggerServer::CLoggerServer()
	: m_thread(&CLoggerServer::ThreadFunc, this)
	,m_socket(nullptr)
{
	//TODO:
	char absPath[128];
	getcwd(absPath, 128);

	m_bufPath = CBuffer(absPath) + CBuffer("/log/") + GetTimeStr() + CBuffer(".log");
}

CLoggerServer::~CLoggerServer()
{
	Close();
}

int CLoggerServer::Start()
{
	//log目录.专门用于日志文件的写入
	if (access("log", R_OK | W_OK) != 0)
	{
		//用户及用户组内的其他成员可读可写,其余只可读
		mkdir("log", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	}

	fopen(m_bufPath, "wr+");

	int ret = m_epoll.Create(2);
	ret = m_socket->InitSocket(CSockParam("./logger.sock", EnTCP | EnServer | EnNonBlock));
	ret = m_epoll.Add(*m_socket, CEpoll_Data(m_socket),EPOLLIN | EPOLLERR);
	ret = m_thread.Start();

	return 0;
}

void CLoggerServer::Trace(const CBuffer& buff)
{
	static thread_local CLocalSocket client;
	int ret = -1;
	if (client == -1) {
		ret = client.InitSocket(CSockParam("./logger.sock",0));
		ret = client.Link();
	}
	ret = client.Send(buff);
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
		delete m_socket;
	}
	m_epoll.Close();
	m_thread.Stop();
	fclose(m_file);
}

void CLoggerServer::ThreadFunc(void* args)
{
	EPEVENTS events;
	static std::unordered_map<int, CSockBase*> mpClients;
	while (1)
	{
		int ret = m_epoll.WaitEvents(events);
		if (ret < 0) continue;
		if (ret > 0)
		{
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
						CSockBase* client = new CLocalSocket();
						m_socket->Link(&client);
						ret = client->InitSocket(CSockParam("./log/server.sock",0));
						if (ret != 0) continue;
						m_epoll.Add(*client, CEpoll_Data((void*)client), EPOLLIN | EPOLLERR);
						auto it = mpClients.find(*client);
						if (it != mpClients.end())
						{
							delete it->second;
							it->second = NULL;
							it->second = client;
						}
						else {
							mpClients.insert({ *client,client });
						}
					}
					else {
						//数据传入
						CSockBase* client = (CLocalSocket*)events[i].data.ptr;
						CBuffer bufRecv(2048);
						client->Recv(bufRecv);
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
}

CBuffer CLoggerServer::GetTimeStr()
{
	CBuffer bufTime(128);
	timeb tmb;
	ftime(&tmb);
	tm* time = localtime(&tmb.time);

	//拼接时间字符串
	int nSize = snprintf(bufTime, bufTime.size(), 
		"%04d-02%d-%02d-%02d %02d:%02d:%02d.%03d", 
		time->tm_year, time->tm_mon, time->tm_mday,
		time->tm_hour, time->tm_min, time->tm_sec,
		tmb.millitm);
	bufTime.resize(nSize);
	return bufTime;
}