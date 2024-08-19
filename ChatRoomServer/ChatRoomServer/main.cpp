#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "Process.h"


int CreateLoggerServer(CProcess* proc) {
    printf("create logger server\n");
    return 0;
}



int CreateLoggerClient(CProcess* proc) {
    int fd = -1;
    int iRet = proc->RecvFD(fd);
    if (iRet != 0) {
        printf("<%s> [%d] (%s)  iRet:%d errno :%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, iRet, errno,strerror(errno));
    }
    sleep(1);           //等待主进程写入数据
    char buf[10] = "";
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 10);
    printf("fd:%d read stream from file:%s\n",fd, buf);
    return 0;
}

int main()
{
    int iRet = -1;
    CProcess procLogS, procLogC;
    iRet = procLogS.SetFunctionEntry(CreateLoggerServer, &procLogS);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    iRet = procLogS.CreateSubProcess();
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    iRet = procLogC.SetFunctionEntry(CreateLoggerClient, &procLogC);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    iRet = procLogC.CreateSubProcess();
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
        return -1;
    }
    usleep(100);
   
    int fd = open("./1.txt", O_RDWR | O_APPEND | O_CREAT);
    if (fd == -1)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
        return -1;
    }

    iRet = procLogC.SendFD(fd);
    if (iRet != 0)
    {
        printf("<%s> [%d] (%s)  send ret:%d errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, iRet, errno, strerror(errno));
        return -1;
    }

    const char* buf = "hello";
    write(fd, buf, 6);
    close(fd);

    return 0;
}