///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TsFetcher.h
// 创建者：gaoxd
// 创建时间：2009-06-16
// 内容描述：视频数据获取线程
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/SOCK_Connector.h"
#include <vector>
#include <string>
#include "../Foundation/TypeDef.h"
class TsSender;

class TsFetcher : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TsFetcher(int deviceid);
	~TsFetcher();
private:
	TsFetcher();
public:
	virtual int Start() = 0;

	virtual int open(void*) = 0;

	virtual int svc() = 0;

	virtual int Stop() = 0;

	virtual bool SetTsDeviceXml(std::string devicexml) = 0;

	virtual bool SendTsDeviceXml() = 0;

	virtual void IncreaseTaskNum() = 0;
	virtual void DecreaseTaskNum() = 0;
	virtual void StopHistoryTask() = 0;

	virtual bool SetTsSendTask(TsSender* task) = 0;
	virtual bool SetTsRoundTask(TsSender* task) = 0;
	virtual void SetSendSwitch(bool sendswitch) = 0;		//控制是否发送数据
	virtual bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task) = 0;
	virtual bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task) = 0;
	virtual void SetReSendSwitch(bool sendswitch) = 0;
	virtual bool RebootCard() = 0;	
	virtual void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist) = 0;
	virtual void SetForDataToCodec(bool senddatatocodec) = 0;		
protected:
	int FindBegin(unsigned char* tsBuf,unsigned int tsLen);
protected:
	int DeviceId;
	int TunerID;
};