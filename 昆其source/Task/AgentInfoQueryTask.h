///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：AgentInfoQueryTask.h
// 创建者：jsp
// 创建时间：2017-08-1
// 内容描述：前端属性配置查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AgentInfoQueryTask : public DeviceIndependentTask
{
public:
	AgentInfoQueryTask();
	AgentInfoQueryTask(std::string strXML);
	virtual ~AgentInfoQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};