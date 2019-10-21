#pragma once

#include "DeviceIndependentTask.h"

class TaskSchedulerQuery : public DeviceIndependentTask
{
public:
	TaskSchedulerQuery();
	TaskSchedulerQuery(std::string strXML);
	virtual ~TaskSchedulerQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};