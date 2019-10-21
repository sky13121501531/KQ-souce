#pragma once

#include "DeviceIndependentTask.h"

class ManualTaskSchedulerQuery : public DeviceIndependentTask
{
public:
	ManualTaskSchedulerQuery();
	ManualTaskSchedulerQuery(std::string strXML);
	virtual ~ManualTaskSchedulerQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};