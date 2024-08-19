#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "Buffer.h"

enum EnSockFlag
{
	EnNone = 0,
	EnTCP = 1,
	EnServer = 2,
	EnNetwork = 4
};

class CSockParam 
{
public:
	CSockParam();
	CSockParam(const CSockParam& param);
	CSockParam& operator=(const CSockParam& param);
	CSockParam(const CBuffer& buf,short nPort = 9876);
	~CSockParam();

	sockaddr* addrin() {
		return (sockaddr*)&m_addrin;
	}

	sockaddr* addrun() {
		return (sockaddr*)&m_addrun;
	}

private:
	sockaddr_in m_addrin;			//�����ַ
	sockaddr_un m_addrun;			//���������ַ

	CBuffer		m_bufIp;			//���������ͨ��,ΪIP�ַ���,����Ǳ���ͨ��,Ϊ�����ļ�
	short		m_nPort;			//�˿�			


public:
	int		m_iAttr;			//ͨ�ŵ��������
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
	//��ʼ�� �����׽��� �� ���� 
	virtual int InitSocket(const CSockParam& param) = 0;
	//���� ����˽��տͻ������� �ͻ��������ӷ����
	virtual int Link(CSockBase** socket = nullptr) = 0;
	//����
	virtual int Send(const CBuffer& buf) = 0;
	//����
	virtual int Recv(CBuffer& buf) = 0;

	void Close() {
		if (m_socket)
		{
			int fd = m_socket;
			m_socket = -1;
			close(fd);
		}
	}

protected:
	int		m_socket;			//�׽���������
	int		m_iStatus;			//0:δ��ʼ�� 1:��ʼ����� 2:������ 3:�ر�
	CSockParam	m_param;		//����ͨ�Ų�������
};



class CLocalSocket : public CSockBase
{
public:
	CLocalSocket(int fd);
public:
	/// <summary>
	///  ��ʼ�� �����׽��� �� ���� 
	/// </summary>
	/// <param name="param">����ͨ����ز���</param>
	/// <returns>0:��ʼ���ɹ� 1:�ѳ�ʼ ����:��ʼ��ʧ��</returns>
	virtual int InitSocket(const CSockParam& param) override;
	/// <summary>
	/// ���� 
	/// 1:����˽��տͻ������� 
	/// 2:�ͻ��������ӷ����
	/// </summary>
	/// <param name="socket">����ǿͻ��� ������Ч
	/// ����Ƿ����,Ϊ�ͻ��˶���</param>
	/// <returns>0:�ɹ� ����:ʧ��</returns>
	virtual int Link(CSockBase** socket = nullptr) override;
	/// <summary>
	/// ���� 
	/// </summary>
	/// <param name="buf">�����͵�����</param>
	/// <returns>0:�ɹ� ����ʧ��</returns>
	virtual int Send(const CBuffer& buf) override;
	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="buf">���յ�������</param>
	/// <returns>0:������� 1:����ʧ��</returns>
	virtual int Recv(CBuffer& buf) override;

};

