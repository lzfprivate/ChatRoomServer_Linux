#pragma once
#include <string>
#include <cstddef>
class CBuffer : public std::string
{
public:
	CBuffer();
	CBuffer(size_t nSize);

	operator const char*() const { return c_str(); }
};

