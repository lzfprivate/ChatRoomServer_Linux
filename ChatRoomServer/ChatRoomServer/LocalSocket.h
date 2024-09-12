#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Buffer.h"

enum EnSockFlag
{
	EnNone = 0,
	EnTCP = 1,
	EnServer = 2,			//服务端标识
	EnNetwork = 4,			//网络套接字标识
	EnNonBlock = 8			//阻塞标识
};

class CSockParam 
{
public:
	CSockParam();
	CSockParam(const CSockParam& param);
	CSockParam& operator=(const CSockParam& param);
	CSockParam(const CBuffer& bufIP, short nPort,int attr);
	CSockParam(const CBuffer& buf,  int attr);
	~CSockParam();

public:
	sockaddr* addrin() {
		return (sockaddr*)&m_addrin;
	}

	sockaddr* addrun() {
		return (sockaddr*)&m_addrun;
	}

	operator CBuffer() {
		return m_bufIp;
	}
	operator const char* () {
		return m_bufIp.c_str();
	}

public:
	sockaddr_in m_addrin;			//网络地址
	sockaddr_un m_addrun;			//本地网络地址

	CBuffer		m_bufIp;			//如果是网络通信,为IP字符串,如果是本地通信,为本地文件
	short		m_nPort;			//端口			

public:
	int		m_iAttr;			//通信的相关属性
};


class CSockBase {
public:
	CSockBase() : m_socket(-1) {

	}
	virtual ~CSockBase()
	{
		Close();
	}

public:
	//初始化 建立套接字 绑定 监听 
	virtual int InitSocket(const CSockParam& param) = 0;
	//连接 服务端接收客户端连接 客户主动连接服务端
	virtual int Link(CSockBase** socket = nullptr) = 0;
	//发送
	virtual int Send(const CBuffer& buf) = 0;
	//接收
	virtual int Recv(CBuffer& buf) = 0;

	virtual void Close() {
		if (m_socket)
		{
			int fd = m_socket;
			m_socket = -1;
			close(fd);
		}
		if (m_param.m_iAttr & EnServer) {
			unlink(m_param);
		}
	}

	operator int() const {
		return m_socket;
	}

protected:
	int		m_socket;			//套接字描述符
	int		m_iStatus;			//0:未初始化 1:初始化完成 2:已连接 3:关闭
	CSockParam	m_param;		//网络通信参数集合
};



class CLocalSocket : public CSockBase
{
public:
	CLocalSocket();
	CLocalSocket(int fd);
public:
	/// <summary>
	///  初始化 建立套接字 绑定 监听 
	/// </summary>
	/// <param name="param">网络通信相关参数</param>
	/// <returns>0:初始化成功 1:已初始 其他:初始化失败</returns>
	virtual int InitSocket(const CSockParam& param) override;
	/// <summary>
	/// 连接 
	/// 1:服务端接收客户端连接 
	/// 2:客户主动连接服务端
	/// </summary>
	/// <param name="socket">如果是客户端 参数无效
	/// 如果是服务端,为客户端对象</param>
	/// <returns>0:成功 其他:失败</returns>
	virtual int Link(CSockBase** socket = nullptr) override;
	/// <summary>
	/// 发送 
	/// </summary>
	/// <param name="buf">待发送的数据</param>
	/// <returns>0:成功 其他失败</returns>
	virtual int Send(const CBuffer& buf) override;
	/// <summary>
	/// 接收数据
	/// </summary>
	/// <param name="buf">接收到的数据</param>
	/// <returns>0:接收完成 1:接收失败</returns>
	virtual int Recv(CBuffer& buf) override;

	virtual void Close() {
		CSockBase::Close();
	}
};

class CSocket : public CSockBase
{
public:
	CSocket();
	CSocket(int fd);
public:
	/// <summary>
	///  初始化 建立套接字 绑定 监听 
	/// </summary>
	/// <param name="param">网络通信相关参数</param>
	/// <returns>0:初始化成功 1:已初始 其他:初始化失败</returns>
	virtual int InitSocket(const CSockParam& param) override;
	/// <summary>
	/// 连接 
	/// 1:服务端接收客户端连接 
	/// 2:客户主动连接服务端
	/// </summary>
	/// <param name="socket">如果是客户端 参数无效
	/// 如果是服务端,为客户端对象</param>
	/// <returns>0:成功 其他:失败</returns>
	virtual int Link(CSockBase** socket = nullptr) override;
	/// <summary>
	/// 发送 
	/// </summary>
	/// <param name="buf">待发送的数据</param>
	/// <returns>0:成功 其他失败</returns>
	virtual int Send(const CBuffer& buf) override;
	/// <summary>
	/// 接收数据
	/// </summary>
	/// <param name="buf">接收到的数据</param>
	/// <returns>0:接收完成 1:接收失败</returns>
	virtual int Recv(CBuffer& buf) override;
};

