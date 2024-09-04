#pragma once
#include <string.h>
#include "DataBase.h"
#include <mysql/mysql.h>


enum enDataType {
	TYPE_NULL=0,		//��
	TYPE_BOOL =1,		//bool
	TYPE_INT = 2,		//int
	TYPE_DATETIME = 4,	//ʱ��
	TYPE_REAL = 8,		//С��
	TYPE_VARCHAR = 16,	//�ַ�
	TYPE_TEXT=32,			//�ַ�
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

	//������
	CBuffer m_strName;			//������ ���� �Ա� ...
	CBuffer m_strType;			//�������� int char varchar...
	CBuffer m_strSize;			//����ռ�ó���
	unsigned m_uAttr;			//���Ե����� NULL NOT NULL PRIVATE KEY UNIQUE��
	CBuffer m_strDefault;		//���Ե�Ĭ��ֵ
	CBuffer m_strCheck;			//���Ե�Լ������ ����0 С��0 ...

	unsigned m_uCondition;

	

public:
	_mysql_field_() {}
	virtual ~_mysql_field_() {}
	_mysql_field_(int nType, CBuffer name, CBuffer type, CBuffer size, int attr, CBuffer defaultVal, CBuffer Check);
	_mysql_field_(const _mysql_field_& field);
	virtual CBuffer Create();
	//���ַ����м���
	virtual void LoadFromStr(const CBuffer& str);
	//���ʽ,����where���
	virtual CBuffer EqualExp();
	//�����е�����ת�����ַ���
	virtual CBuffer toSqlStr();
	//��ȡ�е�ȫ�� ���ݿ�.��.����
	virtual operator const CBuffer() const;

private:
	//�ַ�ת16����
	CBuffer Str2Hex(const CBuffer& str) const;
	

	unsigned m_nType;			//�������� ����ඨ���m_strType������ͬ 

};	

class _mysql_table_ : public _Table_
{
public:
	_mysql_table_();
	~_mysql_table_(){}
	_mysql_table_(const _mysql_table_&);

public:

	//���²���������һ��sql���
	CBuffer Create() override;
	CBuffer Drop()override;
	CBuffer Insert(const _Table_& table)override;
	CBuffer Remove(const _Table_& table) override;
	//TODO:��Ӳ�������
	CBuffer Query(const CBuffer& condition = "")override;
	CBuffer Modify(const _Table_& table)override;

	//��ȡ������� ���ݿ���.����
	operator const CBuffer() const override;
	//���Ʊ�
	PTable Copy() const override;
	//�����ʹ��ֵ
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
	//ִ����䲻���ؽ��: ���� ɾ���Ȳ����ؽ����sql���
	virtual int Execute(const CBuffer& sql);
	//ִ���������ؽ��: ��ѯ��Ҫ���ؽ��
	virtual int Execute(const CBuffer& sql, Result& result, const _Table_& table);
	//��������
	virtual int StartTransaction();
	//�ύ����
	virtual int CommitTransaction();
	//�ع�����
	virtual int RollbackTransaction();
	//�Ƿ�����
	virtual bool IsConnected();
	//�ر�
	virtual int Close();

private:
	MYSQL m_db;				//mysql���ݿ�SDK����
	bool m_bInit;			//��ʼ����ʶ
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

