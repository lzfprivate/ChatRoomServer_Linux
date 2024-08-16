//#pragma once

#include <functional>

/// <summary>
/// 
/// CFunctionBase类是函数调用类
/// </summary>
class CFunctionBase {
public:
	virtual ~CFunctionBase() {

	}
	virtual int operator()() = 0;			//()运算符重载，用于调用回调函数
};
