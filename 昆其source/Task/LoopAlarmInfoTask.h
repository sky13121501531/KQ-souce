#pragma once

#include "DeviceIndependentTask.h"

//—≠«–±®æØ…Ë÷√
class LoopAlarmInfoTask : public DeviceIndependentTask
{
public:
	LoopAlarmInfoTask();
	LoopAlarmInfoTask(std::string strXML);
	virtual ~LoopAlarmInfoTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};