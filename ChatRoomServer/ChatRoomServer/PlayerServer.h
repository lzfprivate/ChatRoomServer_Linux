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
    //解析客户端的http请求
    int DecodeHttpRequest(const CBuffer& request);
    //组织数据回复
    CBuffer MakeHttpResponse(int ret);

    //获取文件夹下的文件
    std::vector<CBuffer> GetFilesInFolder(const CBuffer& folderPath);

    //处理登录请求
    int DealLoginRequest(CSockBase* client,const CBuffer& buff);


private:
    CEpoll      m_epoll;
    CThreadPool m_pool;
    std::map<int, CSockBase*> m_mpClients;
    unsigned    m_iCount;
    CDataBaseClient* m_db;
};

