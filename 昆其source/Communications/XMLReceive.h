///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：XMLReceive.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：接收应用系统下达XML格式的命令
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DownOrderReceive.h"
#include <string>

class XMLReceive : public DownOrderReceive
{
public:
	XMLReceive();
	~XMLReceive();
public:
	int svc();
private:
	void ProcessReceiveXML();
private:
	long lastMsgID;
};