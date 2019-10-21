#pragma once
#include "DeviceIndependentTask.h"
class ControlCardTimeQuery : public DeviceIndependentTask
{
public:
	ControlCardTimeQuery(void);
	ControlCardTimeQuery(std::string strXML);
	virtual ~ControlCardTimeQuery(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};
