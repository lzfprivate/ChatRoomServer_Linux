#pragma once
#include <vector>
#include <openssl/md5.h>
#include "Buffer.h"
class CMd5Encode
{
public:
	static CBuffer Encode(const CBuffer& buffer) {
		CBuffer ret;
		std::vector<unsigned char> data;
		data.resize(16);

		MD5_CTX md5;
		MD5_Init(&md5);
		MD5_Update(&md5, buffer.data(), buffer.size());
		MD5_Final(data.data(), &md5);
		char temp[3] = "";
		for (size_t i = 0; i < data.size(); i++)
		{

			snprintf(temp, 3, "%02x", data[i] & 0XFF);
			ret += temp;
		}
		return ret;
	}
};

