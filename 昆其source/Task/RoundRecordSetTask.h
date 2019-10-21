///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RoundRecordSetTask.h
// 创建者：gaoxd
// 创建时间：2011-04-16
// 内容描述：偱切录像设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RoundRecordSetTask : public DeviceIndependentTask
{
public:
	RoundRecordSetTask();
	RoundRecordSetTask(std::string strXML);
	virtual ~RoundRecordSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};