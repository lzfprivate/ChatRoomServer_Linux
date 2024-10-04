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
    int ret = pthread_attr_init(&attr);
    if (ret != 0) return -1;
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (ret != 0) return -2;
    //ret = pthread_attr_setscope(&attr,PTHREAD_SCOPE_PROCESS);
    //if (ret != 0) return -3;
    ret = pthread_create(&m_pid, &attr, &CThread::ThreadEntry, this);
    if (ret != 0) return -4;
    m_mpThreads[m_pid] = this;
    ret = pthread_attr_destroy(&attr);
    if (ret != 0) return -5;
    printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
    return 0;
}

int CThread::Pause()
{
    return 0;
}

int CThread::Stop()
{
    if (m_pid != 0)
    {
        pthread_t tid = m_pid;
        m_pid = 0;
        timespec tms;
        tms.tv_sec = 0;
        tms.tv_nsec = 100 * 1000000;
        int ret = pthread_timedjoin_np(tid, nullptr, &tms);
        if (ret = ETIMEDOUT)
        {
            pthread_detach(tid);
            pthread_kill(tid, SIGUSR2);
        }
    }
    return 0;
}

bool CThread::IsVaild() const
{
    return m_pid != 0;
}

void* CThread::ThreadEntry(void* args)
{
    CThread* thiz = (CThread*)args;
   
    struct sigaction sig = { 0 };
    //×¢²áÍ£Ö¹ºÍÔÝÍ£µÄÐÅºÅ
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = &CThread::SignalAction;
    sigaction(SIGUSR1, &sig, NULL);
    sigaction(SIGUSR2, &sig, NULL);
    thiz->ThreadFunction();

    if (thiz->m_pid) thiz->m_pid = 0;
    pthread_t local = pthread_self();
    if (m_mpThreads.find(local) != m_mpThreads.end())
    {
        m_mpThreads[local] = nullptr;
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

void CThread::SignalAction(int sigNo, siginfo_t* info, void* context)
{
    if (sigNo == SIGUSR1)
    {
        //ÔÝÍ£
        pthread_t pid = pthread_self();
        auto iter = m_mpThreads.find(pid);
        if (iter != m_mpThreads.end())
        {
            while (!iter->second->m_bRunning)
            {
                if (iter->second->m_pid == 0)
                {
                    pthread_exit(NULL);
                }
                usleep(1000);

            }
            m_mpThreads[pid] = NULL;
        }
    }
    else if (sigNo == SIGUSR2)
    {
        pthread_exit(NULL);
    }
}

