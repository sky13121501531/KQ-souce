//2011-03-18 ±¨¾¯²éÑ¯
#pragma once

#include "DeviceIndependentTask.h"

class AlarmQueryTask : public DeviceIndependentTask
{
public:
	AlarmQueryTask();
	AlarmQueryTask(std::string strXML);
	virtual ~AlarmQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};