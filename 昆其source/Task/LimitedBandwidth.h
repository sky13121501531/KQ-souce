#pragma once
#ifndef LIMITEDBANDWIDTH_H_H
#define LIMITEDBANDWIDTH_H_H

#include "DeviceIndependentTask.h"
//OSD…Ë÷√ »ŒŒÒ
class LimitedBandwidth : public DeviceIndependentTask
{
public:
	LimitedBandwidth(void);
	LimitedBandwidth(std::string strXML);
	virtual ~LimitedBandwidth(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};

#endif