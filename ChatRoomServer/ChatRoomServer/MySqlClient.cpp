#include "MySqlClient.h"
#include "Log.h"
#include <sstream>

CMySqlClient::CMySqlClient():m_bInit(false)
{
	bzero(&m_db, sizeof(m_db));
}

int CMySqlClient::Connect(const KEYVALUE& loginMsg)
{
	if (m_bInit) return -1;
	MYSQL* ret = mysql_init(&m_db);
	if (ret == nullptr) return -2;
	//调用mysqlSDK连接和错误处理
	ret = mysql_real_connect(&m_db, loginMsg.at("host"), loginMsg.at("user"), loginMsg.at("passwd"),
		loginMsg.at("db"),atoi(loginMsg.at("port").c_str()), nullptr, 0);
	if (ret == nullptr && mysql_errno(&m_db) != 0) {
		mysql_close(&m_db);
		bzero(&m_db, sizeof(m_db));
		
		return -3;
	}
	m_bInit = true;
	return 0;
}

int CMySqlClient::Execute(const CBuffer& sql)
{
	//调用MYSQL SDK执行
	if (!m_bInit) return -1;
	if (0 != mysql_real_query(&m_db, sql, sql.size()))
	{
		return -2;
	}
	return 0;
}

int CMySqlClient::Execute(const CBuffer& sql, Result& result,
	const _Table_& table)
{
	//调用MYSQL SDK执行
	if (!m_bInit) return -1;
	if (0 != mysql_real_query(&m_db, sql, sql.size()))
	{
		return -2;
	}
	//获取的查询结果
	MYSQL_RES* resultQuery = mysql_store_result(&m_db);
	//获取查询结果的总个数
	int resultSize = mysql_num_fields(resultQuery);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(resultQuery)) != 0) {
		//将数据丢入到表结构中
		PTable pTable = table.Copy();
		for (size_t i = 0; i < resultSize; i++)
		{
			if (row[i] != NULL)
			{
				pTable->m_FieldDefine[i]->LoadFromStr(row[i]);
			}
			result.push_back(pTable);
		}
	}
	return 0;
}

int CMySqlClient::StartTransaction()
{
	//转换数据 调用MYSQL SDK
	if (!m_bInit) return -1;
	if (0 != mysql_real_query(&m_db, "BEGIN", 6))
	{
		return -2;
	}
	return 0;
}

int CMySqlClient::CommitTransaction()
{
	if (!m_bInit) return -1;
	if (0 != mysql_real_query(&m_db, "COMMIT", 7))
	{
		return -2;
	}
	return 0;
}

int CMySqlClient::RollbackTransaction()
{
	if (!m_bInit) return -1;
	if (0 != mysql_real_query(&m_db, "ROLLBACK", 9))
	{
		return -2;
	}
	return 0;
}

bool CMySqlClient::IsConnected()
{
	return m_bInit;
}

int CMySqlClient::Close()
{
	if (m_bInit) {
		m_bInit = false;
		mysql_close(&m_db);
		bzero(&m_db, sizeof(m_db));
	}
	return 0;

}
_mysql_table_::_mysql_table_()
{
}

_mysql_table_::_mysql_table_(const _mysql_table_&)
{
}


_mysql_table_::operator const CBuffer() const
{
	return "'" + m_strBelongDataBase + "'." + m_strName;
}

CBuffer _mysql_table_::Remove(const _Table_& table)
{
	CBuffer sql;
	//if (condition.size() > 0)
	//{
	//	sql = "DELETE FROM " + (CBuffer)*this + "WHERE(" + condition + ");";
	//}
	//else
	//{
	//	sql = "DELETE * FROM " + (CBuffer)*this + "WHERE(" + condition + ");";
	//}
	return sql;
}

PTable _mysql_table_::Copy() const
{
	return PTable(new _mysql_table_(*this));
}

void _mysql_table_::ClearFieldUsed()
{
	for (size_t i = 0; i < m_FieldDefine.size(); i++)
	{
		m_FieldDefine[i]->m_uCondition = 0;
	}
}

_mysql_field_::_mysql_field_(int nType, CBuffer name, CBuffer type, 
	CBuffer size, int attr, CBuffer defaultVal, CBuffer Check)
{
	m_nType = nType;
	m_strName = name;
	m_strType = type;
	m_strSize = size;
	m_uAttr = attr;
	m_strDefault = defaultVal;
}

_mysql_field_::_mysql_field_(const _mysql_field_& field)
{
	m_nType = field.m_nType;
	switch (m_nType)
	{
	case TYPE_VARCHAR:
	case TYPE_TEXT:
	case TYPE_BLOB:
		UnValueType.String = new CBuffer;
		*UnValueType.String = *field.UnValueType.String;
		break;
	default:
		break;
	}
	m_strName = field.m_strName;
	m_strType = field.m_strType;
	m_strSize = field.m_strSize;
	m_uAttr = field.m_uAttr;
	m_strDefault = field.m_strDefault;
	m_strCheck = field.m_strCheck;
}

