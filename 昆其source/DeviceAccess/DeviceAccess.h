///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceAccess.h
// 创建者：gaoxd
// 创建时间：2010-07-06
// 内容描述：硬件设备访问基类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include <string>

class DeviceAccess
{
public:
	DeviceAccess(int deviceid, std::string strIP,int nPort);
	virtual ~DeviceAccess(void);
protected:
	DeviceAccess(void);
public:
	virtual bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg) = 0;
	virtual bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex) = 0;
	virtual bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle) = 0;	
	virtual bool CheckFreqLock() = 0;					//判断该通道是否锁定
	virtual bool SetSysTime() = 0;					//判断该通道是否锁定
protected:
	ACE_Thread_Mutex sendTaskMsgMutex;
	int DeviceId;

	time_t m_tChanFixKeepTime; //上次检测时间,用于判断是否检测该通道锁定状态 默认3秒内不重复检测
	bool m_bIsFreqLock; //当前频道锁定状态

	std::string strIPAddress;
	int port;
};

