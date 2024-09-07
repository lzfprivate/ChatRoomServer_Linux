#pragma once
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <functional>
#include <memory.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "FuncionBase.h"


template<typename _FUNCTION_, typename... _ARGS_>
class CFunctionProc :public CFunctionBase
{
public:
	CFunctionProc(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{}
	virtual ~CFunctionProc() {}
	virtual int operator()() {
		return m_binder();
	}
	typename std::_Bindres_helper<int,_FUNCTION_, _ARGS_...>::type m_binder;
};

class CProcess
{
public:
	CProcess();
	~CProcess();

	/// <summary>
	/// ������ں���
	/// </summary>
	/// <typeparam name="_FUNCTION_">����</typeparam>
	/// <typeparam name="..._ARGS_">��������</typeparam>
	/// <param name="func">������</param>
	/// <param name="...args">��������</param>
	/// <returns>0���ɹ� ������ʧ��</returns>
	template<typename _FUNCTION_, typename..._ARGS_>
	int SetEntryFunction(_FUNCTION_ func,_ARGS_... args)
	{
		m_func = new CFunctionProc<_FUNCTION_,_ARGS_...>(func, args...);
		if(m_func) return 0;
		return -1;
	}

	/// <summary>
	/// �����ӽ���
	/// </summary>
	/// <returns>0���ɹ� ������ʧ��</returns>
	int CreateSubProcess();
	
	/// <summary>
	/// ����ͨ�ž��
	/// </summary>
	/// <param name="fd">ͨ�ŵ��׽��־��</param>
	/// <returns>0�����ͳɹ� ����������ʧ��</returns>
	int SendFD(int fd);
	int SendSocket(int fd, sockaddr* addr);
	/// <summary>
	/// ����ͨ�ž��
	/// </summary>
	/// <param name="fd">���յ����׽��־��</param>
	/// <returns>0�����ճɹ�  ����������ʧ��</returns>
	int RecvFD(int& fd);
	int RecvSocket(int& fd,sockaddr* addr);


private:
	CFunctionBase* m_func;
	int m_pid;
	int m_pipes[2];				//�ܵ�ͨ�� pipe[0]Ϊ�� pipe[1]Ϊд
};
