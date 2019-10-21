///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：Reactor.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：自定义的反应器类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Reactor.h"

class XMLReceiverServer;
class RealTimeVideoServer;
class RealTimeQualityServer;
class DeviceServer;

class MyReactor : public ACE_Task<ACE_MT_SYNCH>
{
public:
	MyReactor(void);
	virtual ~MyReactor(void);
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

	ACE_Reactor* GetReactor();
private:
	RealTimeVideoServer *pRealTimeVideoServer;
	RealTimeQualityServer* pRealTimeQualityServer;
	DeviceServer* pDeviceServer;
	XMLReceiverServer* pXMLReceiverServer;
};
