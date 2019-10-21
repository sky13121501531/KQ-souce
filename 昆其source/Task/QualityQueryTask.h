///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：QualityQueryTask.h
// 创建者：gaoxd
// 创建时间：2009-06-22
// 内容描述：指标结果查询
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

class QualityQueryTask : public DeviceIndependentTask
{
public:
	QualityQueryTask();
	QualityQueryTask(std::string strXML);
	virtual ~QualityQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};