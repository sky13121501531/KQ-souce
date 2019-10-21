


#ifndef REBOOT_SET_TASK_H
#define REBOOT_SET_TASK_H


#include "DeviceIndependentTask.h"


#ifdef ZONG_JU_ZHAO_BIAO

//前端重启任务
class RebootSetTask : public DeviceIndependentTask
{
public:
	RebootSetTask();
	RebootSetTask(std::string strXML);
	virtual ~RebootSetTask();
public:
	DWORD DoReboot(); 
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};


#endif






















#endif