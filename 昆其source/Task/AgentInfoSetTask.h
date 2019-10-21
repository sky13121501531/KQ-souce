


#ifndef AGENT_INFO_SET_TASK_H
#define AGENT_INFO_SET_TASK_H


#include "DeviceIndependentTask.h"

#ifdef ZONG_JU_ZHAO_BIAO

//前端属性配置任务
class AgentInfoSetTask : public DeviceIndependentTask
{
public:
	AgentInfoSetTask();
	AgentInfoSetTask(std::string strXML);
	virtual ~AgentInfoSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	bool SetVideoInfo();
	bool SetSIInfo();
	bool SetSMSInfo();
	bool SetCASInfo();

private:
	std::string m_strAgentType;
};

#endif























#endif