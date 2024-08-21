#include "Buffer.h"

CBuffer::CBuffer()
{
	clear();
}

CBuffer::CBuffer(size_t nSize)
{
	resize(nSize);
}

CBuffer::CBuffer(const char* buf)
{
	//TODO:
}

CBuffer::CBuffer(std::string str)
{
}

CBuffer& CBuffer::operator=(const char* buf)
{
	// TODO: 在此处插入 return 语句
	return *this;
}
