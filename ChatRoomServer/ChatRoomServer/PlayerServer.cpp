#include "PlayerServer.h"
#include <memory.h>
using namespace std;
using namespace std::placeholders;

CPlayerServer::CPlayerServer(unsigned nSize):
	m_iCount(nSize)
{
	
}

CPlayerServer::~CPlayerServer()
{
}

int CPlayerServer::BusinessProcess(CProcess* process)
{
	m_epoll.Create(m_iCount);
	m_pool.Start(m_iCount);

	for (unsigned i = 0; i < m_iCount; ++i)
	{
		m_pool.AddTask(&CPlayerServer::ThreadFunc, this);
	}
	SetRecvFunction(&CPlayerServer::Recv, this, std::placeholders::_1, std::placeholders::_2);
	SetConnFunction(&CPlayerServer::Conn, this, std::placeholders::_1);
	
	return 0;
}

int CPlayerServer::ThreadFunc()
{
	int ret = 0;
	while (1)
	{
		EPEVENTS epEvents;
		ret = m_epoll.WaitEvents(epEvents);
		if (ret < 0) break;
		if (ret > 0)
		{
			for (int i = 0; i < ret; ++i)
			{
				if (epEvents[i].events & EPOLLERR) break;
				if (epEvents[i].events & EPOLLIN)
				{
					//TODO:
					CSockBase* client = (CSockBase*)epEvents[i].data.ptr;
					CBuffer buffer;
					client->Recv(buffer);
					if (m_funcRecv)
						(*m_funcRecv)(client, buffer);
				}
			}
		}
	}
	return 0;
}

int CPlayerServer::Conn(CSockBase* client)
{
	return 0;
}

int CPlayerServer::Recv(CSockBase* client, const CBuffer& buffer)
{
	return 0;
}