///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TCPTsFetcher.h
// 创建者：gaoxd
// 创建时间：2010-07-07
// 内容描述：视频数据获取线程 基于TCP协议
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TsFetcher.h"
#include "ace/Task.h"
#include "ace/SOCK_Connector.h"
#include <vector>
#include <string>

class TsSender;

class TCPTsFetcher : public TsFetcher
{
public:
	TCPTsFetcher(int deviceid);
	~TCPTsFetcher();
private:
	TCPTsFetcher();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	bool SetTsDeviceXml(std::string devicexml);

	bool SendTsDeviceXml();
	bool RebootCard(){return true;};	//暂时先不实现

	void IncreaseTaskNum();
	void DecreaseTaskNum();
	void StopHistoryTask();

	bool SetTsSendTask(TsSender* task);
	bool SetTsRoundTask(TsSender* task);
	void SetSendSwitch(bool sendswitch);		//控制是否发送数据
	bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	void SetReSendSwitch(bool sendswitch);
	//
	void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist)
	{
		m_SendForMulti = bMulti;
	}
private:
	std::string DeviceTaskXml;			//硬件初始化指令
	std::string CurDeviceXml;			//当前TS硬件层信息

	bool URLChanged;
	ACE_Thread_Mutex TaskPointerMutex;
	ACE_Thread_Mutex TaskMutex;
	ACE_Thread_Mutex TaskNumMutex;
	static ACE_Thread_Mutex TaskRoundMutex;

	TsSender* TsSendTaskPointer;
	TsSender* TsSendRoundTaskPointer;
	std::vector<ACE_Task<ACE_MT_SYNCH>*> RecordTaskPointerVec;
	
	ACE_SOCK_Stream DeviceSock;
	std::string URL;
	bool bFlag;
	bool bSendSwtich;
	int TaskNum;

	bool NewPackageHead;		//新视频数据头处理
	bool m_SendForMulti;
};