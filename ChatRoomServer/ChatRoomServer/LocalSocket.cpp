#include "LocalSocket.h"

#define MSS 4096

CLocalSocket::CLocalSocket(int fd)
{
    m_socket = fd;
    m_iStatus = 0;
}

int CLocalSocket::InitSocket(const CSockParam& param)
{
    if (m_iStatus != 0) return 1;
    if (m_socket != 0) {
        m_iStatus = 2;
        return 1;
    }
    int type = m_param.m_iAttr & EnTCP ? SOCK_STREAM : SOCK_DGRAM;
    int domain = m_param.m_iAttr & EnNetwork ? AF_INET : AF_LOCAL;
    m_socket = socket(domain, type, 0);
    if (-1 == m_socket)      return -1;

    m_param = param;

    if (m_param.m_iAttr & EnServer) {
        if (-1 == bind(m_socket, m_param.addrun(), sizeof(sockaddr_un)))
            return -2;
        if(-1 == listen(m_socket, 5))
            return -3;
    }
    m_iStatus = 1;
    return 0;
}

int CLocalSocket::Link(CSockBase** socket)
{
    if (m_param.m_iAttr & EnServer)
    {
        sockaddr_un addrClient;
        unsigned int iSockLen = sizeof(addrClient);
        int fd = accept(m_socket, (sockaddr*)&addrClient, &iSockLen);
        if (fd == -1)    return -1;
        *socket = new CLocalSocket(fd);
    }
    else {
        int iSockLen = sizeof(sockaddr_un);
        if (-1 == connect(m_socket, m_param.addrun(), iSockLen))
            return -2;
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
        index += iLen;
    }
    return 0;
}

int CLocalSocket::Recv(CBuffer& buf)
{
    //第一包收到数据长度
    CBuffer bufTotalLen(100);
    int recvLen = (int)::read(m_socket, (void*)bufTotalLen.c_str(), bufTotalLen.size());
    if (recvLen > 0) {
        buf.resize(recvLen);
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
}

CSockParam::CSockParam(const CSockParam& param)
{
    m_iAttr = param.m_iAttr;
    m_nPort = param.m_nPort;
    memcpy(&m_addrin, &param.m_addrin,sizeof(sockaddr_in));
    memcpy(&m_addrun, &param.m_addrun, sizeof(sockaddr_un));
    m_bufIp = param.m_bufIp;
}

CSockParam& CSockParam::operator=(const CSockParam& param)
{
    if (&param != this)
    {
        m_iAttr = param.m_iAttr;
        m_nPort = param.m_nPort;
        memcpy(&m_addrin, &param.m_addrin, sizeof(sockaddr_in));
        memcpy(&m_addrun, &param.m_addrun, sizeof(sockaddr_un));
        m_bufIp = param.m_bufIp;
    }
    return *this;
}

CSockParam::CSockParam(const CBuffer& buf, short nPort)
{
    m_nPort = nPort;
}

CSockParam::~CSockParam()
{
}
