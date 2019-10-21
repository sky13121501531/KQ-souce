#pragma once
#ifndef CARDDEVSTATUSTASK_H_H
#define CARDDEVSTATUSTASK_H_H

#include "DeviceIndependentTask.h"
//OSD…Ë÷√ »ŒŒÒ
class CardDevStatusTask : public DeviceIndependentTask
{
public:
	CardDevStatusTask(void);
	CardDevStatusTask(std::string strXML);
	virtual ~CardDevStatusTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};

#endif