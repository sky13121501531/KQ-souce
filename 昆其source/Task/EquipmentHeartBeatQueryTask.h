#pragma once
#ifndef EquipmentHeartBeatQueryTask_H_H
#define EquipmentHeartBeatQueryTask_H_H

#include "DeviceIndependentTask.h"
//OSDÉèÖÃ ÈÎÎñ
class EquipmentHeartBeatQueryTask : public DeviceIndependentTask
{
public:
	EquipmentHeartBeatQueryTask(void);
	EquipmentHeartBeatQueryTask(std::string strXML);
	virtual ~EquipmentHeartBeatQueryTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};

#endif