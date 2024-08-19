#include "Thread.h"


std::unordered_map<pthread_t, CThread*> CThread::m_mpThreads;

CThread::CThread() : m_function(nullptr),m_bRunning(false)
{
}

CThread::~CThread()
{
}

int CThread::Start()
{
    pthread_attr_t attr;
    int ret = pthread_create(&m_pid, &attr, CThread::ThreadEntry, this);
    return ret;
}

int CThread::Pause()
{
    return 0;
}

int CThread::Stop()
{
    return 0;
}

void* CThread::ThreadEntry(void* args)
{
    CThread* thiz = (CThread*)args;
    thiz->ThreadFunction();
    if (thiz->m_pid) thiz->m_pid = 0;

    //×¢²áÍ£Ö¹ºÍÔÝÍ£µÄÐÅºÅ


    pthread_t local = pthread_self();
    if (m_mpThreads.find(local) != m_mpThreads.end())
    {
        m_mpThreads[local] = 0;
    }
    pthread_detach(local);
    pthread_exit(NULL);
}

int CThread::ThreadFunction()
{
    (*m_function)();
    return 0;
}
