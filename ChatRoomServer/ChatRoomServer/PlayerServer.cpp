#include "PlayerServer.h"
#include <memory.h>
#include <dirent.h>
#include "MySqlClient.h"
#include "LoggerServer.h"
#include "HttpParser.h"
#include "Md5Encode.h"
#include "Frame.h"
using namespace std;
using namespace std::placeholders;

const char* md5_key = "YARI\\a3QU>5DOKmYvAKTrf23s7EYtD_cwouUAa:ueT_Q`>rme[_\\Ui<RGr4";

#ifndef Err_Return(ret,retval) 
#define Err_Return(ret,retval) if(ret != 0) {TRACEE("errno:%d,errmsg:%s",errno,strerror(errno)); return retval;}
#endif // !Err_Return(ret,retval)


DECLARE_TABLE_CLASS(TableUsers, _mysql_table_)
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_id", "int", "", PRIMARY_KEY | NOT_NULL | AUTO_INCREAMENT, "", "")
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_account", "VARCHAR", "(15)", NOT_NULL, "", "")
DECLARE_MYSQL_FIELD(TYPE_VARCHAR, "user_passwd", "VARCHAR", "(15)", NOT_NULL, "", "")
DECLARE_MYSQL_FIELD(TYPE_TEXT, "user_sex", "TEXT", "", NOT_NULL, "男", "")
DECLARE_MYSQL_FIELD(TYPE_TEXT, "user_name", "TEXT", "", NOT_NULL, "", "")
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
	args["host"] = "192.168.0.11";
	args["user"] = "root";
	args["passwd"] = "123456";
	args["db"] = "ChatRoom";
	args["port"] = "3306";
	int ret = -1;
	ret = m_db->Connect(args);
	Err_Return(ret, -1);

	TableUsers tableUser;
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
	//修改协议
	CFrame frame;
	int ret = frame.Decode(buffer);
	if (ret != 0) {
		TRACEE("recv data decode error!!!\n");
		return -1;
	}
	CBuffer bufName;
	CBuffer strName;
	long long size;
	FILE* file;
	std::vector<CBuffer> fileList = GetFilesInFolder("./");
	switch (frame.m_iFrameFunc)
	{
	case 1:	
		DealLoginRequest(client, frame.m_bufFrame);
		
		break;
	case 2:
	case 3:
		ret = frame.Encode(2, buffer);
		break;
	case 4:				//收到文件大小和文件的名称
		size = *(long long*)frame.m_bufFrame.size();
		
		bufName.resize(frame.m_bufFrame.size() - sizeof(long long));
		memcpy((void*)bufName.c_str(), frame.m_bufFrame.c_str() + sizeof(long long), bufName.size());
		break;
	case 5:				//上传文件	存储在特定的路径下
		fopen(bufName, "wr+");
		fwrite(frame.m_bufFrame, 1, frame.m_bufFrame.size(), file);
		fclose(file);
		break;
	case 6:				//查询文件
		//TODO:查询特定路径下的所有文件，循环发送文件大小和目录
		//TODO:在发送完所有的文件大小和文件名后，再发送一个空的文件名，表示没有文件了
		for (int i = 0; i < fileList.size(); ++i)
		{
			frame.Encode(8, fileList[i].c_str());
			client->Send(frame.m_bufTotal);
		}
		break;
	case 7:				//发送要下载文件名称和大小
		size = *(long long*)frame.m_bufFrame.size();

		bufName.resize(frame.m_bufFrame.size() - sizeof(long long));
		memcpy((void*)bufName.c_str(), frame.m_bufFrame.c_str() + sizeof(long long), bufName.size());
		break;
	case 8:				//下载特定文件
		fopen(bufName, "r");
		char buf[10000];
		fread(buf, 1, 10000, file);
		frame.Encode(4, buf);
		break;
	default:
		break;
	}
	if(ret != 0){
		TRACEE("encode message error\n");
		return -2;
	}
	client->Send(frame.m_bufTotal);
	return 0;
	
	//解析http
	//int ret = DecodeHttpRequest(buffer);
	//CBuffer response = MakeHttpResponse(ret);
	//client->Send(response);
	//return 0;
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

			TableUsers loginDb;
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

