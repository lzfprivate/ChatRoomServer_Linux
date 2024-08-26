#pragma once

#include <functional>

/// <summary>
/// 
/// CFunctionBase类是函数调用类
/// </summary>

class CSockBase;
class CBuffer;
class CFunctionBase 
{
public:
	virtual ~CFunctionBase() {

	}
	//()运算符重载，用于调用回调函数
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
