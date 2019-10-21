///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：HttpOpe.h
// 创建者：jiangcheng
// 创建时间：2009-06-05
// 内容描述：与硬件Http服务通信类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <string>

class HttpOpe
{
public:
	HttpOpe(const char* hostName,int port);
	virtual ~HttpOpe();
public:
	bool PostHttpMessage(const std::string& strSubRoot,const std::string& strCmdMsg,std::string& strRetMsg);//post方式发送http消息
	bool GetRecvHandle(const std::string& strSubRoot,ACE_SOCK_Stream& socketHandle,bool IsRecvXmlHeader=true);//获得socket
private:
	std::string PostHttpHeader(const std::string& strSubRoot,int len);//post http请求头
	std::string GetHttpHeader(const std::string& strSubRoot);//构造http请求头
private:
	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
};