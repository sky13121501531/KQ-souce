///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：ChannelSetTask.cpp
// 创建者：gaoxd
// 创建时间：2009-07-27
// 内容描述：频道设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class ChannelSetTask : public DeviceIndependentTask
{
public:
	ChannelSetTask();
	ChannelSetTask(std::string strXML);
	virtual ~ChannelSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};