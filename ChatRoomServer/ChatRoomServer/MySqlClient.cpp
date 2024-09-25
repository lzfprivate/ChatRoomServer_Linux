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
	if (loginMsg.at("db") > 0)
	{
		Execute("use " + loginMsg.at("db") + ";");
	}
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

_mysql_table_::_mysql_table_(const _mysql_table_& table)
{
	printf("mysql table copy construct entry======\n");
	m_strBelongDataBase = table.m_strBelongDataBase;
	m_strName = table.m_strName;
	for (size_t i = 0; i < table.m_FieldDefine.size(); i++)
	{
		PFIELD field = PFIELD(new _mysql_field_(*(_mysql_field_*)table.m_FieldDefine[i].get()));
		m_FieldDefine.push_back(field);
		printf("copy construct input attr:%d======\n", table.m_FieldDefine[i]->m_uAttr);
		printf("copy construct output attr:%d======\n", m_FieldDefine[i]->m_uAttr);
		m_FieldList[field->m_strName] = field;
	}
}



_mysql_table_::operator const CBuffer() const
{
	if (m_strBelongDataBase.size() > 0)
	{
		return "'" + m_strBelongDataBase + "'." +
			"'" + m_strName + "'";
	}
	return 	"'" + m_strName + "'";
}

CBuffer _mysql_table_::Remove(const _Table_& table)
{
	CBuffer sql = "DELETE FROM " + (CBuffer)*this + " ";
	CBuffer Where = "";
	bool bFirst = true;
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		if (table.m_FieldDefine[i]->m_uCondition & SQL_CONDITION)
		{
			if (!bFirst) {
				sql += " AND ";
			}
			else {
				bFirst = false;
			}
			Where += (CBuffer)(*table.m_FieldDefine[i]) + " = " + table.m_FieldDefine[i]->toSqlStr();
		}
	}
	if (Where.size() > 0)
	{
		sql += " WHERE " + Where;
	}
	sql += ";";
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

_mysql_field_::_mysql_field_() : _Field_()
{
	m_nType = TYPE_NULL;
	UnValueType.Double = 0.0;
	printf("%s(%d):<%s> field construct\n", __FILE__, __LINE__, __FUNCTION__);
}

_mysql_field_::_mysql_field_(int nType, const CBuffer& name, const CBuffer& type,
	const CBuffer& size, unsigned attr, const CBuffer& defaultVal, const CBuffer& Check)
{
	m_nType = nType;
	m_strName = name;
	m_strType = type;
	m_strSize = size;
	m_uAttr = attr;
	m_strDefault = defaultVal;
}

_mysql_field_::_mysql_field_(const _mysql_field_& field): _Field_(field)
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

_mysql_field_& _mysql_field_::operator=(const _mysql_field_& field)
{
	if (&field != this)
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
	return *this;
}

CBuffer _mysql_field_::Create()
{
	//名称 类型 属性 长度
	CBuffer sql = "'" + m_strName + "' " + m_strType + m_strSize + " ";
	if (m_uAttr & NOT_NULL)
	{
		sql += "NOT NULL ";
	}
	else
	{
		sql += "NULL ";
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
		UnValueType.String = new CBuffer;
		*(UnValueType.String) = str;
		break;
	case TYPE_BLOB:				//16进行
		UnValueType.String = new CBuffer;
		*(UnValueType.String) = Str2Hex(str);
		break;
	default:
		//TRACEW("expected data type\n");
		break;
	}
}

CBuffer _mysql_table_::Create()
{
	/*CREATE TABLE IF NOT EXIST 表全名(
	* 列名 列名的相关属性...,
	* PRIMARY KEY '列名',
	* UNIQUE INDEX '列名_UNIQUE' (列名 ASC) VISIBLE
	)ENGINE=INNODB;
	*
	*/
	
	CBuffer sql = "CREATE TABLE IF NOT EXIST " + (CBuffer)*this + "(\r\n";
	bool bFirst = true;
	for (int i = 0; i < m_FieldDefine.size(); ++i)
	{
		if (i > 0)
		{
			//除第一个属性和最后一个属性不添加\r\n,中间的属性添加
			sql += ",\r\n";
		}
		sql += m_FieldDefine[i]->Create();
		printf("attr:%d-----------------\n", m_FieldDefine[i]->m_uAttr);
		if (m_FieldDefine[i]->m_uAttr & PRIMARY_KEY)
		{
			sql += ",\r\nPRIMARY KEY ('" + m_FieldDefine[i]->m_strName + "')";
		}
		if (m_FieldDefine[i]->m_uAttr & UNIQUE)
		{
			sql += ",\r\nUNIQUE INDEX '" + m_FieldDefine[i]->m_strName + "_UNIQUE' (";
			sql += (CBuffer)(*m_FieldDefine[i])+ " ASC) VISIBLE ";
		}
	}
	sql += ")ENGINE=INNODB;";
	return sql;
}

