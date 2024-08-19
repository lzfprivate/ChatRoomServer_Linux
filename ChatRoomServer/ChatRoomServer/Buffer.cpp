#include "Buffer.h"

CBuffer::CBuffer()
{
	clear();
}

CBuffer::CBuffer(size_t nSize)
{
	resize(nSize);
}
