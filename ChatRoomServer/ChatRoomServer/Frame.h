#pragma once

#include "Buffer.h"
class CFrame
{
public:
	CFrame();
	~CFrame();
	//组帧
	int Encode(int funcode, const CBuffer& str);
	//解帧
	int Decode(const CBuffer& str);
	int Decode(const char* str,int len);

public:
	int m_iFrameHead;		//帧头
	int m_iPackLen;			//全帧长度
	int m_iFrameFunc;		//功能码		0:确认帧 1:请求登录 2:发送消息 3:发送文件 4:发送表情包 TODO:待添加
	int m_iFrameLen;		//帧数据长度
	CBuffer m_bufFrame;		//帧数据

	int m_iFrameTail;		//帧尾
	CBuffer m_bufTotal;		//全帧数据
};