CBuffer _mysql_table_::Drop()
{
	CBuffer sql = "DROP TABLE " + (CBuffer)*this + ";";
	return sql;
}

CBuffer _mysql_table_::Insert(const _Table_& table)
{
	/*INSERT INTO 表全名 VALUES(值1 值2 ...值n)*/
	CBuffer sql = "INSERT INTO " + CBuffer(*this) + " (";
	bool bFirst = true;
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		printf("%s(%d):<%s> table attr = %d\n"
			, __FILE__, __LINE__, __FUNCTION__, 
			table.m_FieldDefine[i]->m_uAttr);
		if (table.m_FieldDefine[i]->m_uCondition & SQL_INSERT)
		{
			printf("%s(%d):<%s> insert table num = %d\n", __FILE__, __LINE__, __FUNCTION__, table.m_FieldDefine.size());
			if (!bFirst) {
				sql += ",";
			}
			else {
				bFirst = false;
			}
			sql += table.m_FieldDefine[i]->m_strName;
		}
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
		if (i > 0)
		{
			sql += ",";
		}
		sql += "'" + m_FieldDefine[i]->m_strName + "' ";
	}
	sql += "FROM " + (CBuffer)*this;
	if(condition.size() > 0)
		sql += "WHERE " + condition;

	return sql;
}

CBuffer _mysql_table_::Modify(const _Table_& table)
{
	//UPDATE 表全名 SET 列=值1 ... WHERE ()
	CBuffer sql = "UPDATE " + (CBuffer)*this + "SET ";
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		if (table.m_FieldDefine[i]->m_uAttr & SQL_MODIFY)
		{
			sql += table.m_FieldDefine[i]->m_strName + "="
				+ table.m_FieldDefine[i]->toSqlStr();
		}
	}
	CBuffer Where = "";
	for (int i = 0; i < table.m_FieldDefine.size(); ++i)
	{
		if (i > 0) Where += " AND ";
		if (table.m_FieldDefine[i]->m_uAttr & SQL_CONDITION)
		{
			sql += table.m_FieldDefine[i]->m_strName + "="
				+ table.m_FieldDefine[i]->toSqlStr();
		}
	}
	if (Where.size() > 0)
	{
		sql += "WHERE " + Where;
	}
	sql += ";";
	return sql;
}

CBuffer _mysql_field_::EqualExp()
{
	CBuffer sql = (CBuffer)*this + "=";
	std::stringstream ss;
	switch (m_nType)
	{
	case TYPE_NULL:
		sql += " NULL ";
		break;
	case TYPE_BOOL:
	case TYPE_INT:
	case TYPE_DATETIME:
		ss << UnValueType.Integer;
		sql += ss.str() + " ";
		break;
	case TYPE_REAL:
		ss << UnValueType.Double;
		sql += ss.str() + " ";
		break;
	case TYPE_VARCHAR:
	case TYPE_TEXT:
	case TYPE_BLOB:
		ss << "\"" + *UnValueType.String + "\" ";
		sql += ss.str();
		break;
	default:
		break;
	}
	return sql;
}

CBuffer _mysql_field_::toSqlStr()
{
	CBuffer sql = "";
	std::stringstream ss;
	switch (m_nType)
	{
	case TYPE_NULL:
		sql += " NULL ";
		break;
	case TYPE_BOOL:
	case TYPE_INT:
	case TYPE_DATETIME:
		ss << UnValueType.Integer;
		sql += ss.str() + " ";
		break;
	case TYPE_REAL:
		ss << UnValueType.Double;
		sql += ss.str() + " ";
		break;
	case TYPE_VARCHAR:
	case TYPE_TEXT:
	case TYPE_BLOB:
		ss << "\"" + *UnValueType.String + "\" ";
		sql += ss.str();
		break;
	default:
		break;
	}
	return sql;
}

_mysql_field_::operator const CBuffer() const
{
	return "'" + m_strName + "'";
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




