#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "LoggerServer.h"
#include "Process.h"
#include "ThreadPool.h"
#include "MySqlClient.h"



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

DECLARE_TABLE_CLASS(test_mysql, _mysql_table_)
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_id", "VARCHAR", "(15)", PRIMARY_KEY | NOT_NULL|AUTO_INCREAMENT, "", "")
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_qq", "VARCHAR", "(15)", NOT_NULL, "", "")
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_passwd", "VARCHAR", "(15)", NOT_NULL, "", "")
DECLARE_MYSQL_FIELD(TYPE_TEXT, "user_sex", "TEXT", "", NOT_NULL, "男", "")
DECLARE_MYSQL_FIELD(TYPE_TEXT, "user_name", "TEXT", "", NOT_NULL, "", "")
DECLARE_TABLE_CLASS_END()

int MysqlTest()
{
    test_mysql test,value;
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
    getchar();
    return 0;
}

int main()
{
    return MysqlTest();
}