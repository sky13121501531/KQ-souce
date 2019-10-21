///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：ClientInfoQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：视频用户查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class ClientInfoQueryTask : public DeviceIndependentTask
{
public:
	ClientInfoQueryTask();
	ClientInfoQueryTask(std::string strXML);
	virtual ~ClientInfoQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};