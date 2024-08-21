#pragma once
#include <string>
#include <cstddef>
class CBuffer : public std::string
{
public:
	CBuffer();
	CBuffer(size_t nSize);
	CBuffer(const char* buf);
	CBuffer(std::string str);
	CBuffer& operator=(const char* buf);
	operator char* () { return (char*)c_str(); }
	operator const char*() const { return c_str(); }
};

