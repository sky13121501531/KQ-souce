///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TsSender.h
// 创建者：gaoxd
// 创建时间：2009-06-16
// 内容描述：视频数据发送线程
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <vector>

class TsSender : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TsSender(int deviceid);
	~TsSender();
private:
	TsSender();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetSendSwitch(bool sendswitch);		//控制是否发送数据
	bool AddClient(sVedioUserInfo newclient);	//增加视频连接用户
	bool HasClient();							//判断是否有视频用户 
	bool GetAllClient(std::vector<sVedioUserInfo>& uservec);//获得所有视频连接用户
	bool StopAllClient();									//强制停止该通道视频用户

private:
	bool ClearAllClient();
	int ProcessMessage(ACE_Message_Block* mb);//ts数据发送至客户
private:
	int DeviceID;
	std::vector<sVedioUserInfo> ClientVec;
	ACE_Thread_Mutex QueueMutex;
	bool bFlag;
	bool bSendSwitch;

};
