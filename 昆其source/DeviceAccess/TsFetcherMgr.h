///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TsFetcherMgr.h
// 创建者：gaoxd
// 创建时间：2009-06-18
// 内容描述：数据获取线程管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"

#include <map>
#include "TsFetcher.h"

class TsFetcherMgr
{
public:
	TsFetcherMgr();
	virtual ~TsFetcherMgr();
public:
	int Start();
	int Stop();
public:
	bool SetTsDeviceXml(int deviceid,std::string devicexml);
	bool ReSendAllDeviceXml(int deviceid);

	bool RebootCard(int deviceid);

	bool SetTsSendTask(int deviceid,TsSender* task);

	//wz_0217	
	bool SetTsRoundTask(int deviceid, TsSender* task);
	//wz_0217
	
	void SetSendSwitch(int deviceid,bool sendswitch);
	void SetForDataToCodec(int deviceid,bool senddatatocodec);
	bool SetRecordTask(int deviceid,ACE_Task<ACE_MT_SYNCH>* task);
	bool DelRecordTask(int deviceid,ACE_Task<ACE_MT_SYNCH>* task);
	void IncreaseTaskNum(int deviceid);
	void DecreaseTaskNum(int deviceid);
	void StopHistoryTask(int deviceid);
	void SetReSendSwitch(int deviceid,bool sendswitch);
	//
	void SetMulti(int deviceid,bool Multi,std::vector<UdpSendTsAddr> UdpSendTsAddrlist);
private:
	std::map<int,TsFetcher*> TsFetcherMap;
};

typedef ACE_Singleton<TsFetcherMgr,ACE_Mutex>  TSFETCHERMGR;