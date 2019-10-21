
#pragma once

#include "DeviceIndependentTask.h"

class EnvironmentQueryTask : public DeviceIndependentTask
{
public:
	EnvironmentQueryTask();
	EnvironmentQueryTask(std::string strXML);
	virtual ~EnvironmentQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};
