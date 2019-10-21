#pragma once

#include "DeviceIndependentTask.h"

class StreamRealtimeRateTask : public DeviceIndependentTask
{
public:
	StreamRealtimeRateTask();
	StreamRealtimeRateTask(std::string strXML);
	virtual ~StreamRealtimeRateTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};