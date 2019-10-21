///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceStatusQueryTask.h
// 创建者：gaoxd
// 创建时间：2009-06-22
// 内容描述：通道状态查询
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

class DevStatusQueryTask : public DeviceIndependentTask
{
public:
	DevStatusQueryTask();
	DevStatusQueryTask(std::string strXML);
	virtual ~DevStatusQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
public:
	int GetMem();
	string GetDisk();
	int GetCPU();
    __int64 CompareFileTime(FILETIME time1, FILETIME time2);
public:
    int m_mem;
    string m_disk;
    int m_cpu;
};