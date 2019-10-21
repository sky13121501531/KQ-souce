#pragma once
#ifndef CAMERAQUERY_H_H
#define CAMERAQUERY_H_H

#include "DeviceIndependentTask.h"
//OSD…Ë÷√ »ŒŒÒ
class CameraQueryTask : public DeviceIndependentTask
{
public:
	CameraQueryTask(void);
	CameraQueryTask(std::string strXML);
	virtual ~CameraQueryTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};

#endif