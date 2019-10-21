
#pragma once

#include "DeviceIndependentTask.h"

class DeviceInfoQuery : public DeviceIndependentTask
{
public:
	DeviceInfoQuery();
	DeviceInfoQuery(std::string strXML);
	virtual ~DeviceInfoQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};