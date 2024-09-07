#include "Server.h"


//TODO:
CServer::CServer()
{
}

CServer::~CServer()
{
}

int CServer::Init(CBussiness* bussiness, CBuffer bufIP, short port)
{
    //创建客户端处理进程
    m_bussiness = bussiness;
    m_process.SetEntryFunction(&CBussiness::BusinessProcess, m_bussiness, &m_process);
    m_process.CreateSubProcess();

    m_pool.Start(2);
    m_epoll.Create(2);

    //创建网络服务器
    m_server = new CLocalSocket();
    m_server->InitSocket(CSockParam(bufIP,port,EnNetwork | EnServer));
    m_epoll.Add(*m_server, CEpoll_Data(m_server), EPOLLIN);

    for (int i = 0; i < m_pool.Size(); ++i)
    {
        m_pool.AddTask(&CServer::ThreadFunc, this);
    }

    return 0;
}

int CServer::Run()
{
    while (m_server)
    {
        usleep(1);
    }
    return 0;
}

int CServer::Close()
{
    return 0;
}

int CServer::ThreadFunc()
{
    while (m_server)
    {
        EPEVENTS epEvents;
        ssize_t ret = m_epoll.WaitEvents(epEvents);
        if (ret < 0) break;
        else
        {
            for (ssize_t i = 0; i < ret; ++i)
            {
                if (epEvents[i].events & EPOLLERR)
                {
                    break;
                }
                else if(epEvents[i].events & EPOLLIN)
                {
                    if (epEvents[i].data.ptr = m_server)
                    {
                        CSockBase* client = nullptr;
                        m_server->Link(&client);
                        m_process.SendFD(*client);
                    }
                }
            }
        }
    }
    return 0;
}
