#pragma once
/// <summary>
/// �̳߳���:���ڶ��߳����ݴ���
/// 
/// 
/// 
/// 
/// </summary>
#include <vector>
#include <queue>
#include <condition_variable>
#include "Epoll.h"
#include "Thread.h"
#include "FuncionBase.h"
#include "LocalSocket.h"
#include "Buffer.h"

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();
	CThreadPool& operator=(const CThreadPool& ) = delete;
	CThreadPool(const CThreadPool&) = delete;
	//����
	int Start(unsigned int nCount = 10);
	//�������
	template <typename _FUNCTION_ ,typename ..._ARGS_>
	int AddTask(_FUNCTION_ func, _ARGS_... args) {
		static thread_local CLocalSocket client;
		client.InitSocket(CSockParam(m_bufSockPath, 0));
		client.Link();

		CFunctionBase* base = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
		CBuffer funcName(1024);
		memcpy((void*)funcName.c_str(), base, sizeof(CFunctionBase));
		client.Send(funcName);
		return 0;
	}
	//���зַ�����
	int DispatchTask();
	//�ر�
	int Close();
	//��ȡ�̸߳���
	int Size() const;


private:
	CEpoll		m_epoll;					

	std::vector<CThread*> m_vecThreads;

	CSockBase* m_server;
	CBuffer		m_bufSockPath;

};


/// <summary>
/// ����ҵ��������̳߳�
///	����˼��:
/// 1.�����������������
/// 2.�̳߳��еĿ����߳�ȥ���������л�ȡ����ִ��
/// 3.ִ����ɺ�,��ȥ�����л�ȡ����ִ��
/// 
/// ����:
/// ������У�				
/// �����̣߳�	������-�������е�������
/// �������̣߳����ڹ����̵߳���Ӻ��ͷ�
///
/// 
/// </summary>
/// 

using cbTask = void (*)(void* args);
//������е�����ṹ
template <typename T>
class Task {
public:
	Task() :m_function(nullptr), m_args(nullptr) {}
	Task(cbTask f, void* args) 
	{
		m_function = f;
		m_args = (T*)args;
	}
public:
	cbTask m_function;
	T* m_args;
};
//�������
template<typename T>
class TaskQueue
{
public:
	TaskQueue() {
		pthread_mutex_init(&m_mutex, nullptr);
	}
	~TaskQueue() {
		pthread_mutex_destroy(&m_mutex);
	}
	//�������
	
	int AddTask(const Task<T>& task) {
		pthread_mutex_lock(&m_mutex);
		m_queueTask.push(task);
		pthread_mutex_unlock(&m_mutex);
		return 0;
	}
	int AddTask(cbTask f, T* args) {
		Task<T> task(f, args);
		pthread_mutex_lock(&m_mutex);
		m_queueTask.push(task);
		pthread_mutex_unlock(&m_mutex);
		return 0;
	}
	//�Ӷ�����ȡ������
	Task<T> TakeTask() {
		Task<T> task;
		pthread_mutex_lock(&m_mutex);
		if (!m_queueTask.empty())
		{
			task = m_queueTask.front();
			m_queueTask.pop();
		}
		pthread_mutex_unlock(&m_mutex);
		return task;
	}
	//��ȡ�������
	int GetTaskSize() {
		int iSize = 0;
		pthread_mutex_lock(&m_mutex);
		iSize = m_queueTask.size();
		pthread_mutex_unlock(&m_mutex);
		return iSize;
	}

private:
	std::queue<Task<T> > m_queueTask;		//�������

	pthread_mutex_t		m_mutex;		
};


template<typename T>
class CCommonThreadPool {
public:
	CCommonThreadPool(size_t min, size_t max) {
		do
		{
			m_iMinNum = min;
			m_iMaxNum = max;
			m_workThreadList.resize(max);
			m_iBusyNum = 0;
			m_iLiveNum = min;
			m_iRemoveNum = 0;
			m_iShutdown = 0;

			pthread_mutex_init(&m_mutexTask, nullptr);
			pthread_cond_init(&m_condNotEmpty, nullptr);
			return;
		} while (0);
		Exit();
	}
	~CCommonThreadPool() {
		Exit();
	}
	CCommonThreadPool operator=(const CCommonThreadPool&) = delete;
	CCommonThreadPool(const CCommonThreadPool&) = delete;
	CCommonThreadPool(CCommonThreadPool&&) = delete;

	int Create() {
		pthread_mutex_lock(&m_mutexTask);
		pthread_create(&m_pidManager, nullptr, &CCommonThreadPool::managerFunEntry, this);
		for (int i = 0; i < m_iMaxNum; ++i)
		{
			pthread_create(&m_workThreadList[i], nullptr,
				&CCommonThreadPool::workerFunEntry, this);
		}
		pthread_mutex_unlock(&m_mutexTask);
		return 0;
	}
	int AddTask(const Task<T>& task) {
		//�˴�����Ҫ����������������ڲ���������
		m_queueTask.AddTask(task);
		return 0;
	}
	int AddTask(void (*function)(void*), T* args) {
		pthread_mutex_lock(&m_mutexTask);
		if (m_iShutdown)
		{
			pthread_mutex_unlock(&m_mutexTask);
			return -1;
		}
		m_queueTask.AddTask(function, args);
		pthread_mutex_unlock(&m_mutexTask);
		pthread_cond_signal(&m_condNotEmpty);
		return 0;
	}
	int Exit() {
		m_iShutdown = 1;
		pthread_join(m_pidManager, nullptr);
		pthread_mutex_lock(&m_mutexTask);
		for (int i = 0; i < m_iLiveNum; ++i)
		{
			pthread_cond_signal(&m_condNotEmpty);
		}
		pthread_mutex_unlock(&m_mutexTask);

		pthread_mutex_destroy(&m_mutexTask);
		pthread_cond_destroy(&m_condNotEmpty);
		return 0;
	}

