//#pragma once

#include <functional>

/// <summary>
/// 
/// CFunctionBase���Ǻ���������
/// </summary>
class CFunctionBase {
public:
	virtual ~CFunctionBase() {

	}
	virtual int operator()() = 0;			//()��������أ����ڵ��ûص�����
};
