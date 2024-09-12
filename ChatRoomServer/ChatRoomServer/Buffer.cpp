#include "Buffer.h"
#include <string.h>

CBuffer::CBuffer() : std::string()
{
}

CBuffer::CBuffer(size_t nSize) : std::string()
{
	resize(nSize);
}

CBuffer::CBuffer(const char* buf) : std::string(buf)
{
	resize(strlen(buf));
	if (buf[0] == '.') {
		resize(256);
	}
	memcpy((void*)c_str(), buf, strlen(buf));
}

CBuffer::CBuffer(const char* buf, size_t length)
{
	resize(length);
	memcpy((void*)c_str(), buf, length);
}

CBuffer::CBuffer(std::string str)
	: std::string(str)
{
	resize(str.size());
	memcpy((void*)c_str(), str.c_str(), str.size());
}

CBuffer::CBuffer(const char* begin, const char* end)
{
	//复制某个区间的数据
	std::string str(begin, end);
	resize(str.size());
	memcpy((void*)c_str(), str.c_str(), str.size());
}

CBuffer& CBuffer::operator=(const CBuffer& buf)
{
	if (&buf != this)
	{
		*this = buf;
	}
	printf("%s(%d):<%s> param.bufIp = %s,m_bufIp=%s\n", __FILE__, __LINE__, __FUNCTION__, buf.c_str(), c_str());

	return *this;
}

CBuffer& CBuffer::operator=(const char* buf)
{
	resize(strlen(buf));
	memcpy((void*)c_str(), buf, strlen(buf));
	return *this;
}
