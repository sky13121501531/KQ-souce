///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：AlarmParamQueryTask.h
// 创建者：jsp
// 创建时间：2017-07-26
// 内容描述：报警参数查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AlarmParamQueryTask : public DeviceIndependentTask
{
public:
	AlarmParamQueryTask();
	AlarmParamQueryTask(std::string strXML);
	virtual ~AlarmParamQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};