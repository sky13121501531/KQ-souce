///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：CommunicationMgr.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：通信层接口管理类
///////////////////////////////////////////////////////////////////////////////////////////
#include "CommunicationMgr.h"

CommunicationMgr::CommunicationMgr()
{
	CreateInstance();
}

CommunicationMgr::~CommunicationMgr()
{
	DestoryInstance();
}

void CommunicationMgr::CreateInstance()
{
	pDownOrderReceive = new XMLReceive;
	pMyReactor = new MyReactor;
	pXMLSend = new TaskXMLSend;
	pXMLReSend = new TaskXMLReSend;
	pDeviceLog = new DeviceLog;
}

void CommunicationMgr::DestoryInstance()
{
	//移除下行命令接收模块
	if (NULL != pDownOrderReceive)
	{
		delete pDownOrderReceive;
		pDownOrderReceive = NULL;
	}

	//移除反应器
	if (NULL != pMyReactor)
	{
		delete pMyReactor;
		pMyReactor = NULL;
	}

	//移除上行XML结果发送模块
	if (NULL != pXMLSend)
	{
		delete pXMLSend;
		pXMLSend = NULL;
	}
	if (NULL != pXMLReSend)
	{
		delete pXMLReSend;
		pXMLReSend = NULL;
	}
	//移除硬件层日志服务
	if (NULL != pDeviceLog)
	{
		delete pDeviceLog;
		pDeviceLog = NULL;
	}
}

int CommunicationMgr::Start()
{
	//初始化下行命令接收模块
	if (pDownOrderReceive->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)下行命令接收模块初始化错误!\n"));
		return -1;
	}
	else
		pDownOrderReceive->open(0);

	//初始化反应器
	if (pMyReactor->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)反应器初始化错误!\n"));
		return -1;
	}
	else
		pMyReactor->open(0);

	//初始化上行XML结果发送模块
	if (pXMLSend->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)发送线程初始化错误!\n"));
		return -1;
	}
	else
		pXMLSend->open(0);

	if (pXMLReSend->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)发送线程初始化错误!\n"));
		return -1;
	}
	else
		pXMLReSend->open(0);

	//硬件层日志服务
	if (pDeviceLog != NULL)
	{
		pDeviceLog->open(0);
	}
	return 0;
}

int CommunicationMgr::Stop()
{
	//停止下行命令接收模块
	if (NULL != pDownOrderReceive)
		pDownOrderReceive->Stop();

	//停止反应器
	if (NULL != pMyReactor)
		pMyReactor->Stop();

	//停止上行XML结果发送模块
	if (NULL != pXMLSend)
		pXMLSend->Stop();

	if (NULL != pXMLReSend)
		pXMLReSend->Stop();
	//硬件层日志服务
	if (pDeviceLog != NULL)
		pDeviceLog->Stop();
	return 0;
}