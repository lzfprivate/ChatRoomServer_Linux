#pragma once
/// <summary>
/// 线程池类:用于多线程数据处理
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
	//开启
	int Start(unsigned int nCount = 10);
	//添加任务
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
	//队列分发任务
	int DispatchTask();
	//关闭
	int Close();
	//获取线程个数
	int Size() const;


private:
	CEpoll		m_epoll;					

	std::vector<CThread*> m_vecThreads;

	CSockBase* m_server;
	CBuffer		m_bufSockPath;

};


/// <summary>
/// 不与业务关联的线程池
///	核心思想:
/// 1.将任务放入阻塞队列
/// 2.线程池中的空闲线程去阻塞队列中获取任务执行
/// 3.执行完成后,再去队列中获取任务执行
/// 
/// 特性:
/// 任务队列：				
/// 工作线程：	生产者-消费者中的消费者
/// 管理者线程：用于管理线程的添加和释放
///
/// 
/// </summary>
/// 

using cbTask = void (*)(void* args);
//任务队列的任务结构
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
//任务队列
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
	//添加任务
	
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
	//从队列中取出任务
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
	//获取任务个数
	int GetTaskSize() {
		int iSize = 0;
		pthread_mutex_lock(&m_mutex);
		iSize = m_queueTask.size();
		pthread_mutex_unlock(&m_mutex);
		return iSize;
	}

private:
	std::queue<Task<T> > m_queueTask;		//任务队列

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
		//此处不需要加锁，在任务队列内部有锁机制
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

	//获取正在执行
	int GetBusyThreadCount() {
		pthread_mutex_lock(&m_mutexTask);
		int iBusyNum = m_iBusyNum;
		pthread_mutex_unlock(&m_mutexTask);
		return iBusyNum;
	}
	//获取运行的
	int GetLiveThreadCount() {
		pthread_mutex_lock(&m_mutexTask);
		int iLiveNum = m_iLiveNum;
		pthread_mutex_unlock(&m_mutexTask);
		return iLiveNum;
	}

private:
	//工作线程的回调函数
	static void* workerFunEntry(void* args) {
		CCommonThreadPool* pool = (CCommonThreadPool*)args;
		pool->workerFun();
		return nullptr;
	}
	//管理者线程回调
	static void* managerFunEntry(void* args) {
		CCommonThreadPool* pool = (CCommonThreadPool*)args;
		pool->managerFun();
		return nullptr;
	}
	//工作线程的回调函数
	void workerFun() {
		while (1)
		{
			//取出任务队列中的任务执行
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
			//从队头取出
			Task<T> task = m_queueTask.TakeTask();
			m_iBusyNum--;
			pthread_mutex_unlock(&m_mutexTask);

			//(*task.function)(task.args);
			task.m_function(task.m_args); //执行函数
			//释放空间 
			delete task.m_args;

			pthread_mutex_lock(&m_mutexTask);
			m_iBusyNum--;
			pthread_mutex_unlock(&m_mutexTask);
		}
	}
	//管理者线程回调
	void managerFun() {
		//每隔一段时间，检测当前运行的线程数和执行的线程数，
	//如果运行的线程数远大于正在执行的线程数，停止一些空转的线程，
	//并将它移出容器
		while (!m_iShutdown)
		{
			sleep(3);
			pthread_mutex_lock(&m_mutexTask);
			//取出任务的个数
			//取出正在运行的线程个数
			//取出正在执行的线程个数
			int iTaskNum = m_queueTask.GetTaskSize();
			int iExecNum = m_iBusyNum;
			int iRunNum = m_iLiveNum;
			pthread_mutex_unlock(&m_mutexTask);

			//任务个数 > 存活的线程个数 && 小于最大线程个数 创建线程
			//运行线程个数 * 2 < 存活的线程个数 && 大于最小线程个数 删除线程
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
	//线程退出
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
	//任务队列
	TaskQueue<T>				m_queueTask;	//任务队列 任务是以函数的形式存在，所以保存的是函数的地址
	//管理者线程 
	pthread_t	m_pidManager;		//管理者线程ID

	//工作线程
	std::vector<pthread_t> m_workThreadList;	//工作线程ID
	pthread_mutex_t m_mutexTask;		//对任务队列的锁


	int m_iMaxNum;		//线程最大数
	int m_iMinNum;		//线程最小数
	int m_iBusyNum;		//正在执行任务的线程数
	int m_iLiveNum;		//正在运行的线程数（正在运行，不代表在执行任务，有可能在空转）
	int m_iRemoveNum;	//待删除的线程数（多个线程空转，超出一定数量后将线程删除）

	pthread_cond_t  m_condNotEmpty;		//队空条件

	int				m_iShutdown;		//是否销毁线程池

	static const int SIZE = 2;

};

