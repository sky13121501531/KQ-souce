///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：QualityCompensationSetTask.h
// 创建者：zhangyc
// 创建时间：2009-11-27
// 内容描述：指标补偿设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class QualityCompensationSetTask : public DeviceIndependentTask
{
public:
	QualityCompensationSetTask();
	QualityCompensationSetTask(std::string strXML);
	virtual ~QualityCompensationSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};