///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceStatusQueryTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-22
// ����������ͨ��״̬��ѯ
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