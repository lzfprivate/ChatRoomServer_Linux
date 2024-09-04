#include "PlayerServer.h"
#include <memory.h>
#include "MySqlClient.h"
#include "LoggerServer.h"
#include "HttpParser.h"
#include "Md5Encode.h"
using namespace std;
using namespace std::placeholders;

const char* md5_key = "";

#ifndef Err_Return(ret,retval) 
#define Err_Return(ret,retval) if(ret != 0) {TRACEE("errno:%d,errmsg:%s",errno,strerror(errno)); return retval;}
#endif // !Err_Return(ret,retval)


DECLARE_TABLE_CLASS(Login_user_mysql,_mysql_table_)
DECLARE_TABLE_CLASS_END()


CPlayerServer::CPlayerServer(unsigned nSize):
	m_iCount(nSize)
{
	
}

CPlayerServer::~CPlayerServer()
{
	if (m_db)
	{
		CDataBaseClient* db = m_db;
		m_db = nullptr;
		db->Close();
		delete db;
	}
	m_epoll.Close();
	m_pool.Close();
	for (auto it : m_mpClients)
	{
		delete it.second;
	}
	m_mpClients.clear();
}

int CPlayerServer::BusinessProcess(CProcess* process)
{
	//初始化数据库
	m_db = new CMySqlClient();
	if (!m_db) {
		TRACEE("data base create failed");
		return -1;
	}
	KEYVALUE args;
	args["host"] = "127.0.0.1";
	args["user"] = "张三";
	args["passwd"] = "12345678";
	args["db"] = "videoserver";
	args["port"] = "3306";
	int ret = -1;
	ret = m_db->Connect(args);
	Err_Return(ret, -1);

	Login_user_mysql tableUser;
	ret = m_db->Execute(tableUser.Create());
	Err_Return(ret, -2);
	ret = m_epoll.Create(m_iCount);
	Err_Return(ret, -3);
	ret = m_pool.Start(m_iCount);
	Err_Return(ret, -4);
	for (unsigned i = 0; i < m_iCount; ++i)
	{
		m_pool.AddTask(&CPlayerServer::ThreadFunc, this);
	}
	ret = SetRecvFunction(&CPlayerServer::Recv, this, std::placeholders::_1, std::placeholders::_2);
	Err_Return(ret, -5);
	ret = SetConnFunction(&CPlayerServer::Conn, this, std::placeholders::_1);
	Err_Return(ret, -6);
	int sock = 0;
	while (m_epoll != -1)
	{

	}
	return 0;
}

int CPlayerServer::ThreadFunc()
{
	int ret = 0;
	while (1)
	{
		EPEVENTS epEvents;
		ret = m_epoll.WaitEvents(epEvents);
		if (ret < 0) break;
		if (ret > 0)
		{
			for (int i = 0; i < ret; ++i)
			{
				if (epEvents[i].events & EPOLLERR) break;
				if (epEvents[i].events & EPOLLIN)
				{
					CSockBase* client = (CSockBase*)epEvents[i].data.ptr;
					CBuffer buffer;
					client->Recv(buffer);
					if (m_funcRecv)
						(*m_funcRecv)(client, buffer);
				}
			}
		}
	}
	return 0;
}

int CPlayerServer::Conn(CSockBase* client)
{
	//简单打印地址和端口
	return 0;
}

int CPlayerServer::Recv(CSockBase* client, const CBuffer& buffer)
{
	//解析http
	int ret = DecodeHttpRequest(buffer);
	CBuffer response = MakeHttpResponse(ret);
	client->Send(response);
	return 0;
}

int CPlayerServer::DecodeHttpRequest(const CBuffer& request)
{
	CHttpParser parser;
	int ret = parser.Parser(request);
	if (ret == 0 || parser.Errno() == 0)
	{
		TRACEE("Http Request parse failed");
		return -1;
	}
	if (parser.Method() == HTTP_GET)
	{
		//get处理
		CUrlParser urlParser("http://127.0.0.1" + parser.Url());
		ret = urlParser.Parser();
		if (ret != 0)
		{
			return -2;
		}
		CBuffer uri = urlParser.GetUri();
		if (uri == "login")
		{
			CBuffer time = urlParser["time"];
			CBuffer salt = urlParser["salt"];
			CBuffer user = urlParser["user"];
			CBuffer sign = urlParser["sign"];

			Login_user_mysql loginDb;
			Result result;
			CBuffer sql = loginDb.Query("user_name=\"" + user + "\"");
			ret = m_db->Execute(sql, result, loginDb);
			if (ret != 0)
			{
				return -3;
			}
			if (result.size() <= 0)
			{
				return -4;
			}
			if (result.size() > 1)
			{
				return -5;	
			}
			auto user_msg = result.front();
			CBuffer bufPasswd = *user_msg->m_FieldList["user_password"]->UnValueType.String;

			CBuffer encodeStr = time + user + salt + bufPasswd + md5_key;
			//通过md5加密
			CBuffer md5 = CMd5Encode::Encode(encodeStr);
			if (md5 == sign)
			{
				return 0;
			}
			return -6;
		}
	}
	else if(parser.Method() == HTTP_POST)
	{
		//处理post
	}
	return 0;
}

CBuffer CPlayerServer::MakeHttpResponse(int ret)
{
	return CBuffer();
}
