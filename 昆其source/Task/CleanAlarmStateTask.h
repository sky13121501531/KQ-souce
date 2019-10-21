#pragma once

#include "DeviceIndependentTask.h"
//±¨¾¯×´Ì¬Çå³ý
class CleanAlarmStateTask : public DeviceIndependentTask
{
public:
	CleanAlarmStateTask();
	CleanAlarmStateTask(std::string strXML);
	virtual ~CleanAlarmStateTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};