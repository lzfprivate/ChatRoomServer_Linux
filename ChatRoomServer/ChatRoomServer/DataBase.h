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
/// �򵥵����:���е����� Ҳ���Ǳ����
/// </summary>
class _Field_ {
public:
	_Field_() {}
	virtual ~_Field_() {}

public:
	virtual CBuffer Create() = 0;
	//���ַ����м���
	virtual void LoadFromStr(const CBuffer& str) = 0;
	//���ʽ,����where���
	virtual CBuffer EqualExp() = 0;
	//�����е�����ת�����ַ���
	virtual CBuffer toSqlStr() = 0;
	//��ȡ�е�ȫ�� ���ݿ�.��.����
	virtual operator const CBuffer() const = 0;

	//TODO:�����������
public:
	//������
	CBuffer m_strName;			//������ ���� �Ա� ...
	CBuffer m_strType;			//�������� int char varchar...
	CBuffer m_strSize;			//����ռ�ó���
	unsigned m_uAttr;			//���Ե����� NULL NOT NULL PRIVATE KEY UNIQUE��
	CBuffer m_strDefault;		//���Ե�Ĭ��ֵ
	CBuffer m_strCheck;			//���Ե�Լ������ ����0 С��0 ...

	unsigned m_uCondition;		//���ݿ������ʶ�� �����������Ĳ���
	

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
/// ���ݿ�����
/// ��װ��Ĳ���
/// 1.����
/// 2.ɾ����
/// 3.����в�������
/// 4.�޸ı��е�����
/// 5.�ӱ��ѯ����
/// 6.ɾ����������
/// </summary>

class _Table_
{
public:
	_Table_() {}
	virtual ~_Table_() {}
	//���²���������һ��sql���
	virtual CBuffer Create() = 0;
	virtual CBuffer Drop() = 0;
	virtual CBuffer Insert(const _Table_& table) = 0;
	virtual CBuffer Remove(const _Table_& table) = 0;
	virtual CBuffer Query(const CBuffer& condition = "") = 0;
	virtual CBuffer Modify(const _Table_& table) = 0;

	//��ȡ������� ���ݿ���.����
	virtual operator const CBuffer() const = 0;
	//���Ʊ�
	virtual PTable Copy() const = 0;
	//�����ʹ��ֵ
	virtual void ClearFieldUsed() = 0;



public:
	CBuffer m_strBelongDataBase;	//�����������ݿ���
	CBuffer m_strName;				//����
	FIELDARRAY	m_FieldDefine	;	//���ڴ洢���
	FILEDMAP	m_FieldList;		//��ϵӳ��


	
};

/// <summary>
/// ���ݿ����
/// 
/// ���ݿ�Ļ�������
/// �������ݿ�
/// ��ɾ���
/// 
/// ��������
/// 1.���� --> ִ�� --> ������ --> �ر�
/// 2.���� --> �������� --> ִ�� --> �ύ���� --> �ر�
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
	//ִ����䲻���ؽ��: ���� ɾ���Ȳ����ؽ����sql���
	virtual int Execute(const CBuffer& sql) = 0;
	//ִ���������ؽ��: ��ѯ��Ҫ���ؽ��
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="sql">sql���</param>
	/// <param name="result">��ѯ���</param>
	/// <param name="table">��ѯ�������ı�</param>
	/// <returns></returns>
	virtual int Execute(const CBuffer& sql,Result& result,const _Table_& table) = 0;
	//��������
	virtual int StartTransaction() = 0;
	//�ύ����
	virtual int CommitTransaction() = 0;
	//�ع�����
	virtual int RollbackTransaction() = 0;
	//�Ƿ�����
	virtual bool IsConnected() = 0;
	//�ر�
	virtual int Close() = 0;
};


//����
#define DECLARE_CLASS(name,base) class name:public base { \
public:\
virtual PTable Copy() const{return PTable(new name(*this));}\
name():base(){Name = #name;}


#define DECLARE_TABLE_CLASS_END() }};

//�����������
#define DECLARE_FIELD_CLASS(chat_user_table,_Table_)

#define DECLARE_FIELD_CLASS_END() }};

