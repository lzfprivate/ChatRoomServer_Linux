#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "LoggerServer.h"
#include "Process.h"
#include "ThreadPool.h"



int CreateLoggerServer(CProcess* proc) {
    printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    int ret = -1;
    CLoggerServer server;
    printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    ret = server.Start();
    printf("%s(%d):<%s> logserver start ret:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, ret, getpid());
    int fd = 0;
    while (1)
    {
        proc->RecvFD(fd);
        printf("%s(%d):<%s> server recv fd:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, fd, getpid());
        if (fd == 0) break;
    }
    server.Close();
    printf("%s(%d):<%s> log server close pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    return 0;
}



int CreateLoggerClient(CProcess* proc) {
    printf("%s(%d):<%s> pid=%d\n", __FILE__,
        __LINE__, __FUNCTION__, getpid());
    int fd = -1;
    int ret = proc->RecvFD(fd);
    printf("%s(%d):<%s> ret=%d\n", __FILE__,
        __LINE__, __FUNCTION__, ret);
    printf("%s(%d):<%s> fd=%d\n", __FILE__,
        __LINE__, __FUNCTION__, fd);
    if (ret != 0) {
        printf("%s(%d):<%s> errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno,strerror(errno));
    }
    sleep(1);
    char buf[10] = "";
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof(buf));
    printf("%s(%d):<%s> buf=%s\n", __FILE__,
        __LINE__, __FUNCTION__, buf);
    close(fd);
    
    return 0;
}

int LoggerTest()
{
    //测试点：整数、小数、字符 
    //英语 汉语 阿拉伯数字 标点符号
    //单线程 多线程 信号触发 调用
    usleep(1000 * 100);//等待子进程启动
    char buf[] = "李白 给我一首《divide》的时间 ";
    TRACEI("Print Trace Content : %d %f %g %c %s 可口 百事", 100, 0.1f, 0.2, '3', buf);
    DUMPD(buf, sizeof(buf));
    LOGI << "write log into file: " << 1 << 2.0f << 3.0 << '4' << "中文添加";
    return 0;
}

int main()
{
    CProcess proclog, procclients;
    printf("%s(%d):<%s> pid=%d\n", __FILE__,__LINE__, __FUNCTION__, getpid());
    proclog.SetEntryFunction(CreateLoggerServer,&proclog);
    int ret = proclog.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -1;
    }
    printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    //LoggerTest();
    printf("%s(%d):<%s> main thread pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());

    //CThread thread(LoggerTest);
    //thread.Start();
    //printf("%s(%d):<%s> another thread pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    CThreadPool pool;
    pool.Start();
    pool.AddTask(LoggerTest);
    
    procclients.SetEntryFunction(CreateLoggerClient, &procclients);
       
    ret = procclients.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -2;
    }
    printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
       
    usleep(100 * 000);
    int fd = open("./1.txt", O_RDWR | O_CREAT | O_APPEND);
        
    printf("%s(%d):<%s> fd=%d\n", __FILE__, __LINE__, __FUNCTION__, fd);

    if (fd == -1)return -3;
    ret = procclients.SendFD(fd);
    printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
    if (ret != 0)printf("errno:%d msg:%s\n", errno, strerror(errno));
    write(fd, "edoyun", 6);
    close(fd);
    proclog.SendFD(0);
    getchar();
    return 0;
}