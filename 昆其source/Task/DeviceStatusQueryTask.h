///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceStatusQueryTask.h
// 创建者：gaoxd
// 创建时间：2009-06-22
// 内容描述：通道状态查询
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

class DeviceStatusQueryTask : public DeviceIndependentTask
{
public:
	DeviceStatusQueryTask();
	DeviceStatusQueryTask(std::string strXML);
	virtual ~DeviceStatusQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};