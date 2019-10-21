///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：AlarmParamSetTask.h
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：报警参数设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AlarmParamSetTask : public DeviceIndependentTask
{
public:
	AlarmParamSetTask();
	AlarmParamSetTask(std::string strXML);
	virtual ~AlarmParamSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};