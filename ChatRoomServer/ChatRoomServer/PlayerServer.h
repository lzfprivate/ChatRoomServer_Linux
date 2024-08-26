#pragma once
#include <map>
#include "Bussiness.h"
#include "ThreadPool.h"
class CPlayerServer :
    public CBussiness
{
public:
    CPlayerServer(unsigned nSize);
    ~CPlayerServer();
    
    CPlayerServer(const CPlayerServer&) = delete;
    CPlayerServer operator=(const CPlayerServer&) = delete;

public:
    int BusinessProcess(CProcess* process) override;

private:
    int ThreadFunc();
    int Conn(CSockBase* client);
    int Recv(CSockBase* client,const CBuffer& buffer);

private:
    CEpoll      m_epoll;
    CThreadPool m_pool;
    std::map<int, CSockBase*> m_mpClients;
    unsigned    m_iCount;

};

