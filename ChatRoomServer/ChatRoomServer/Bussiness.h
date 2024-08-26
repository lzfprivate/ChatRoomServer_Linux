#pragma once
/// <summary>
/// 业务类:
/// 1.接收连接
/// 2.接收数据
/// </summary>

#include "FuncionBase.h"
#include "Process.h"

template<typename _FUNCTION_, typename ..._ARGS_>
class CFunctionConnect :public CFunctionBase
{
public:
	CFunctionConnect(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward< _FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{

	}

	virtual ~CFunctionConnect()
	{

	}

//public:
//	int SetFunction(_FUNCTION_ func, _ARGS_... args) {
//		m_binder = new CFunctionConnect(func, args...);
//		if (m_binder) return 0;
//		return -1;
//	}

public:
	virtual int operator()(CSockBase* client) override {
		return m_binder(client);
	}

private:
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;

};

template<typename _FUNCTION_, typename ..._ARGS_>
class CFunctionReceive :public CFunctionBase
{
public:
	CFunctionReceive(_FUNCTION_ func, _ARGS_... args) :
		m_binder(std::forward< _FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{

	}
		
	//int SetFunction(_FUNCTION_ func, _ARGS_... args) {
	//	m_binder = new CFunctionReceive(func, args...);
	//	if (m_binder) return 0;
	//	return -1;
	//}
public:
	int operator()(CSockBase* sock,const CBuffer& buffer) override {
		return m_binder(sock, buffer);
	}


private:
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;
};


class CBussiness
{
public:
	CBussiness() :m_funcConn(nullptr), m_funcRecv(nullptr){

	}
	~CBussiness()
	{

	}

	template<typename _FUNCTION_,typename ..._ARGS_>
	int SetConnFunction(_FUNCTION_ func, _ARGS_... args) {
		m_funcConn = new CFunctionConnect<_FUNCTION_,_ARGS_...>(func, args...);
		if (m_funcConn) return 0;
		return -1;
	}

	template<typename _FUNCTION_, typename ..._ARGS_>
	int SetRecvFunction(_FUNCTION_ func, _ARGS_... args) 
	{
		m_funcRecv = new CFunctionReceive<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_funcRecv) return 0;
		return -1;
	}

public:
	virtual int BusinessProcess(CProcess* process) = 0;

protected:
	CFunctionBase* m_funcConn;
	CFunctionBase* m_funcRecv;
};

