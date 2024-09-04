#include "HttpParser.h"
#include <string.h>

CHttpParser::CHttpParser():m_bComplete(false)
{
    memset(&m_parser, 0, sizeof(m_parser));
    m_parser.data = this;
    http_parser_init(&m_parser, HTTP_REQUEST);
    m_settings.on_message_begin = &CHttpParser::onMessageBegin;
    m_settings.on_url = &CHttpParser::onUrl;
    m_settings.on_status = &CHttpParser::onStatus;
    m_settings.on_header_field = &CHttpParser::onHeaderField;
    m_settings.on_header_value = &CHttpParser::onValueField;
    m_settings.on_headers_complete = &CHttpParser::onHeaderFinish;
    m_settings.on_body = &CHttpParser::onBody;
    m_settings.on_message_complete = &CHttpParser::onMessageFinish;
    m_settings.on_chunk_header = &CHttpParser::onChunkHeader;
    m_settings.on_chunk_complete = &CHttpParser::onChunkFinish;
}

CHttpParser::~CHttpParser()
{
}

size_t CHttpParser::Parser(const CBuffer& buffer)
{
    m_bComplete = false;
    int ret = http_parser_execute(&m_parser, &m_settings, buffer.c_str(), buffer.size());
    if (m_bComplete == false)
    {
        m_parser.http_errno = 0x7f;
        return 0;
    }
    return ret;

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
    return ((CHttpParser*)(parser->data))->Url(data, length);
}

int CHttpParser::onStatus(http_parser* parser, const char* data, size_t length)
{
    return ((CHttpParser*)(parser->data))->Status(data, length);
}

int CHttpParser::onHeaderField(http_parser* parser, const char* data, size_t length)
{
    return ((CHttpParser*)(parser->data))->HeaderField(data, length);
}

int CHttpParser::onValueField(http_parser* parser, const char* data, size_t length)
{
    return ((CHttpParser*)(parser->data))->ValueField(data, length);
}

int CHttpParser::onHeaderFinish(http_parser* parser)
{
return ((CHttpParser*)(parser->data))->HeaderFinish();
}

int CHttpParser::onBody(http_parser* parser, const char* data, size_t length)
{
    return ((CHttpParser*)(parser->data))->Body(data, length);
}

int CHttpParser::onMessageFinish(http_parser* parser)
{
    return ((CHttpParser*)(parser->data))->MessageFinish();
}

int CHttpParser::onChunkHeader(http_parser* parser)
{
    return ((CHttpParser*)(parser->data))->ChunkHeader();
}

int CHttpParser::onChunkFinish(http_parser* parser)
{
    return ((CHttpParser*)(parser->data))->ChunkFinish();
}

int CHttpParser::MessageBegin()
{
    return 0;
}

int CHttpParser::Url(const char* data, size_t length)
{
    m_bufUrl = CBuffer(data, length);
    return 0;
}

int CHttpParser::Status(const char* data, size_t length)
{
    m_bufStatus = CBuffer(data, length);
    return 0;
}

int CHttpParser::HeaderField(const char* data, size_t length)
{
    m_bufLastField = CBuffer(data, length);
    return 0;
}

int CHttpParser::ValueField(const char* data, size_t length)
{
    m_header2Value[m_bufLastField] = CBuffer(data, length);
    return 0;
}

int CHttpParser::HeaderFinish()
{
    return 0;
}

int CHttpParser::Body(const char* data, size_t length)
{
    m_bufBody = CBuffer(data, length);
    return 0;
}

int CHttpParser::MessageFinish()
{
    m_bComplete = true;
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

CUrlParser::CUrlParser(const CBuffer& url) :m_bufUrl(url)
{
}

CUrlParser::~CUrlParser()
{
}

int CUrlParser::Parser()
{
    //协议 域名 端口 uri 键值对 
    //1 解析协议 http://
    const char* pos = m_bufUrl.c_str();
    const char* target = strstr(pos, "://");
    if (target == nullptr) return -1;
    m_bufProtocol = CBuffer(pos, target);
    pos += 3;//跳过://
    //找域名 www.baidu.com
    target = strstr(pos, "/");
    if (target == nullptr) {
        if (m_bufProtocol.size() + 3 >= m_bufUrl.size())
        {
            //没找到域名
            return -2;
        }
        //找到域名但没有后续内容www.baidu.com
        m_bufHost = CBuffer(pos, target);
        return 0;
    }
    CBuffer addr = CBuffer(pos, target);
    if (addr.size() == 0) return -3;
    //网络地址的格式是地址+端口 127.0.0.1:8000
    target = strchr(addr.c_str(),':');
    if (target)
    {
        //取出地址和端口
        m_bufHost = CBuffer(pos, target);
        m_nPort = atoi(CBuffer(target + 1, (char*)(addr.c_str() + addr.size())));
    }
    else {
        m_bufHost = addr;
    }
    return 0;
}

CBuffer CUrlParser::operator[](const CBuffer key)
{
    if(m_values.find(key) != m_values.end())
        return m_values[key];
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

CBuffer CUrlParser::GetUri() const
{
    return m_bufUri;
}

void CUrlParser::SetUrl(const CBuffer& buffer)
{
    m_bufUrl = buffer;
}
