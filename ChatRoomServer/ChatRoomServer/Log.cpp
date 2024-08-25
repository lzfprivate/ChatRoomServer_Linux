#include "Log.h"
#include <sys/timeb.h>
#include <stdarg.h>

//TODO:Æ´½Ó×Ö·û´®

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level, const char* fmt ...)
{
	
}

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level)
{
}

CLogMessage::CLogMessage(const char* file, int line, const char* name, pid_t pid, pthread_t tid, int level, void* data, size_t size)
{
}

CLogMessage::~CLogMessage()
{

}
