#pragma once

#include "DeviceIndependentTask.h"

class AlarmExpireTask : public DeviceIndependentTask
{
public:
	AlarmExpireTask();
	AlarmExpireTask(std::string strXML);
	virtual ~AlarmExpireTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};