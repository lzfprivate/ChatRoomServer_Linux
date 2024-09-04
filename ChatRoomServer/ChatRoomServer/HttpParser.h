#pragma once
/// <summary>
/// http���ݽ�����
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
	//����
	size_t Parser(const CBuffer& buffer);


	//��ȡģʽ
	int Method() const;
	const std::map<CBuffer, CBuffer>& Header();
	CBuffer Status() const;
	CBuffer Url() const;
	CBuffer Body() const;
	unsigned Errno() const;

	//��http�ط�װ
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
	http_parser		m_parser;		//http�������ݽṹ
	http_parser_settings m_settings;//http��������
	std::map<CBuffer,CBuffer>			m_header2Value;			//���ݼ�ֵ��
	CBuffer			m_bufUrl;			//������ַ
	CBuffer			m_bufStatus;
	CBuffer			m_bufBody;
	bool			m_bComplete;		//�����Ƿ����
	CBuffer			m_bufLastField;		//����һ��ģ��



};

class CUrlParser
{
public:
	CUrlParser(const CBuffer& url);
	~CUrlParser();
	int Parser();

	//��ȡ˽�г�Ա����
	CBuffer operator[](const CBuffer key);
	CBuffer GetProtocol() const;
	CBuffer GetHost() const;
	int GetPort() const;
	CBuffer GetUri() const;
	//����url
	void SetUrl(const CBuffer& buffer);


private:
	CBuffer m_bufUrl;				//����URL
	CBuffer m_bufProtocol;			//Э��
	CBuffer m_bufHost;				//������ַ
	CBuffer m_bufUri;				//uri
	int		m_nPort;				//�˿�
	std::map<CBuffer, CBuffer> m_values;//��ֵ��
};

