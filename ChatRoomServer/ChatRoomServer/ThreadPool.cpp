#include "ThreadPool.h"
#include <time.h>
#include <cstdio>


CThreadPool::CThreadPool():m_server(nullptr)
{
    //使用时间创建本地套接字
    timespec tspec{ 0,0 };
    clock_gettime(CLOCK_REALTIME, &tspec);
    char* buf = NULL;
    asprintf(&buf, "%d.%d.sock", tspec.tv_sec%100000, tspec.tv_nsec%1000000);
    if (buf) {
        m_bufSockPath = buf;
        free(buf);
    }
    usleep(1);
}

CThreadPool::~CThreadPool()
{
    Close();
}

int CThreadPool::Start(unsigned int nCount)
{
    m_epoll.Create(10);
    //TODO:
    m_server->InitSocket(CSockParam(m_bufSockPath, EnServer | EnNonBlock));

    m_epoll.Add(*m_server,CEpoll_Data((void*)m_server));

    m_vecThreads.resize(nCount);
    for (size_t i = 0; i < nCount; ++i) {
        m_vecThreads[i] = new CThread(&CThreadPool::DispatchTask, this);
        m_vecThreads[i]->Start();
    }

    return 0;
}

int CThreadPool::DispatchTask()
{
    while (1)
    {
        EPEVENTS epEvents;
        int ret = m_epoll.WaitEvents(epEvents);
        if (ret <= 0)
        {
            break;
        }
        //如果当前事件为客户端连接
        int i = 0;
        for (; i < ret; ++i)
        {
            if (epEvents[i].events & EPOLLERR)
            {
                break;
            }
            if (epEvents[i].events & EPOLLIN)
            {
                if (epEvents[i].data.ptr == m_server) {
                    //客户端接入
                    CSockBase* client = new CLocalSocket();
                    client->InitSocket(CSockParam(m_bufSockPath, 0));
                    client->Link();
                }
                else
                {
                    //数据传输
                    CSockBase* client = (CLocalSocket*)epEvents[i].data.ptr;
                    
                    CFunctionBase* base = nullptr;
                    CBuffer buffer(sizeof(CFunctionBase));
                    client->Recv(buffer);
                    memcpy(&base, buffer.c_str(), sizeof(CFunctionBase));
                    if (base) (*base)();
                    delete base;
                }
            }

        }
    }
    return 0;
}

int CThreadPool::Close()
{
    m_epoll.Close();
    if (m_server) {
        CSockBase* sock = m_server;
        delete sock;
    }
    unlink(m_bufSockPath);
    m_server = nullptr;
    return 0;
}

int CThreadPool::Size() const
{
    return m_vecThreads.size();
}