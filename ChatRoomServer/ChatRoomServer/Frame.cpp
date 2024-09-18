#include "Frame.h"
#include <string.h>

CFrame::CFrame()
{
	m_iFrameHead = 0xFFFE;
	m_iPackLen = sizeof(int) * 5;
	m_iFrameFunc = 0;
	m_iFrameLen = 0;
	m_bufFrame.clear();
	m_bufTotal.clear();
	m_iFrameTail = 0;
}

CFrame::~CFrame()
{
}

int CFrame::Encode(int funcode,const CBuffer& str)
{
	m_iFrameFunc = funcode;
	m_bufFrame = str;
	m_iFrameLen = str.size();
	m_iPackLen += str.size();
	m_bufTotal.append((char*)&m_iFrameHead);
	m_bufTotal.append((char*)&m_iPackLen);
	m_bufTotal.append((char*)&m_iFrameFunc);
	m_bufTotal.append((char*)&m_iFrameLen);
	m_bufTotal.append(m_bufFrame);

	m_iFrameTail = 0;
	for (int i = 0; i < m_bufFrame.size(); ++i)
	{
		m_iFrameTail += (m_bufFrame[i] & 0xFF);
	}
	m_bufTotal.append((char*)&m_iFrameTail);
	if (m_bufTotal.size() < sizeof(int) * 5)
		return -1;
	return 0;
}

int CFrame::Decode(const CBuffer& str)
{
	int iUsedLen = 0;
	int index = 0;
	do
	{
		for (; index < str.size(); ++index)
		{
			if ((*(int*)(str.c_str())) == 0xFFFE)
			{
				iUsedLen = index;
				break;
			}
		}
		m_iPackLen = *(int*)str.c_str() + index;
		//包中的包长度变量 < 包的长度 包的长度不足
		if (m_iPackLen < str.size())
		{
			iUsedLen = 0;
			break;
		}
		index += sizeof(int);
		m_iFrameFunc = *(int*)str.c_str() + index;
		if (m_iFrameFunc < 0)
		{
			iUsedLen = 0;
			break;
		}
		index += sizeof(int);

		m_iFrameLen = *(int*)str.c_str() + index;
		if (m_iFrameLen < 0)
		{
			iUsedLen = 0;
			break;
		}
		m_iFrameLen = *(int*)str.c_str() + index;

		memcpy((void*)m_bufFrame.c_str(), str.c_str() + index, m_iFrameLen);
		index += m_iFrameLen;

		m_iFrameTail = *(int*)str.c_str() + index;
		int checksum = 0;
		for (int i = 0; i < m_iFrameLen; ++i)
		{
			checksum += (m_bufFrame[i] & 0xFF);
		}
		if (checksum == m_iFrameTail)
		{
			iUsedLen = index;
		}
		else {
			iUsedLen = 0;
		}
	} while (0);

	return iUsedLen;
}

int CFrame::Decode(const char* str, int len)
{
	int iUsedLen = 0;
	int index = 0;
	do
	{
		for (; index < len; ++index)
		{
			if ((*(int*)str) == 0xFFFE)
			{
				iUsedLen = index;
				break;
			}
		}
		m_iPackLen = *(int*)str + index;
		//包中的包长度变量 < 包的长度 包的长度不足
		if (m_iPackLen < len)
		{
			iUsedLen = 0;
			break;
		}
		index += sizeof(int);
		m_iFrameFunc = *(int*)str + index;
		if (m_iFrameFunc < 0)
		{
			iUsedLen = 0;
			break;
		}
		index += sizeof(int);

		m_iFrameLen = *(int*)str + index;
		if (m_iFrameLen < 0)
		{
			iUsedLen = 0;
			break;
		}
		m_iFrameLen = *(int*)str + index;

		memcpy((void*)m_bufFrame.c_str(), str+ index, m_iFrameLen);
		index += m_iFrameLen;

		m_iFrameTail = *(int*)str + index;
		int checksum = 0;
		for (int i = 0; i < m_iFrameLen; ++i)
		{
			checksum += (m_bufFrame[i] & 0xFF);
		}
		if (checksum == m_iFrameTail)
		{
			iUsedLen = index;
		}
		else {
			iUsedLen = 0;
		}
	} while (0);

	return iUsedLen;
}
