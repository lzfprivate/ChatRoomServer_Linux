#pragma once
#include "Process.h"
#include "Bussiness.h"
#include "ThreadPool.h"
/// <summary>
/// ��ģ��
/// ������־����������
/// �����ͻ��˴������
/// �������������
/// </summary>
class CServer
{
public:
	CServer();
	~CServer();
	//��ʼ��
	int Init(CBussiness* bussiness, CBuffer bufIP, short port);
	//����
	int Run();
	//�ر�
	int Close();

private:
	//�̻߳ص�����
	int ThreadFunc();


private:
	CProcess	m_process;
	CBussiness* m_bussiness;
	CSockBase* m_server;
	CEpoll		m_epoll;
	CThreadPool m_pool;

};

