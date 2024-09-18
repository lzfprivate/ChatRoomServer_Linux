#pragma once

#include "Buffer.h"
class CFrame
{
public:
	CFrame();
	~CFrame();
	//��֡
	int Encode(int funcode, const CBuffer& str);
	//��֡
	int Decode(const CBuffer& str);
	int Decode(const char* str,int len);

public:
	int m_iFrameHead;		//֡ͷ
	int m_iPackLen;			//ȫ֡����
	int m_iFrameFunc;		//������		0:ȷ��֡ 1:�����¼ 2:������Ϣ 3:�����ļ� 4:���ͱ���� TODO:�����
	int m_iFrameLen;		//֡���ݳ���
	CBuffer m_bufFrame;		//֡����

	int m_iFrameTail;		//֡β
	CBuffer m_bufTotal;		//ȫ֡����
};

