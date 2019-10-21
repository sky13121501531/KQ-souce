
#include "OS_Environment.h"

#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"

#if defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
#else
#include <sys/vfs.h> 
#endif



OS_Environment::OS_Environment()
{
}

OS_Environment::~OS_Environment()
{

}

long OS_Environment::MemoryRealtimeUsage()
{
#if defined(WIN32) || defined(__WIN32__)
  MEMORYSTATUSEX statusEx;
  statusEx.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&statusEx);//获得系统内存信息
  long percentage = (long)statusEx.dwMemoryLoad;
//  printf("MemoryRealtimeUsage: %d\n",percentage);
  return percentage;
#else
  return 0;
#endif
}


long OS_Environment::CPURealtimeUsage()
{
#if defined(WIN32) || defined(__WIN32__)
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;//性能信息
	SYSTEM_TIME_INFORMATION        SysTimeInfo;//时间信息
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;//基本信息
	typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
	PROCNTQSI NtQuerySystemInformation;
	double                         dbIdleTime;
	double                         dbSystemTime;
	static LARGE_INTEGER                  liOldIdleTime = {0,0};
	static LARGE_INTEGER                  liOldSystemTime = {0,0};
	LONG                           status;
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");//加载ntdll库
	if (!NtQuerySystemInformation)      return 0;
	// 获得处理器个数
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);//获得系统的基本信息
	if (status != NO_ERROR)       return 0 ;
	//设置新的系统时间：
	status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);//获得系统的时间信息
	if (status!=NO_ERROR)      return 0;
	status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);//获得系统的性能信息
	if (status != NO_ERROR)    return 0; 
	dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);//获得系统空闲时间
	dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);//获得系统整个时间
	dbIdleTime = dbIdleTime / dbSystemTime;
	dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors;//计算系统的cpu使用率
	long percentage=(long)dbIdleTime;

	liOldIdleTime=SysPerfInfo.liIdleTime;//更新空闲时间
	liOldSystemTime=SysTimeInfo.liKeSystemTime;//更新系统时间

//	printf("CPURealtimeUsage: %d\n",percentage);

	return percentage;

#else
	return 0;
#endif
}