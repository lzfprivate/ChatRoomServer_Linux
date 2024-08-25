#pragma once
#include <pthread.h>
#include <unordered_map>
#include "FuncionBase.h"


template<typename _FUNCTION_,typename ..._ARGS_>
class CFunction : public CFunctionBase
{
public:
	CFunction(_FUNCTION_ func, _ARGS_... args) :
		m_binder(std::forward<_FUNCTION_>(func),std::forward<_ARGS_>(args)...)
	{

	}
	virtual ~CFunction() {
		
	}

	int SetFunctionEntry(_FUNCTION_ func, _ARGS_... args)
	{
		m_binder = new CFunction(func, args...);
		if (m_binder) return 0;
		return -1;
	}

	int operator()() {
		return m_binder();
	}
private:
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;

};


class CThread
{
public:
	CThread();
	template<typename _FUNCTION_,typename ..._ARGS_>
	CThread(_FUNCTION_ func, _ARGS_... args) {

	}

	template<typename _FUNCTION_, typename ..._ARGS_>
	int SetFunctionEntry(_FUNCTION_ func, _ARGS_... args)
	{
		m_function = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_function) return 0;
		return -1;
	}

	CThread& operator=(const CThread& thread) = delete;
	CThread(const CThread& thread)=delete;
	~CThread();

	//启动
	int Start();
	//暂停
	int Pause();
	//停止
	int Stop();


private:
	//线程程序入口 
	static void* ThreadEntry(void* args);
	//线程功能执行
	int ThreadFunction();


private:
	CFunctionBase* m_function;			//连接响应函数
	pthread_t		m_pid;				//线程号
	bool			m_bRunning;			//线程运行状态 0:未运行 1:正在运行

	static std::unordered_map<pthread_t, CThread*> m_mpThreads;
};

