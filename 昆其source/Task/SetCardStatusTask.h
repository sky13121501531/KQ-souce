#pragma once
#include "DeviceIndependentTask.h"
//机箱板卡使用状态设置任务
class SetCardStatusTask : public DeviceIndependentTask
{
public:
	SetCardStatusTask(void);
	SetCardStatusTask(std::string strXML);
	virtual ~SetCardStatusTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};