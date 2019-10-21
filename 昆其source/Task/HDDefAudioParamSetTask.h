#pragma once

#include "DeviceIndependentTask.h"

class HDDefAudioParamSetTask : public DeviceIndependentTask
{
public:
	HDDefAudioParamSetTask();
	HDDefAudioParamSetTask(std::string strXML);
	virtual ~HDDefAudioParamSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};