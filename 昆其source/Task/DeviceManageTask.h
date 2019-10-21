#pragma once
#include "DeviceIndependentTask.h"
#include <string>

class DeviceManageTask:public DeviceIndependentTask
{
public:
	DeviceManageTask(void);
	DeviceManageTask(std::string strXML);
	virtual~DeviceManageTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	std::string CreateRetXml();
};