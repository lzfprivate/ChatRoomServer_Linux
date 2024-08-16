#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include "Process.h"


int CreateLoggerServer(CProcess* proc) {
    printf("create logger server\n");
    return 0;
}



int CreateLoggerClient(CProcess* proc) {
    int fd = -1;
    int iRet = proc->RecvFD(fd);
    printf("<%s> [%d] (%s)  iRet:%d fd :%d\n", __FILE__, __LINE__, __FUNCTION__, iRet, fd);
    sleep(1);
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
    printf("<%s> [%d] (%s)  iRet:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
    iRet = procLogS.CreateSubProcess();
    printf("<%s> [%d] (%s)  iRet:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
    
    iRet = procLogC.SetFunctionEntry(CreateLoggerClient, &procLogC);
    printf("<%s> [%d] (%s)  iRet:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
    iRet = procLogC.CreateSubProcess();
    printf("<%s> [%d] (%s)  iRet:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);
    
    usleep(100 * 000);
   
    int fd = open("./1.txt", O_RDWR | O_APPEND | O_CREAT);
    if (fd == -1)
    {
        printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
        return -1;
    }
    const char* buf = "hello";
    write(fd, buf, 6);

    printf("<%s> [%d] (%s)  fd:%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
    iRet = procLogC.SendFD(fd);
    printf("<%s> [%d] (%s)  iRet:%d\n", __FILE__, __LINE__, __FUNCTION__, iRet);

 
    close(fd);

    return 0;
}