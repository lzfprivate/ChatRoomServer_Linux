#pragma once
#include <string.h>
#include "DataBase.h"
#include <mysql/mysql.h>


enum enDataType {
	TYPE_NULL=0,		//空
	TYPE_BOOL =1,		//bool
	TYPE_INT = 2,		//int
	TYPE_DATETIME = 4,	//时间
	TYPE_REAL = 8,		//小数
	TYPE_VARCHAR = 16,	//字符
	TYPE_TEXT=32,			//字符
	TYPE_BLOB=64
};

enum enColumn
{
	NOT_NULL = 1,
	PRIMARY_KEY = 2,
	DEFAULT = 4,
	AUTO_INCREAMENT = 8,
	CHECK = 16,
	UNIQUE = 32
};

class _mysql_field_ : public _Field_
{
public:

	//列属性
	CBuffer m_strName;			//属性名 姓名 性别 ...
	CBuffer m_strType;			//属性类型 int char varchar...
	CBuffer m_strSize;			//属性占用长度
	unsigned m_uAttr;			//属性的属性 NULL NOT NULL PRIVATE KEY UNIQUE等
	CBuffer m_strDefault;		//属性的默认值
	CBuffer m_strCheck;			//属性的约束条件 大于0 小于0 ...

	unsigned m_uCondition;

	

public:
	_mysql_field_() {}
	virtual ~_mysql_field_() {}
	_mysql_field_(int nType, CBuffer name, CBuffer type, CBuffer size, int attr, CBuffer defaultVal, CBuffer Check);
	_mysql_field_(const _mysql_field_& field);
	virtual CBuffer Create();
	//从字符串中加载
	virtual void LoadFromStr(const CBuffer& str);
	//表达式,用于where语句
	virtual CBuffer EqualExp();
	//将列中的数据转换成字符串
	virtual CBuffer toSqlStr();
	//获取列的全名 数据库.表.列名
	virtual operator const CBuffer() const;

private:
	//字符转16进制
	CBuffer Str2Hex(const CBuffer& str) const;
	

	unsigned m_nType;			//数据类型 与基类定义的m_strType作用相同 

};	

class _mysql_table_ : public _Table_
{
public:
	_mysql_table_();
	~_mysql_table_(){}
	_mysql_table_(const _mysql_table_&);

public:

	//以下操作均返回一个sql语句
	CBuffer Create() override;
	CBuffer Drop()override;
	CBuffer Insert(const _Table_& table)override;
	CBuffer Remove(const _Table_& table) override;
	//TODO:添加查找条件
	CBuffer Query(const CBuffer& condition = "")override;
	CBuffer Modify(const _Table_& table)override;

	//获取表的命名 数据库名.表名
	operator const CBuffer() const override;
	//复制表
	PTable Copy() const override;
	//清理可使用值
	virtual void ClearFieldUsed()override;


};


class CMySqlClient :
    public CDataBaseClient
{
public:
	CMySqlClient();
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
	virtual int Close();

private:
	MYSQL m_db;				//mysql数据库SDK对象
	bool m_bInit;			//初始化标识
};


#define DECLARE_TABLE_CLASS(name, base) \
class name :public base{ \
public: \
virtual PTable Copy() const { return PTable(new name(*this));} \
name() :base() { m_strName = #name;}
#define DECLARE_MYSQL_FIELD(ntype,name,attr,type,size,default_,check) \
{PField field(new _mysql_field_(ntype, #name,attr, type, size, default_,check)); \
FieldDefine.push_back(field);  Fields[#name] = field; }
#define DECLARE_TABLE_CLASS_END() };

