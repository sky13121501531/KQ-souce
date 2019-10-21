#pragma once

#include "DeviceIndependentTask.h"
class ManualRecordSetTask: public DeviceIndependentTask
{
public:
	ManualRecordSetTask(void);
	ManualRecordSetTask(std::string strXML);
	virtual ~ManualRecordSetTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};
