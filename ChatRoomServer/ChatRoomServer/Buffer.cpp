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
	// TODO: �ڴ˴����� return ���
	return *this;
}
