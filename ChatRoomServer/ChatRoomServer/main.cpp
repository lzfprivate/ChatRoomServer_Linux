#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "LoggerServer.h"
#include "Process.h"



int CreateLoggerServer(CProcess* proc) {
    printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
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
    return 0;
}