CBuffer _mysql_field_::Create()
{
	//名称 类型 属性 长度
	CBuffer sql = m_strName + " " + m_strType + " " + m_strSize + " ";

	if (m_uAttr & NOT_NULL)
	{
		sql += "NOT NULL ";
	}
	else
	{
		sql += "NULL ";
	}
	if (m_uAttr & PRIMARY_KEY)
	{
		sql += "PRIMARY KEY ";
	}
	if ((m_uAttr & DEFAULT) && m_strDefault.size() > 0 
		&& m_strType != "BLOB" && m_strType != "TEXT" 
		&& m_strType != "GEOMETRY" && m_strType != "JSON")
	{
		//BLOB TEXT GEOMETRY JSON不能有默认值的
		sql += "DEFAULT \"" + m_strDefault + "\" ";
	}
	if (m_uAttr & AUTO_INCREAMENT)
	{
		sql += "AUTO_INCREAMENT ";
	}
	sql += ",\r\n";
	return sql;
}

void _mysql_field_::LoadFromStr(const CBuffer& str)
{
	switch (m_nType)
	{
	case TYPE_NULL:
		break;
	case TYPE_BOOL:
	case TYPE_INT:
	case TYPE_DATETIME:
		UnValueType.Integer = atoi(str.c_str());
		break;
	case TYPE_REAL:
		UnValueType.Double = atof(str.c_str());
		break;
	case TYPE_VARCHAR:
	case TYPE_TEXT:
		*(UnValueType.String) = str;
		break;
	case TYPE_BLOB:				//16进行
		*(UnValueType.String) = Str2Hex(str);
		break;
	default:
		//TRACEW("expected data type\n");
		break;
	}
}

CBuffer _mysql_table_::Create()
{
	/*CREATE TABLE  IF NOT EXIST name(
	* column1 type not null primary key ...
	* ...
	* ...
	)ENGINE=INNODB;
	*
	*/
	CBuffer sql = "CREATE TABLE IF NOT EXIST" + (CBuffer)*this + "(\r\n";
	bool bFirst = true;
	for (int i = 0; i < m_FieldDefine.size(); ++i)
	{
		if (!bFirst)
		{
			//除第一个属性和最后一个属性不添加\r\n,中间的属性添加
			sql += ",\r\n";
		}
		sql += m_FieldDefine[i]->Create();
		if (m_FieldDefine[i]->m_uAttr & PRIMARY_KEY)
		{
			sql += "PRIMARY KEY ";
		}
		if (m_FieldDefine[i]->m_uAttr & NOT_NULL)
		{
			sql += "NOT NULL ";
		}
		if (m_FieldDefine[i]->m_uAttr & PRIMARY_KEY)
		{
			sql += "PRIMARY KEY ";
		}
		if (m_FieldDefine[i]->m_uAttr & UNIQUE)
		{
			sql += "UNIQUE INDEX ";
		}
		if (m_FieldDefine[i]->m_uAttr & AUTO_INCREAMENT)
		{
			sql += "AUTO_INCREAMENT";
		}
	}
	sql += ");";
	return sql;
}

CBuffer _mysql_table_::Drop()
{
	CBuffer sql = "DROP TABLE " + (CBuffer)*this + ";";
	return sql;
}

CBuffer _mysql_table_::Insert(const _Table_& table)
{
	/*insert into table values(值1 值2 ...值n)*/
	CBuffer sql = "INSERT INTO " + CBuffer(*this) + "(";
	bool bFirst = true;
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		if (!bFirst) {
			sql += ",";
		}
		else {
			bFirst = false;
		}
		sql += table.m_FieldDefine[i]->m_strName;
	}
	sql += ")VALUES(";
	bFirst = true;
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		if (table.m_FieldDefine[i]->m_uCondition & SQL_INSERT)
		{
			if (!bFirst) {
				sql += ",";
			}
			else {
				bFirst = false;
			}
			sql += table.m_FieldDefine[i]->toSqlStr();
		}
	}
	sql += ");";
	return sql;
}

CBuffer _mysql_table_::Query(const CBuffer& condition)
{
	/*
	* SELECT * FROM table WHERE (condition)
	*/
	CBuffer sql;
	sql += "SELECT ";
	for (size_t i = 0; i < m_FieldDefine.size(); i++)
	{
		sql += m_FieldDefine[i].get()->m_strName;
	}
	sql += "FROM " + (CBuffer)*this + ";";

	return sql;
}

CBuffer _mysql_table_::Modify(const _Table_& table)
{
	return CBuffer();
}

CBuffer _mysql_field_::EqualExp()
{
	return CBuffer();
}

CBuffer _mysql_field_::toSqlStr()
{
	//switch (m_nType)
	//{
	//case TYPE_NULL:
	//	break;
	//case TYPE_BOOL:
	//case TYPE_INT:
	//case TYPE_DATETIME:
	//	UnValueType.Integer = atoi(str.c_str());
	//	break;
	//case TYPE_REAL:
	//	UnValueType.Double = atof(str.c_str());
	//	break;
	//case TYPE_VARCHAR:
	//case TYPE_TEXT:
	//	UnValueType.String = &str;
	//	break;
	//case TYPE_BLOB:				//16进行
	//	UnValueType.String = &(Str2Hex(str));
	//	break;
	//default:
	//	
	//	break;
	//}
	return CBuffer();
}

_mysql_field_::operator const CBuffer() const
{
	return m_strName;
}

CBuffer _mysql_field_::Str2Hex(const CBuffer& str) const
{
	const char* hex = "0123456789ABCDEF";
	std::stringstream ss;
	for (int i = 0; i < str.size(); ++i)
	{
		unsigned char high = str.at(i) >> 4;
		unsigned char low = str.at(i) >> 0xF;
		ss << hex[high] << hex[low];
	}
	return CBuffer(ss.str());
}




