#include "Log.h"
#include <sys/timeb.h>
#include <stdarg.h>
#include "LoggerServer.h"

//TODO:Æ´½Ó×Ö·û´®

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level, const char* fmt ...)
{
	char spLevel[][8] = { "INFO","DEBUG","WARNING","ERROR","FATAL" };
	char* buf = nullptr;
	m_bAuto = false;
	int count = asprintf(&buf, "<%s>[%d-%s] %d %d %s %s", file, line, name, pid, tid, GetCurTimeString().c_str(), spLevel[level]);
	if (count > 0) m_buf = buf;
	else return;
	free(buf);
	va_list ap;
	va_start(ap,fmt);
	count = vasprintf(&buf, fmt, ap);
	if (count > 0)
	{
		m_buf += buf;
		free(buf);
	}
	va_end(ap);
}

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level)
{
	char spLevel[][8] = { "INFO","DEBUG","WARNING","ERROR","FATAL" };
	char* buf = nullptr;
	m_bAuto = false;
	timeb tmb;
	ftime(&tmb);
	int count = asprintf(&buf, "<%s>[%d-%s] %d %d %s %s", file, line, name, pid, tid, GetCurTimeString().c_str(), spLevel[level]);
	if (count > 0) m_buf = buf;
	else return;
	free(buf);
}

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level, void* data, size_t size)
{
	char spLevel[][8] = { "INFO","DEBUG","WARNING","ERROR","FATAL" };
	char* buf = nullptr;
	m_bAuto = false;
	int count = asprintf(&buf, "<%s>[%d-%s] %d %d %s %s", file, line, name, pid, tid, GetCurTimeString().c_str(), spLevel[level]);
	if (count > 0) m_buf = buf;
	else return;
	free(buf);
	size_t index = 0;
	m_buf += "\n";
	char* temp = (char*)data;

	for (; index < size; ++index)
	{
		//Êä³öÊ®Áù½øÖÆ×Ö·û´®    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF        ................
		//						FE FE FE											   ...
		char buf[16];
		snprintf(buf, sizeof(buf), "%02X ", temp[index] & 0xFF);
		m_buf += buf;
		if ((index + 1) % 16 == 0)
		{
			m_buf += "\t";
			for (size_t j = index - 15; j <= index; j++)
			{
				//acsiiÂë
				if ((temp[j] & 0xFF) > 0x31 && (temp[j] & 0xFF) < 0x7F)
				{
					m_buf += temp[j];
				}
				else
				{
					m_buf += ".";
				}
			}
			m_buf += "\n";
		}
	}
	int mod = index % 16;
	if (mod != 0)
	{
		for (size_t j = 0; j < 16 - mod; j++)
		{
			m_buf += "   ";
		}
		m_buf += "\t";
		for (size_t j = index - 15; j <= index; j++)
		{
			//acsiiÂë
			if ((temp[j] & 0xFF) > 0x31 && (temp[j] & 0xFF) < 0x7F)
			{
				m_buf += temp[j];
			}
			else
			{
				m_buf += ".";
			}
		}
		m_buf += "\n";
	}
}

CLogMessage::~CLogMessage()
{
	if (m_bAuto)
	{
		//·¢ËÍ
		CLoggerServer::Trace(*this);
	}
}

CBuffer CLogMessage::GetCurTimeString()
{
	CBuffer bufTime(128);
	timeb tmb;
	ftime(&tmb);
	tm* time = localtime(&tmb.time);

	//Æ´½ÓÊ±¼ä×Ö·û´®
	int nSize = snprintf(bufTime, bufTime.size(),
		"%04d-02%d-%02d-%02d_%02d_%02d_%02d_%03d",
		time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
		time->tm_hour, time->tm_min, time->tm_sec,
		tmb.millitm);
	bufTime.resize(nSize);
	return bufTime;
}
