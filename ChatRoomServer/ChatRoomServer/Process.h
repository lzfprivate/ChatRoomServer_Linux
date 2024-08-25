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
	/// ������ں���
	/// </summary>
	/// <typeparam name="_FUNCTION_">����</typeparam>
	/// <typeparam name="..._ARGS_">��������</typeparam>
	/// <param name="func">������</param>
	/// <param name="...args">��������</param>
	/// <returns>0���ɹ� ������ʧ��</returns>
	template <typename _FUNCTION_, typename ..._ARGS_>
	int SetFunctionEntry(_FUNCTION_ func, _ARGS_... args) {
		m_func = new CFunctionProcess<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_func) return 0;
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
	/// <summary>
	/// ����ͨ�ž��
	/// </summary>
	/// <param name="fd">���յ����׽��־��</param>
	/// <returns>0�����ճɹ�  ����������ʧ��</returns>
	int RecvFD(int& fd);


private:
	CFunctionBase* m_func;
	int m_pid;
	int m_pipes[2];				//�ܵ�ͨ�� pipe[0]Ϊ�� pipe[1]Ϊд
};
