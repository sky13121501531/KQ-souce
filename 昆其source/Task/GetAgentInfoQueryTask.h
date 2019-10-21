#pragma once
#ifndef GETAGENTINFOQUERYTASK_H_H
#define GETAGENTINFOQUERYTASK_H_H

#include "DeviceIndependentTask.h"
//OSD…Ë÷√ »ŒŒÒ
class GetAgentInfoQueryTask : public DeviceIndependentTask
{
public:
	GetAgentInfoQueryTask(void);
	GetAgentInfoQueryTask(std::string strXML);
	virtual ~GetAgentInfoQueryTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	
	//
	string GetQueryLevel(){return m_strQueryLevel;};
	string m_strQueryLevel;
};

#endif