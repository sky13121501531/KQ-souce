#pragma once

#include "DeviceIndependentTask.h"

//视频转码录像默认参数设置
class RecordParamSetTask : public DeviceIndependentTask
{
public:
	RecordParamSetTask();
	RecordParamSetTask(std::string strXML);
	virtual ~RecordParamSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};