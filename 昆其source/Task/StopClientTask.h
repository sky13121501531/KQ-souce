///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：StopClientTask.h
// 创建者：gaoxd
// 创建时间：2010-08-05
// 内容描述：停止视频用户任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class StopClientTask : public DeviceIndependentTask
{
public:
	StopClientTask();
	StopClientTask(std::string strXML);
	virtual ~StopClientTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
private:
	int UserDeviceID;
};