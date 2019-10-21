///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：CommunicationMgr.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：通信层接口管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "DownOrderReceive.h"
#include "XMLReceive.h"
#include "TaskXMLSend.h"
#include "TaskXMLReSend.h"
#include "Reactor.h"
#include "DeviceLog.h"

class CommunicationMgr
{
public:
	CommunicationMgr();
	virtual ~CommunicationMgr();
public:
	int Start();
	int Stop();
	DownOrderReceive* AccessOrderReceive() { if (NULL != pDownOrderReceive) return pDownOrderReceive; else return NULL;}
	TaskXMLSend* AccessTaskXMLSend() { if (NULL != pXMLSend) return pXMLSend; else return NULL;}
	TaskXMLReSend* AccessTaskXMLReSend() { if (NULL != pXMLReSend) return pXMLReSend; else return NULL;}
	MyReactor *AccessMyReactor() {if (NULL != pMyReactor) return pMyReactor; else return NULL;}
private:
	void CreateInstance();
	void DestoryInstance();
private:
	MyReactor *pMyReactor;
	DownOrderReceive *pDownOrderReceive;
	TaskXMLSend *pXMLSend;
	TaskXMLReSend* pXMLReSend;
	DeviceLog* pDeviceLog;
};

typedef ACE_Singleton<CommunicationMgr,ACE_Mutex>  COMMUNICATIONMGR;