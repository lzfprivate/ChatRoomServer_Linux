#include "Buffer.h"

CBuffer::CBuffer() : std::string()
{
}

CBuffer::CBuffer(size_t nSize) : std::string()
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
