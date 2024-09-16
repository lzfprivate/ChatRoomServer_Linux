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
    //pthread_attr_t attr;
    //pthread_attr_init(&attr);
    //int ret = pthread_create(&m_pid, &attr, &CThread::ThreadEntry, this);
    //pthread_attr_destroy(&attr);
    int ret = pthread_create(&m_pid, NULL, &CThread::ThreadEntry, this);
    m_mpThreads[m_pid] = this;
    printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
    return ret;
}

int CThread::Pause()
{
    return 0;
}

int CThread::Stop()
{
    m_pid = 0;
    m_bRunning = false;
    for (auto iter = m_mpThreads.begin(); iter != m_mpThreads.end(); iter++)
    {
        if (iter->second)
        {
            delete iter->second;
        }
        iter->second = nullptr;
    }
    pthread_exit(NULL);
    return 0;
}

bool CThread::IsVaild() const
{
    return m_pid != 0;
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
    if (m_function)
    {
        return (*m_function)();
    }
    return -1;
}
