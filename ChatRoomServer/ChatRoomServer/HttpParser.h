#pragma once
/// <summary>
/// http数据解析类
/// </summary>

#include <map>
#include "Buffer.h"
#include "http_parser.h"
#include <cstddef>
class CHttpParser
{
public:
	CHttpParser();
	~CHttpParser();

public:
	//解析
	size_t Parser(const CBuffer& buffer);


	//获取模式
	int Method() const;
	const std::map<CBuffer, CBuffer>& Header();
	CBuffer Status() const;
	CBuffer Url() const;
	CBuffer Body() const;
	unsigned Errno() const;

	//对http重封装
protected:
	static int onMessageBegin(http_parser* parser);
	static int onUrl(http_parser* parser,const char* data,size_t length);
	static int onStatus(http_parser* parser, const char* data, size_t length);
	static int onHeaderField(http_parser* parser, const char* data, size_t length);
	static int onValueField(http_parser* parser, const char* data, size_t length);
	static int onHeaderFinish(http_parser* parser);
	static int onBody(http_parser* parser, const char* data, size_t length);
	static int onMessageFinish(http_parser* parser);
	static int onChunkHeader(http_parser* parser);
	static int onChunkFinish(http_parser* parser);

private:
	int MessageBegin();
	int Url(const char* data, size_t length);
	int Status(const char* data, size_t length);
	int HeaderField(const char* data, size_t length);
	int ValueField(const char* data, size_t length);
	int HeaderFinish();
	int Body(const char* data, size_t length);
	int MessageFinish();
	int ChunkHeader();
	int ChunkFinish();


private:
	http_parser		m_parser;		//http解析数据结构
	http_parser_settings m_settings;//http解析设置
	std::map<CBuffer,CBuffer>			m_header2Value;			//数据键值对
	CBuffer			m_bufUrl;			//完整地址
	CBuffer			m_bufStatus;
	CBuffer			m_bufBody;
	bool			m_bComplete;		//解析是否完成
	CBuffer			m_bufLastField;		//最后的一个模块



};

class CUrlParser
{
public:
	CUrlParser(const CBuffer& url);
	~CUrlParser();
	int Parser();

	//获取私有成员变量
	CBuffer operator[](const CBuffer key);
	CBuffer GetProtocol() const;
	CBuffer GetHost() const;
	int GetPort() const;
	CBuffer GetUri() const;
	//设置url
	void SetUrl(const CBuffer& buffer);


private:
	CBuffer m_bufUrl;				//完整URL
	CBuffer m_bufProtocol;			//协议
	CBuffer m_bufHost;				//主机地址
	CBuffer m_bufUri;				//uri
	int		m_nPort;				//端口
	std::map<CBuffer, CBuffer> m_values;//键值对
};