std::vector<CBuffer> CPlayerServer::GetFilesInFolder(const CBuffer& folderPath)
{
	std::vector<CBuffer> fileList;
	DIR* dir;
	struct dirent* entry;

	// 打开文件夹
	dir = opendir(folderPath.c_str());
	if (dir == nullptr) {
		return fileList;
	}

	// 读取文件夹中的文件
	while ((entry = readdir(dir)) != nullptr) {
		//忽略当前目录和上级目录
		if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
			continue;
		}
		bool bFirst = true;
		if (entry->d_type == DT_DIR)
		{
			std::string nextdir;
			if (bFirst)
			{
				nextdir = folderPath + std::string(entry->d_name) + "/";
			}
			else {
				bFirst = false;
				nextdir = folderPath + "/" + std::string(entry->d_name);
			}

			printf("filefoldername:%s\n", nextdir.c_str());
			std::vector<CBuffer> nextfileFolder = GetFilesInFolder(nextdir);
			if (nextfileFolder.size() > 0)
			{
				for (size_t i = 0; i < nextfileFolder.size(); ++i)
				{
					fileList.push_back(nextfileFolder[i]);
				}
			}
		}
		else {
			// 将文件名添加到列表中
			fileList.push_back(entry->d_name);
		}
	}

	// 关闭文件夹
	closedir(dir);

	return fileList;

}
int CPlayerServer::DealLoginRequest(CSockBase* client,const CBuffer& buff)
{
	CFrame frame;
	int ret = 0;
	//解析md5

	char* temp = (char*)buff.c_str();
	int* len = (int*)temp;
	temp += sizeof(int);					//用户名长度

	CBuffer bufAccount(*len);
	memcpy((void*)bufAccount.c_str(), temp, *len);
	temp += (*len);							//用户名

	len = (int*)temp;						
	temp += sizeof(int);					//时间字符串长度

	CBuffer bufTime(*len);
	memcpy((void*)bufTime.c_str(), temp, *len);
	temp += (*len);							//时间字符串

	len = (int*)temp;
	temp += sizeof(int);					//噪声值长度

	CBuffer bufSalt(*len);
	memcpy((void*)bufSalt.c_str(), temp, *len);
	temp += (*len);							//噪声值字符串

	CBuffer bufPrivate(*len);
	memcpy((void*)bufPrivate.c_str(), temp, *len);
	temp += (*len);							//私有字符串

	CBuffer bufMd5Recv(*len);
	memcpy((void*)bufMd5Recv.c_str(), temp, *len);
	temp += (*len);							//md5值

	CBuffer bufCheck;

	int index = 0;
	do
	{
		TableUsers loginDb;
		Result result;
		CBuffer sql = loginDb.Query("user_account=" + bufAccount + "");
		ret = m_db->Execute(sql, result, loginDb);
		if (ret != 0)
		{
			index = -1;
		}
		if (result.size() <= 0)
		{
			index = -2;
		}
		if (result.size() > 1)
		{
			index = -3;
		}
		auto user_msg = result.front();
		CBuffer bufPasswd = *user_msg->m_FieldList["user_passwd"]->UnValueType.String;

		CBuffer encodeStr = bufAccount + bufTime + bufSalt + bufPasswd + md5_key;
		//通过md5加密
		CBuffer md5 = CMd5Encode::Encode(encodeStr);
		if (md5 == bufMd5Recv)
		{
			bufCheck = "1";
		}
		else {
			bufCheck = "0";
			index = -4;
		}
	} while (0);

	frame.Encode(0, bufCheck);
	if (0 != client->Send(frame.m_bufTotal))
	{
		index = -5;
	}
	return index;
}
