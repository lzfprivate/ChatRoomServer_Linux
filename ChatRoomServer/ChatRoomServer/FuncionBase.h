#pragma once

#include <functional>

/// <summary>
/// 
/// CFunctionBase���Ǻ���������
/// </summary>

class CSockBase;
class CBuffer;
class CFunctionBase 
{
public:
	virtual ~CFunctionBase() {

	}
	//()��������أ����ڵ��ûص�����
	virtual int operator()() {
		return -1;
	}
	virtual int operator()(CSockBase*) {
		return -1;
	}
	virtual int operator()(CSockBase*,const CBuffer& ) {
		return -1;
	}

};
