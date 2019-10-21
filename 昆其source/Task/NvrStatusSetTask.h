


#ifndef NVR_STATUS_SET_TASK_H
#define NVR_STATUS_SET_TASK_H


#include "DeviceIndependentTask.h"


#ifdef ZONG_JU_ZHAO_BIAO

//板卡(通道)设置任务
class NvrStatusSetTask : public DeviceIndependentTask
{
public:
	NvrStatusSetTask();
	NvrStatusSetTask(std::string strXML);
	virtual ~NvrStatusSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};


#endif





















#endif