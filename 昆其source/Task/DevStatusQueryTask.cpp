
#include "DevStatusQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../Foundation/PropManager.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
DevStatusQueryTask::DevStatusQueryTask() : DeviceIndependentTask()
{

}

DevStatusQueryTask::DevStatusQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

DevStatusQueryTask::~DevStatusQueryTask()
{

}
void DevStatusQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 设备工作状态查询任务执行 !\n"));
	SetRunning();

	RetValue = RUN_SUCCESS;
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE rootnode=psr.GetRootNode();
    std::list<string> Devlist;
    this->m_mem = GetMem();
    this->m_disk = GetDisk();
    this->m_cpu = GetCPU();
	string strDevInfo = string("0_cpu:") + StrUtil::Int2Str(m_cpu) + string("_mem:") + StrUtil::Int2Str(m_mem) + string("_disk:") + m_disk;
    if(strDevInfo!="")
    {
        Devlist.push_back(strDevInfo);
    }
    int IndexNum = PROPMANAGER::instance()->GetMonitorDevNum();
    for(int i = 0;i<IndexNum;++i)
    {
        sDeviceInfo sDevInfo;
        PROPMANAGER::instance()->GetDevMonitorInfo(i,sDevInfo);
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
        string strDevInfoCMD = "GET /device/info  HTTP/1.1  \r\n\r\n";
        string strRetDevInfo;
        pDeviceAccess->SendTaskMsg(strDevInfoCMD,strRetDevInfo);
        if(strRetDevInfo.size()<0)
        {
            continue;
        }
        if(strRetDevInfo.find("<html>")!=-1)
        {
            string strcpu,strmem;
            XmlParser psr;
            psr.Set_xml(strRetDevInfo);
            pXMLNODE node=psr.GetNodeFromPath("html/body");
            pXMLNODELIST nodeList = psr.GetNodeList(node);
            int count = nodeList->Size();
            for(int i =0;i<count;i++)
            {
                pXMLNODE childNode = psr.GetNextNode(nodeList);
                string name = psr.GetNodeName(childNode);
                if(name == "CPU")
                {
                    psr.GetAttrNode(childNode,"num",strcpu);
                }
                else if(name == "MEM")
                {
                    psr.GetAttrNode(childNode,"num",strmem);
                }
            }
			strDevInfo = sDevInfo.s_deviceindex + string("_cpu:") + strcpu + string("_mem:") + strmem + string("_disk:") + string("--");
			Devlist.push_back(strDevInfo);
        }
		delete pDeviceAccess;
    }
    if(Devlist.size()<0)
    {
        RetValue = RUN_FAILED;
    }
    SendXML(TranslateUpXML::TranslateDevStatusQueryTask(this,Devlist));//发送回复xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 设备工作状态查询任务停止 !\n"));
}

std::string DevStatusQueryTask::GetTaskName()
{
	return "设备工作状态查询任务";
}

std::string DevStatusQueryTask::GetObjectName()
{
	return std::string("DevStatusQuery");
}

int DevStatusQueryTask::GetMem()
{
	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	return ms.dwMemoryLoad;
}

string DevStatusQueryTask::GetDisk()
{
	int DiskCount = 0;
	DWORD DiskInfo = GetLogicalDrives();
	while (DiskInfo)
	{
		if (DiskInfo & 1)
		{
			++DiskCount;
		}
		DiskInfo = DiskInfo >> 1;
	}
	int DSLength = GetLogicalDriveStrings(0, NULL);
	char* DStr = new char[DSLength];
	GetLogicalDriveStrings(DSLength, DStr);

	int DType;
	int si = 0;
	BOOL fResult;
	DWORD64 i64FreeBytesToCaller;
	DWORD64 i64TotalBytes;//总容量
	DWORD64 i64FreeBytes;//剩余空间

	/*为了显示每个驱动器的状态，则通过循环输出实现，由于DStr内部保存的数据是A:\NULLB:\NULLC:\NULL，这样的信息，所以DSLength/4可以获得具体大循环范围*/
    string diskSpace="";
	for (int i = 0; i<DSLength / 4; ++i)
	{
		char dir[3]={DStr[si],':','\\'};
		DType = GetDriveType(DStr+i*4);
		if(DType == DRIVE_FIXED)//硬盘
		{
			char *tmp = &DStr[si];
			string tmpDStr = tmp;
			fResult = GetDiskFreeSpaceEx(tmpDStr.c_str(),(PULARGE_INTEGER)&i64FreeBytesToCaller,(PULARGE_INTEGER)&i64TotalBytes,(PULARGE_INTEGER)&i64FreeBytes);
			if(fResult)
			{
                if(diskSpace!="")
                {
                    diskSpace+=";";
                }
				float DiskFree = (float)i64FreeBytesToCaller / 1024 / 1024 /1024;
                diskSpace+=tmpDStr+StrUtil::Float2Str(DiskFree);
			}
			si += 4;
		}
	}
	return diskSpace;
}
int DevStatusQueryTask::GetCPU()
{
	HANDLE hEvent;
	BOOL res;
	BOOL ret = false;
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
	hEvent = CreateEventA(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled
	int cpu = -1;
	while (!ret) 
	{
		Sleep(5);
		WaitForSingleObject(hEvent, 1000);
		res = GetSystemTimes(&idleTime, &kernelTime, &userTime);

		__int64 idle = CompareFileTime(preidleTime, idleTime);
		__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
		__int64 user = CompareFileTime(preuserTime, userTime);
		__int64 tempcpu = (kernel + user - idle) * 100 / (kernel + user);
		cpu = (int)tempcpu;
		if(cpu>=0)
		{
			ret = true;
		}
		preidleTime = idleTime;
		prekernelTime = kernelTime;
		preuserTime = userTime;
	}
	CloseHandle(hEvent);
	return cpu;
}
__int64 DevStatusQueryTask::CompareFileTime(FILETIME time1, FILETIME time2)
{
    __int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
    __int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

    return (b - a);
}