#include "LocalSocket.h"
#include <string.h>
#include <fcntl.h>

#define MSS 4096

CLocalSocket::CLocalSocket()
{
    m_socket = -1;
    m_iStatus = 0;
    memset(&m_param, 0, sizeof(CSockParam));
}

CLocalSocket::CLocalSocket(int fd)
{
    m_socket = fd;
    m_iStatus = 0;
}

int CLocalSocket::InitSocket(const CSockParam& param)
{
    if (m_iStatus != 0) return -1;      //当前状态不是未初始化
    m_param = param;
    if (m_socket == -1) {               //如果当前套接字句柄不是-1，已经初始化完成
        int domain = (m_param.m_iAttr & EnNetwork) ? PF_INET : PF_LOCAL;
        int type = (m_param.m_iAttr & EnUDP) ? SOCK_DGRAM : SOCK_STREAM;
        m_socket = socket(domain, type, 0);
    }
    else {
        m_iStatus = 2;
    }
    
    if (-1 == m_socket)
    {
        Close();
        printf("%s(%d):<%s> errno = %d  errmsg= %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
        return -1;
    }

    if (m_param.m_iAttr & EnServer) {
        if (-1 == bind(m_socket, m_param.addrun(), sizeof(sockaddr_un)))
        {
            Close();
            printf("%s(%d):<%s> path= %s\n", __FILE__, __LINE__, __FUNCTION__, m_param.m_bufIp.c_str());
            printf("%s(%d):<%s> errno = %d  errmsg= %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
            return -2;
        }
        if (-1 == listen(m_socket, 10))
        {
            Close();
            printf("%s(%d):<%s> errno = %d  errmsg= %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
            return -3;
        }
    }
    if (m_param.m_iAttr & EnNonBlock)
    {
        int iFlag = fcntl(m_socket, F_GETFL);
        iFlag |= O_NONBLOCK;
        if (-1 == fcntl(m_socket, F_SETFL, iFlag))
            return -4;
    }
    if(m_iStatus == 0) m_iStatus = 1;
    return 0;
}

int CLocalSocket::Link(CSockBase** socket)
{
    if (m_iStatus == 0) return -1;
    if (m_param.m_iAttr & EnServer)
    {
        int fd = 0;
        if (m_param.m_iAttr & EnNetwork)
        {
            sockaddr_in addrClient;
            unsigned int iSockLen = sizeof(addrClient);
            fd = accept(m_socket, (sockaddr*)&addrClient, &iSockLen);
            if (fd == -1)
            {
                printf("%s(%d):<%s> pid=%d ret = %d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), fd, errno, strerror(errno));
                return -1;
            }
        }
        else
        {
            sockaddr_un addrClient;
            unsigned int iSockLen = sizeof(addrClient);
            fd = accept(m_socket, (sockaddr*)&addrClient, &iSockLen);
            if (fd == -1)
            {
                printf("%s(%d):<%s> pid=%d ret = %d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), fd, errno, strerror(errno));
                return -1;
            }
        }
        *socket = new CLocalSocket(fd);
    }
    else {
        int iSockLen = sizeof(sockaddr_un);
        if (-1 == connect(m_socket, m_param.addrun(), iSockLen))
        {
            printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
            return -2;
        }
    }
    m_iStatus = 2;
    return 0;
}

int CLocalSocket::Send(const CBuffer& buf)
{
    int index = 0;
    int iLen = 0;
    while (index < (int)buf.size())
    {
        iLen = (int)write(m_socket, buf.c_str(), buf.size() - index);
        if (iLen == -1)
        {
            printf("%s(%d):<%s> pid=%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno));
            return -1;
        }
        index += iLen;
    }
    return 0;
}

int CLocalSocket::Recv(CBuffer& buf)
{
    printf("recv buf entry===================================\n");
    //第一包收到数据长度
    CBuffer bufTotalLen(1024 * 1024);
    int recvLen = (int)::read(m_socket, (void*)bufTotalLen.c_str(), bufTotalLen.size());
    if (recvLen > 0) {
        buf.resize(recvLen);
        printf("recv buf:%s\n", bufTotalLen.c_str());
        return recvLen;
    }
    else {
        if (errno == EINTR || errno == EAGAIN) {
            buf.clear();
            return 0;
        }
        return -2;
    }
    return -3;
}

CSockParam::CSockParam()
{
    m_iAttr = 0;
    m_nPort = 0;
    bzero(&m_addrin, sizeof(sockaddr_in));
    bzero(&m_addrun, sizeof(sockaddr_un));
}

CSockParam::CSockParam(const CSockParam& param)
{
    m_bufIp = param.m_bufIp;
    m_iAttr = param.m_iAttr;
    m_nPort = param.m_nPort;
    memcpy(&m_addrin, &param.m_addrin, sizeof(sockaddr_in));
    memcpy(&m_addrun, &param.m_addrun, sizeof(sockaddr_un));
}

CSockParam& CSockParam::operator=(const CSockParam& param)
{
    if (&param != this)
    {
        m_bufIp = param.m_bufIp;
        m_iAttr = param.m_iAttr;
        m_nPort = param.m_nPort;
        memcpy(&m_addrin, &param.m_addrin, sizeof(sockaddr_in));
        memcpy(&m_addrun, &param.m_addrun, sizeof(sockaddr_un));
    }
    return *this;
}

CSockParam::CSockParam(const CBuffer& bufIP, short nPort, int attr)
{
    m_bufIp = bufIP;
    m_nPort = nPort;
    m_addrin.sin_family = AF_INET;
    m_addrin.sin_addr.s_addr = inet_addr(bufIP.c_str());
    m_addrin.sin_port = nPort;
    m_iAttr = attr;
}

CSockParam::CSockParam(const CBuffer& buf, int attr)
{
    m_bufIp = buf;
    m_addrun.sun_family = AF_UNIX;
    //strncpy(m_addrun.sun_path, buf.c_str(), buf.size());
    strcpy(m_addrun.sun_path, buf.c_str());
    m_iAttr = attr;
}

CSockParam::~CSockParam()
{
}

CSocket::CSocket()
{
}

CSocket::CSocket(int fd)
{
}

int CSocket::InitSocket(const CSockParam& param)
{
    return 0;
}

int CSocket::Link(CSockBase** socket)
{
    return 0;
}

int CSocket::Send(const CBuffer& buf)
{
    return 0;
}

int CSocket::Recv(CBuffer& buf)
{
    return 0;
}
