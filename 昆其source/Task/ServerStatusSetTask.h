#pragma once

#include "DeviceIndependentTask.h"

class ServerStatusSetTask : public DeviceIndependentTask
{
public:
	ServerStatusSetTask();
	ServerStatusSetTask(std::string strXML);
	virtual ~ServerStatusSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool AddTaskXml(void);
	virtual bool DelTaskXml();
};