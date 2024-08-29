#pragma once
#include "DataBase.h"
class CMySqlClient :
    public CDataBaseClient
{
public:
	CMySqlClient operator=(const CMySqlClient& client) = delete;
	CMySqlClient(const CMySqlClient& client) = delete;
	virtual ~CMySqlClient()
	{

	}
	virtual int Connect(const KEYVALUE& loginMsg);
	//执行语句不返回结果: 增加 删除等不返回结果的sql语句
	virtual int Execute(const CBuffer& sql);
	//执行语句带返回结果: 查询等要返回结果
	virtual int Execute(const CBuffer& sql, Result& result, const _Table_& table);
	//开启事务
	virtual int StartTransaction();
	//提交事务
	virtual int CommitTransaction();
	//回滚事务
	virtual int RollbackTransaction();
	//是否连接
	virtual bool IsConnected();
	//关闭
	virtual void Close();
};

