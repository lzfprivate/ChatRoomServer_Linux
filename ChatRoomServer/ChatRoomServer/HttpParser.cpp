#include "HttpParser.h"

CHttpParser::CHttpParser()
{
}

CHttpParser::~CHttpParser()
{
}

size_t CHttpParser::Parser(const CBuffer& buffer)
{
    return size_t();
}

int CHttpParser::Method() const
{
    return m_parser.method;
}

const std::map<CBuffer, CBuffer>& CHttpParser::Header()
{
    // TODO: 在此处插入 return 语句
    return m_header2Value;
}

CBuffer CHttpParser::Status() const
{
    return m_bufStatus;
}

CBuffer CHttpParser::Url() const
{
    return m_bufUrl;
}

CBuffer CHttpParser::Body() const
{
    return m_bufBody;
}

unsigned CHttpParser::Errno() const
{
    return m_parser.http_errno;
}

int CHttpParser::onMessageBegin(http_parser* parser)
{
    return ((CHttpParser*)(parser->data))->MessageBegin();
}

int CHttpParser::onUrl(http_parser* parser, const char* data, size_t length)
{
    return 0;
}

int CHttpParser::onStatus(http_parser* parser, const char* data, size_t length)
{
    return 0;
}

int CHttpParser::onHeaderField(http_parser* parser, const char* data, size_t length)
{
    return 0;
}

int CHttpParser::onValueField(http_parser* parser, const char* data, size_t length)
{
    return 0;
}

int CHttpParser::onHeaderFinish(http_parser* parser)
{
    return 0;
}

int CHttpParser::onBody(http_parser* parser, const char* data, size_t length)
{
    return 0;
}

int CHttpParser::onMessageFinish(http_parser* parser)
{
    return 0;
}

int CHttpParser::onChunkHeader(http_parser* parser)
{
    return 0;
}

int CHttpParser::onChunkFinish(http_parser* parser)
{
    return 0;
}

int CHttpParser::MessageBegin()
{
    return 0;
}

int CHttpParser::Url(const char* data, size_t length)
{
    return 0;
}

int CHttpParser::Status(const char* data, size_t length)
{
    return 0;
}

int CHttpParser::HeaderField(const char* data, size_t length)
{
    return 0;
}

int CHttpParser::ValueField(const char* data, size_t length)
{
    return 0;
}

int CHttpParser::HeaderFinish()
{
    return 0;
}

int CHttpParser::Body(const char* data, size_t length)
{
    return 0;
}

int CHttpParser::MessageFinish()
{
    return 0;
}

int CHttpParser::ChunkHeader()
{
    return 0;
}

int CHttpParser::ChunkFinish()
{
    return 0;
}

CUrlParser::CUrlParser(const CBuffer& url):m_bufUrl(url)
{
}

CUrlParser::~CUrlParser()
{
}

int CUrlParser::Parser()
{
    return 0;
}

CBuffer CUrlParser::operator[](const CBuffer key)
{
    return CBuffer();
}

CBuffer CUrlParser::GetProtocol() const
{
    return m_bufProtocol;
}

CBuffer CUrlParser::GetHost() const
{
    return m_bufHost;
}

int CUrlParser::GetPort() const
{
    return m_nPort;
}

void CUrlParser::SetUrl(const CBuffer& buffer)
{
    m_bufUrl = buffer;
}
