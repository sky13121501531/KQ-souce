#pragma once

#include "DeviceIndependentTask.h"
//报警方式设置
class AlarmReportSetTask : public DeviceIndependentTask
{
public:
	AlarmReportSetTask();
	AlarmReportSetTask(std::string strXML);
	virtual ~AlarmReportSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};