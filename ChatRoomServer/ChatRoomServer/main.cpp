#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "LoggerServer.h"
#include "Process.h"



int CreateLoggerServer(CProcess* proc) {
    printf("<%s> [%d] (%s) pid:%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    return 0;
}



int CreateLoggerClient(CProcess* proc) {
    printf("<%s> [%d] (%s) pid:%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    int fd = -1;
    int iRet = proc->RecvFD(fd);
    printf("<%s> [%d] (%s)  ret:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
    printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno,strerror(errno));
    }
    sleep(1);           //等待主进程写入数据
    char buf[10] = "";
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 10);
    printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
    time_t now = time(0);
    char* dt = ctime(&now);
    printf("<%s> [%d] (%s)  child close fd time:%s\n", __FILE__, __LINE__, __FUNCTION__, dt);
    close(fd);
    return 0;
}

int main()
{
    int iRet = -1;
    CProcess procLogS, procLogC;
    printf("<%s> [%d] (%s) pid:%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    iRet = procLogS.SetFunctionEntry(CreateLoggerServer, &procLogS);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s) ret:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    iRet = procLogS.CreateSubProcess();
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s) ret:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    printf("<%s> [%d] (%s) pid:%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    iRet = procLogC.SetFunctionEntry(CreateLoggerClient, &procLogC);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s) ret:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    iRet = procLogC.CreateSubProcess();
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s) ret:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    printf("<%s> [%d] (%s) pid:%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());

    usleep(10000); //等待子进程创建

    int fd = open("./1.txt", O_RDWR | O_CREAT | O_APPEND);
    if (fd == -1)
    {
        printf("<%s> [%d] (%s) fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
        return -1;
    }
    printf("<%s> [%d] (%s) fd:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, fd, getpid());

    iRet = procLogC.SendFD(fd);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s) fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
        printf("<%s> [%d] (%s)  send ret:%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, iRet, errno, strerror(errno));
        return -1;
    }
    printf("<%s> [%d] (%s) ret:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, iRet, getpid());
    printf("<%s> [%d] (%s) fd:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, fd, getpid());

    const char* buf = "hello";
    write(fd, buf, 6);
    time_t now = time(0);
    char* dt = ctime(&now);
    printf("<%s> [%d] (%s)  child close fd time:%s\n", __FILE__, __LINE__, __FUNCTION__, dt);
    close(fd);
    return 0;
}