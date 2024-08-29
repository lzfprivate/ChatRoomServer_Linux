#pragma once
#include <unistd.h>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include "Buffer.h"


/// <summary>
/// �򵥵����:���е����� Ҳ���Ǳ����
/// </summary>
class _Field_ {
public:
	_Field_() {}
	virtual ~_Field_()	{}

public:
	virtual void Create() = 0;
	//���ַ����м���
	virtual void LoadFromStr(const CBuffer& str) = 0;
	//���ʽ,����where���
	virtual CBuffer EqualExp() = 0;
	//�����е�����ת�����ַ���
	virtual CBuffer toSqlStr() = 0;
	//��ȡ�е�ȫ�� ���ݿ�.��.����
	virtual operator const CBuffer() const = 0;

	//TODO:�����������
protected:
	//������
	CBuffer m_strName;			//������ ���� �Ա� ...
	CBuffer m_strType;			//�������� int char varchar...
	CBuffer m_strSize;			//����ռ�ó���
	unsigned m_uAttr;			//���Ե����� NULL NOT NULL PRIVATE KEY UNIQUE��
	CBuffer m_strDefault;		//���Ե�Ĭ��ֵ
	CBuffer m_strCheck;			//���Ե�Լ������ ����0 С��0 ...

};

class _Table_;
using PTABLE = std::shared_ptr< _Table_>;
using PFIELD = std::shared_ptr<_Field_>;
using Result = std::list<_Table_>;
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
	virtual CBuffer Remove() = 0;
	virtual CBuffer Query() = 0;
	virtual CBuffer Modify() = 0;

	//��ȡ������� ���ݿ���.����
	virtual CBuffer FullName() const = 0;
	//���Ʊ�
	virtual PTABLE Copy() = 0;


private:
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
	virtual void Close() = 0;
};


//����
#define DECLARE_CLASS(name,base) class name:public base { \
public:\
virtual PTABLE Copy() const{return PTABLE(new name(*this));}\
name():base(){Name = #name;}


#define DECLARE_TABLE_CLASS_END() }};

//�����������
#define DECLARE_FIELD_CLASS(chat_user_table,_Table_)

#define DECLARE_FIELD_CLASS_END() }};