	//��ȡ����ִ��
	int GetBusyThreadCount() {
		pthread_mutex_lock(&m_mutexTask);
		int iBusyNum = m_iBusyNum;
		pthread_mutex_unlock(&m_mutexTask);
		return iBusyNum;
	}
	//��ȡ���е�
	int GetLiveThreadCount() {
		pthread_mutex_lock(&m_mutexTask);
		int iLiveNum = m_iLiveNum;
		pthread_mutex_unlock(&m_mutexTask);
		return iLiveNum;
	}

private:
	//�����̵߳Ļص�����
	static void* workerFunEntry(void* args) {
		CCommonThreadPool* pool = (CCommonThreadPool*)args;
		pool->workerFun();
		return nullptr;
	}
	//�������̻߳ص�
	static void* managerFunEntry(void* args) {
		CCommonThreadPool* pool = (CCommonThreadPool*)args;
		pool->managerFun();
		return nullptr;
	}
	//�����̵߳Ļص�����
	void workerFun() {
		while (1)
		{
			//ȡ����������е�����ִ��
			pthread_mutex_lock(&m_mutexTask);

			while (m_queueTask.GetTaskSize() == 0 || m_iShutdown != 0)
			{
				pthread_cond_wait(&m_condNotEmpty, &m_mutexTask);
				if (m_iRemoveNum > 0)
				{
					m_iRemoveNum--;
					if (m_iMinNum >= m_iLiveNum)
					{
						m_iLiveNum--;
						pthread_mutex_unlock(&m_mutexTask);
						ThreadExit();
					}
				}
			};
			if (m_iShutdown == 1)
			{
				pthread_mutex_unlock(&m_mutexTask);
				ThreadExit();
			}
			//�Ӷ�ͷȡ��
			Task<T> task = m_queueTask.TakeTask();
			m_iBusyNum--;
			pthread_mutex_unlock(&m_mutexTask);

			//(*task.function)(task.args);
			task.m_function(task.m_args); //ִ�к���
			//�ͷſռ� 
			delete task.m_args;

			pthread_mutex_lock(&m_mutexTask);
			m_iBusyNum--;
			pthread_mutex_unlock(&m_mutexTask);
		}
	}
	//�������̻߳ص�
	void managerFun() {
		//ÿ��һ��ʱ�䣬��⵱ǰ���е��߳�����ִ�е��߳�����
	//������е��߳���Զ��������ִ�е��߳�����ֹͣһЩ��ת���̣߳�
	//�������Ƴ�����
		while (!m_iShutdown)
		{
			sleep(3);
			pthread_mutex_lock(&m_mutexTask);
			//ȡ������ĸ���
			//ȡ���������е��̸߳���
			//ȡ������ִ�е��̸߳���
			int iTaskNum = m_queueTask.GetTaskSize();
			int iExecNum = m_iBusyNum;
			int iRunNum = m_iLiveNum;
			pthread_mutex_unlock(&m_mutexTask);

			//������� > �����̸߳��� && С������̸߳��� �����߳�
			//�����̸߳��� * 2 < �����̸߳��� && ������С�̸߳��� ɾ���߳�
			if (iExecNum * 2 < iRunNum)
			{
				pthread_mutex_lock(&m_mutexTask);
				m_iRemoveNum = SIZE;
				pthread_mutex_unlock(&m_mutexTask);
				for (int i = 0; i < SIZE; ++i)
				{
					pthread_cond_signal(&m_condNotEmpty);
				}
			}

		}
	}
	//�߳��˳�
	void ThreadExit() {
		pthread_t pid = pthread_self();
		for (int i = 0; i < m_workThreadList.size(); ++i)
		{
			if (m_workThreadList[i] == pid)
			{
				m_workThreadList[i] = 0;
			}
		}
		pthread_exit(NULL);
	}
private:
	//�������
	TaskQueue<T>				m_queueTask;	//������� �������Ժ�������ʽ���ڣ����Ա�����Ǻ����ĵ�ַ
	//�������߳� 
	pthread_t	m_pidManager;		//�������߳�ID

	//�����߳�
	std::vector<pthread_t> m_workThreadList;	//�����߳�ID
	pthread_mutex_t m_mutexTask;		//��������е���


	int m_iMaxNum;		//�߳������
	int m_iMinNum;		//�߳���С��
	int m_iBusyNum;		//����ִ��������߳���
	int m_iLiveNum;		//�������е��߳������������У���������ִ�������п����ڿ�ת��
	int m_iRemoveNum;	//��ɾ�����߳���������߳̿�ת������һ���������߳�ɾ����

	pthread_cond_t  m_condNotEmpty;		//�ӿ�����

	int				m_iShutdown;		//�Ƿ������̳߳�

	static const int SIZE = 2;

};

