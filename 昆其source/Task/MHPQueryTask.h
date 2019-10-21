///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：MHPQueryTask.h
// 创建者：zhangyc
// 创建时间：2009-11-27
// 内容描述：MHP查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class MHPQueryTask : public DeviceIndependentTask
{
public:
	MHPQueryTask();
	MHPQueryTask(std::string strXML);
	virtual ~MHPQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};