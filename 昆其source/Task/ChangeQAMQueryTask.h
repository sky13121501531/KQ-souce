#pragma once

#include "DeviceIndependentTask.h"
//QAM…Ë÷√
class ChangeQAMQueryTask : public DeviceIndependentTask
{
public:
	ChangeQAMQueryTask();
	ChangeQAMQueryTask(std::string strXML);
	virtual ~ChangeQAMQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};