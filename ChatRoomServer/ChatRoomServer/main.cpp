#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "LoggerServer.h"
#include "Process.h"
#include "ThreadPool.h"
#include "MySqlClient.h"



int CreateLoggerServer(CProcess* proc) {
    int ret = -1;
    CLoggerServer server;
    ret = server.Start();
    if (ret != 0)
    {
        printf("%s(%d):<%s> logserver start ret:%d pid=%d\n", __FILE__, __LINE__, __FUNCTION__, ret, getpid());
        return -1;
    }
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
        printf("%s(%d):<%s> errno=%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
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



int MysqlTest()
{
    /*test_mysql test,value;
    printf("%s\n", test.Create().c_str());
    printf("%s\n", test.Remove(test).c_str());

    value.m_FieldList["user_id"]->LoadFromStr("121650");
    value.m_FieldList["user_id"]->m_uCondition = SQL_INSERT;
    printf("insert ret:%s\n",test.Insert(value).c_str());
    value.m_FieldList["user_id"]->LoadFromStr("664512");
    value.m_FieldList["user_id"]->m_uCondition = SQL_MODIFY;
    printf("modify ret:%s\n",test.Modify(value).c_str());
    printf("query ret:%s\n",test.Query().c_str());
    printf("drop ret:%s\n",test.Drop().c_str());

    CMySqlClient client;
    KEYVALUE connMsg;
    connMsg["host"] = "192.168.0.11";
    connMsg["user"] = "root";
    connMsg["passwd"] = "123456";
    connMsg["db"] = "ChatRoom";
    connMsg["port"] = "3306";

    int ret = client.Connect(connMsg);
    printf("%s(%d):<%s> mysql server connect ret:%d\n", __FILE__,
        __LINE__, __FUNCTION__, ret);
    ret = client.Execute(test.Create());
    printf("%s(%d):<%s> mysql server execute ret:%d\n", __FILE__,
        __LINE__, __FUNCTION__, ret);

    ret = client.Execute(test.Drop());
    printf("%s(%d):<%s> mysql server drop table ret:%d\n", __FILE__,
        __LINE__, __FUNCTION__, ret);
    getchar();
    return 0;*/
}

void epoll_test()
{
    sleep(1);
    int server = socket(PF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = PF_LOCAL;
    strcpy(addr.sun_path, "/root/projects/ChatRoomServer/bin/x64/Debug/log/logger.sock");
    socklen_t addrLen = sizeof(struct sockaddr_un);
    if (-1 == connect(server, (sockaddr*)&addr, addrLen))
    {
        printf("%s(%d):<%s> client connect errno:%d errmsg:%s\n", __FILE__,
            __LINE__, __FUNCTION__, errno,strerror(errno));
    }
    usleep(100000);
    if (-1 == send(server, "hello", 6, 0))
    {
        printf("%s(%d):<%s> client send errno:%d errmsg:%s\n", __FILE__,
            __LINE__, __FUNCTION__, errno, strerror(errno));
    }
    getchar();

}

int main()
{
    CProcess procServer;
    int ret = procServer.SetEntryFunction(CreateLoggerServer, &procServer);
    ret = procServer.CreateSubProcess();
    LoggerTest();
    procServer.SendFD(0);

    getchar();
    return 0;
}