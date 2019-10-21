///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceAccessMgr.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-09
// 内容描述：硬件设备访问管理类
///////////////////////////////////////////////////////////////////////////////////////////
#include "DeviceAccessMgr.h"
#include "DeviceAccess.h"
#include "HTTPDeviceAccess.h"
#include "TCPDeviceAccess.h"
#include "./ATVTcpDeviceAccess.h"
#include "./RADIOTcpDeviceAccess.h"
#include "./AMTcpDeviceAccess.h"
#include "../Foundation/PropManager.h"
#include "HttpOpe.h"
#include "../Foundation/StrUtil.h"

DeviceAccessMgr::DeviceAccessMgr()
{
}

DeviceAccessMgr::~DeviceAccessMgr()
{
	UnInit();
}

void DeviceAccessMgr::UnInit()
{
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.begin();
	for (; iter!=deviceAccessMap.end(); ++iter)
	{
		delete iter->second;
		iter->second = NULL;
	}

	deviceAccessMap.clear();
}

bool DeviceAccessMgr::SendTaskMsg(int deviceID, const std::string& strCmdMsg,std::string& strRetMsg)
{
	bool bRet = false;
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.find(deviceID);//根据通道号查找硬件设备

	if (iter == deviceAccessMap.end())
		return bRet;

	bRet = iter->second->SendTaskMsg(strCmdMsg,strRetMsg);//向硬件设备发送命令

	return bRet;
}

bool DeviceAccessMgr::SendMultiTask(int deviceID,std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex)
{
	bool bRet = false;
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.find(deviceID);//根据通道号查找硬件设备

	if (iter == deviceAccessMap.end())
		return bRet;

	bRet = iter->second->SendMultiTask(strtsIp,tsport,cardindex,MultiIndex);//向硬件设备发送命令

	return bRet;
}

bool DeviceAccessMgr::GetTsReceiveHandle(int deviceID,const std::string& strAddress,ACE_SOCK_Stream& streamHandle)
{
	bool bRet = false;
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.find(deviceID);//根据通道号查找硬件设备

	if (iter == deviceAccessMap.end())
		return bRet;

	bRet = iter->second->GetTsReceiveHandle(strAddress,streamHandle);//获得连接硬件设备的sokcet

	return bRet;
}
bool DeviceAccessMgr::CheckFreqLock(int deviceID)
{
	bool bRet = false;
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.find(deviceID);//根据通道号查找硬件设备

	if (iter == deviceAccessMap.end())
		return bRet;

	bRet = iter->second->CheckFreqLock();

	return bRet;
}
bool DeviceAccessMgr::SetSysTime(int deviceID)
{
	bool bRet = false;
	std::map<int,DeviceAccess*>::iterator iter = deviceAccessMap.find(deviceID);//根据通道号查找硬件设备

	if (iter == deviceAccessMap.end())
		return bRet;

	bRet = iter->second->SetSysTime();

	return bRet;
}


bool DeviceAccessMgr::GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle)
{
	bool bRet = false;
	std::string strUrl = strAddress;
	strUrl = strUrl.substr(strUrl.find_first_of("/"));
	stringstream stream(strUrl);
	std::string strTemp;
	std::string strIPAddress,strSubRoot;

	int i = 0;
	while (getline(stream,strTemp,'/'))
	{
		if (strTemp != "")
		{
			if (i == 0)
				strIPAddress = strTemp;
			else
				strSubRoot = strTemp;
			i++;
		}
	}

	std::string strIP = strIPAddress.substr(0,strIPAddress.find(":"));
	std::string port = strIPAddress.substr(strIPAddress.find(":")+1);

	HttpOpe httpOpe(strIP.c_str(),StrUtil::Str2Int(port));
	ACE_Guard<ACE_Thread_Mutex> guard(getTsMutex);
	if (httpOpe.GetRecvHandle(strSubRoot,streamHandle))
		bRet = true;

	return bRet;
}


