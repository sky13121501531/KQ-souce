///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：BusinessIterfaceMgr.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-21
// 内容描述：业务接口层接口管理类
///////////////////////////////////////////////////////////////////////////////////////////
#include "BusinessIterfaceMgr.h"
#include "DownXMLProcess.h"
#include "../Communications/CommunicationMgr.h"
#include "ace/OS.h"

BusinessIterfaceMgr::BusinessIterfaceMgr()
{
	CreateInstance();
}

BusinessIterfaceMgr::~BusinessIterfaceMgr()
{
	DestoryInstance();
}

void BusinessIterfaceMgr::CreateInstance()
{
	pDownXMLProcess = new DownXMLProcess;
}

void BusinessIterfaceMgr::DestoryInstance()
{
	if (NULL != pDownXMLProcess)
	{
		delete pDownXMLProcess;
		pDownXMLProcess = NULL;
	}
}

int BusinessIterfaceMgr::Start()
{
	//初始化下行XML命令解析模块
		
	if (pDownXMLProcess->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)下行命令接收模块初始化错误!\n"));
		return -1;
	}
	else
		pDownXMLProcess->open(0);
		
	return 0;
}

int BusinessIterfaceMgr::Stop()
{
	if (NULL != pDownXMLProcess)
		pDownXMLProcess->Stop();

	return 0;
}

int BusinessIterfaceMgr::UplaodXML(const std::string& strXML)
{
	if (strXML.empty())
		return -1;
	//使用Message_Block分配内存，将xml拷贝至内存中
	ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
	memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
	mbXML->wr_ptr(strXML.length());

	COMMUNICATIONMGR::instance()->AccessTaskXMLSend()->putq(mbXML);//xml加入发送队列

	return 0;
}


