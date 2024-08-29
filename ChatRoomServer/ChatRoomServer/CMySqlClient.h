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
	virtual void Close();
};

