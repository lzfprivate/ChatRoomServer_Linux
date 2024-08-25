#pragma once
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "FuncionBase.h"


template <typename _FUNCTION_,typename... _ARGS_>
class CFunctionProcess :public CFunctionBase
{
public:
	virtual ~CFunctionProcess() {

	}
	CFunctionProcess(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{

	}

	int SetFunction(_FUNCTION_ func, _ARGS_... args) {
		m_binder = new CFunctionProcess(func, args...);
		if (m_binder) return 0;
		return -1;
	}

	int operator()() override {
		return m_binder();
	}

private:
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;
};

class CProcess
{
public:
	CProcess();
	~CProcess();

	/// <summary>
	/// 设置入口函数
	/// </summary>
	/// <typeparam name="_FUNCTION_">函数</typeparam>
	/// <typeparam name="..._ARGS_">函数参数</typeparam>
	/// <param name="func">函数名</param>
	/// <param name="...args">参数集合</param>
	/// <returns>0：成功 其他：失败</returns>
	template <typename _FUNCTION_, typename ..._ARGS_>
	int SetFunctionEntry(_FUNCTION_ func, _ARGS_... args) {
		m_func = new CFunctionProcess<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_func) return 0;
		return -1;
	}

	/// <summary>
	/// 创建子进程
	/// </summary>
	/// <returns>0：成功 其他：失败</returns>
	int CreateSubProcess();
	
	/// <summary>
	/// 发送通信句柄
	/// </summary>
	/// <param name="fd">通信的套接字句柄</param>
	/// <returns>0：发送成功 其他：发送失败</returns>
	int SendFD(int fd);
	/// <summary>
	/// 接收通信句柄
	/// </summary>
	/// <param name="fd">接收到的套接字句柄</param>
	/// <returns>0：接收成功  其他：接收失败</returns>
	int RecvFD(int& fd);


private:
	CFunctionBase* m_func;
	int m_pid;
	int m_pipes[2];				//管道通信 pipe[0]为读 pipe[1]为写
};
