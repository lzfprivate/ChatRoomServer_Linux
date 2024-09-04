#pragma once
#include <unistd.h>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include "Buffer.h"


enum SQL_OP {
	SQL_INSERT = 1,
	SQL_DELETE = 2,
	SQL_QUERY = 4,
	SQL_MODIFY = 8
};

/// <summary>
/// 简单的理解:表中的属性 也就是表的列
/// </summary>
class _Field_ {
public:
	_Field_() {}
	virtual ~_Field_() {}

public:
	virtual CBuffer Create() = 0;
	//从字符串中加载
	virtual void LoadFromStr(const CBuffer& str) = 0;
	//表达式,用于where语句
	virtual CBuffer EqualExp() = 0;
	//将列中的数据转换成字符串
	virtual CBuffer toSqlStr() = 0;
	//获取列的全名 数据库.表.列名
	virtual operator const CBuffer() const = 0;

	//TODO:根据需求添加
public:
	//列属性
	CBuffer m_strName;			//属性名 姓名 性别 ...
	CBuffer m_strType;			//属性类型 int char varchar...
	CBuffer m_strSize;			//属性占用长度
	unsigned m_uAttr;			//属性的属性 NULL NOT NULL PRIVATE KEY UNIQUE等
	CBuffer m_strDefault;		//属性的默认值
	CBuffer m_strCheck;			//属性的约束条件 大于0 小于0 ...

	unsigned m_uCondition;		//数据库操作标识符 用于有条件的操作
	

	union
	{
		int Integer;
		double Double;
		CBuffer* String;
	}UnValueType;


};

class _Table_;
using PTable = std::shared_ptr< _Table_>;
using PFIELD = std::shared_ptr<_Field_>;
using Result = std::list<PTable>;
using KEYVALUE = std::map<CBuffer, CBuffer>;
using FIELDARRAY = std::vector<PFIELD>;
using FILEDMAP = std::map<CBuffer, PFIELD>;


/// <summary>
/// 数据库表基类
/// 封装表的操作
/// 1.创建
/// 2.删除表
/// 3.向表中插入数据
/// 4.修改表中的数据
/// 5.从表查询数据
/// 6.删除表中数据
/// </summary>

class _Table_
{
public:
	_Table_() {}
	virtual ~_Table_() {}
	//以下操作均返回一个sql语句
	virtual CBuffer Create() = 0;
	virtual CBuffer Drop() = 0;
	virtual CBuffer Insert(const _Table_& table) = 0;
	virtual CBuffer Remove(const _Table_& table) = 0;
	virtual CBuffer Query(const CBuffer& condition = "") = 0;
	virtual CBuffer Modify(const _Table_& table) = 0;

	//获取表的命名 数据库名.表名
	virtual operator const CBuffer() const = 0;
	//复制表
	virtual PTable Copy() const = 0;
	//清理可使用值
	virtual void ClearFieldUsed() = 0;



public:
	CBuffer m_strBelongDataBase;	//本表所属数据库名
	CBuffer m_strName;				//表名
	FIELDARRAY	m_FieldDefine	;	//用于存储结果
	FILEDMAP	m_FieldList;		//关系映射


	
};

/// <summary>
/// 数据库基类
/// 
/// 数据库的基础操作
/// 连接数据库
/// 增删查改
/// 
/// 两套流程
/// 1.连接 --> 执行 --> 处理结果 --> 关闭
/// 2.连接 --> 开启事务 --> 执行 --> 提交事务 --> 关闭
/// 
/// </summary>
class CDataBaseClient
{
public:
	CDataBaseClient() {}
	CDataBaseClient& operator=(const CDataBaseClient& client) = delete;
	CDataBaseClient(const CDataBaseClient& client) = delete;
	virtual ~CDataBaseClient()
	{

	}
	virtual int Connect(const KEYVALUE& loginMsg) = 0;
	//执行语句不返回结果: 增加 删除等不返回结果的sql语句
	virtual int Execute(const CBuffer& sql) = 0;
	//执行语句带返回结果: 查询等要返回结果
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="sql">sql语句</param>
	/// <param name="result">查询结果</param>
	/// <param name="table">查询结果输出的表</param>
	/// <returns></returns>
	virtual int Execute(const CBuffer& sql,Result& result,const _Table_& table) = 0;
	//开启事务
	virtual int StartTransaction() = 0;
	//提交事务
	virtual int CommitTransaction() = 0;
	//回滚事务
	virtual int RollbackTransaction() = 0;
	//是否连接
	virtual bool IsConnected() = 0;
	//关闭
	virtual int Close() = 0;
};


//表类
#define DECLARE_CLASS(name,base) class name:public base { \
public:\
virtual PTable Copy() const{return PTable(new name(*this));}\
name():base(){Name = #name;}


#define DECLARE_TABLE_CLASS_END() }};

//定义表列内容
#define DECLARE_FIELD_CLASS(chat_user_table,_Table_)

#define DECLARE_FIELD_CLASS_END() }};

