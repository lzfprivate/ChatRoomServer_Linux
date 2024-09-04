#pragma once
#include <string>
#include <cstddef>

class CBuffer : public std::string
{
public:
	CBuffer();
	CBuffer(size_t nSize);
	CBuffer(const char* buf);
	CBuffer(const char* buf,size_t length);
	CBuffer(std::string str);
	CBuffer(const char* begin, const char* end);
	CBuffer& operator=(const char* buf);
	operator char* () { return (char*)c_str(); }
	operator char* () const { return (char*)c_str(); }
	operator const char*() { return c_str(); }
	operator void* () { return (void*)c_str(); }
};

