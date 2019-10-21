///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceAccessMgr.h
// 创建者：jiangcheng
// 创建时间：2009-06-09
// 内容描述：硬件设备访问管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "ace/Message_Queue.h"
#include "ace/SOCK_Stream.h"
#include <map>

class DeviceAccess;
class DeviceAccessMgr
{
public:
	DeviceAccessMgr();
	virtual ~DeviceAccessMgr();
public:
	//输入通道号和下发的XML命令，得到硬件设备回复的XML信息
	bool SendTaskMsg(int deviceID, const std::string& strCmdMsg,std::string& strRetMsg);
	bool SendMultiTask(int deviceID,std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	//输入通道号和Ts流文件节点地址，得到访问该流的socket句柄信息
	bool GetTsReceiveHandle(int deviceID,const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	//输入Ts流文件地址，得到访问该流的socket句柄信息
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	//判断该通道是否锁定
	bool CheckFreqLock(int deviceID);					
	//设置卡时间
	bool SetSysTime(int deviceID);	

private:
	void UnInit();
private:
	std::map<int,DeviceAccess*> deviceAccessMap;
	ACE_Thread_Mutex getTsMutex;
};

typedef ACE_Singleton<DeviceAccessMgr,ACE_Mutex>  DEVICEACCESSMGR;