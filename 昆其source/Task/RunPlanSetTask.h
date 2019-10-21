///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RunPlanSetTask.h
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：运行图设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"
#include "../Foundation/XmlParser.h"
class RunPlanSetTask : public DeviceIndependentTask
{
public:
	RunPlanSetTask();
	RunPlanSetTask(std::string strXML);
	virtual ~RunPlanSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
protected:
	bool TransRunplanSet(std::string strXML);
};