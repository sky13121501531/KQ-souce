///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceManager.h
// 创建者：gaoxd
// 创建时间：2011-04-22
// 内容描述：设备管理（重启）类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <windows.h>
#include <io.h>
#include <Tlhelp32.h>
#define MONITOR "monitor.exe"
#define VSCTTB "GD-C1000.exe"
#define MONITORPATH "c:\\vscttb\\monitor.exe"
#define VSCTTBPATH "c:\\vscttb\\GD-C1000.exe"
class DeviceManager : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DeviceManager();
	~DeviceManager();
public:
	int Start();
	int Stop();
	int open(void*);
	virtual int svc();
	void SetStatus(int Type,std::string action,std::string strTime,std::string strurl);
	bool UpVSCTTB(string strurl);
	int GetProcessIdByName(string szProcessName);
private:
	bool SetNextRunTime();
	bool RebootDevice();
	
private:
	bool bFlag;
	std::string ManageType;				//ManageType 取值范围 week,day,single;
	std::string ManageMonthday;
	std::string ManageWeekday;
	std::string ManageSingleday;
	std::string ManageTime;
	std::string ManageSingledatetime;
	eCheckType meCheckType;
	
	std::string mNextRunTime;
};
typedef  ACE_Singleton<DeviceManager,ACE_Mutex>  DEVICEMANGER;