///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：QualitySetTask.h
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：指标任务设置类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class QualitySetTask : public DeviceIndependentTask
{
public:
	QualitySetTask();
	QualitySetTask(std::string strXML);
	virtual ~QualitySetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};