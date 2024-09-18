#pragma once
#include <map>
#include "Bussiness.h"
#include "ThreadPool.h"
#include "DataBase.h"
#include <vector>

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
    //�����ͻ��˵�http����
    int DecodeHttpRequest(const CBuffer& request);
    //��֯���ݻظ�
    CBuffer MakeHttpResponse(int ret);


    std::vector<CBuffer> GetFilesInFolder(const CBuffer& folderPath);


private:
    CEpoll      m_epoll;
    CThreadPool m_pool;
    std::map<int, CSockBase*> m_mpClients;
    unsigned    m_iCount;
    CDataBaseClient* m_db;
};

