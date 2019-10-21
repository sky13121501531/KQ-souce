
///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：HTTPDeviceAccess.h
// 创建者：gaoxd
// 创建时间：2010-07-06
// 内容描述：硬件设备访问类，基于HTTP协议
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DeviceAccess.h"

class HTTPDeviceAccess : public DeviceAccess
{
public:
	HTTPDeviceAccess(int deviceid,const std::string& strIP,int nPort);
	~HTTPDeviceAccess(void);
private:
	HTTPDeviceAccess(void);
public:
	bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg);
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	bool CheckFreqLock(){return true;};
	bool SetSysTime(){return true;};
};
