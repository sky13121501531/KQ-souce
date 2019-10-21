///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UDPUDPTsFetcher.h
// 创建者：gaoxd
// 创建时间：2010-07-07
// 内容描述：视频数据获取线程 基于UDP协议
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TsFetcher.h"
#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include <vector>
#include <string>
class TsSender;
class UdpTsSender;

class UDPTsFetcher : public TsFetcher
{
public:
	UDPTsFetcher(int deviceid);
	~UDPTsFetcher();
private:
	UDPTsFetcher();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	bool SetTsDeviceXml(std::string devicexml);

	bool SendTsDeviceXml();
	bool SendTsDeviceXmlForDVBC(std::string devXml);
	bool SendTsDeviceXmlForCTTB(std::string devXml);
	bool SendTsDeviceXmlForDVBS(std::string devXml);
	bool SendTsDeviceXmlForFMAM(std::string devXml);

	bool RebootCard();

	void IncreaseTaskNum();
	void DecreaseTaskNum();
	void StopHistoryTask();

	bool SetTsSendTask(TsSender* task);
	bool SetTsRoundTask(TsSender* task);
	void SetSendSwitch(bool sendswitch);		//控制是否发送数据
	void SetForDataToCodec(bool senddatatocodec);
	bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	void SetReSendSwitch(bool sendswitch);
	//
	void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist);
	
private:
	bool FlushHistoryDatum();
	void TimeConvert(time_t curtime,unsigned char *timebuf);//把系统时间信息插入到录像
	void PutSysTime(ACE_Task<ACE_MT_SYNCH>* task);
	void GetDeviceIDBySQL(std::string serviceID,vector<int> &vDevIDlist);
	std::string GetTVShowSize(std::string strsizeRet);
private:
	
	std::string CurDeviceXml;			//当前TS硬件层信息

	ACE_Thread_Mutex TaskPointerMutex;
	ACE_Thread_Mutex TaskMutex;
	ACE_Thread_Mutex TaskNumMutex;
	ACE_Thread_Mutex RealStreamMutex;
	static ACE_Thread_Mutex TaskRoundMutex;

	TsSender* TsSendTaskPointer;
	TsSender* TsSendRoundTaskPointer;
	std::vector<ACE_Task<ACE_MT_SYNCH>*> RecordTaskPointerVec;
	std::vector<ACE_Message_Block *> StrRoundDataPointerVec;
	
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	//

	//
	bool m_SendForMulti;
	UdpTsSender* m_pUdpTsSender;
	//

	bool bFlag;
	bool bSendSwtich;
	bool m_bsenddatatocodec;
	bool bSendOrder;
	int TaskNum;
	bool ReSendSwitch;
	bool NewPackageHead;		//新视频数据头处理
	int comamdtype;
};