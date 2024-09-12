#pragma once

#include <pthread.h>
#include <sstream>
#include "Buffer.h"

enum EnLogLevel
{
	EnMsg = 0,
	EnDebug,
	EnWarning,
	EnError,
	EnFatal
};

class CLogMessage {
public:
	//���ڴ�ӡ
	CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level, const char* fmt...);
	//���ڱ����ļ�
	CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level);
	//������ʾ������
	CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level,void * data,size_t size);
	~CLogMessage();

	template<typename T>
	CLogMessage& operator<<(const T& data)
	{
		std::stringstream stream;
		stream << data;
		m_buf += stream.str();
		return *this;
	}

	operator CBuffer() const {
		return m_buf;
	}

private:
	CBuffer		m_buf;
	bool		m_bAuto;

};





