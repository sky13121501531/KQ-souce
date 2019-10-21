///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskRealTimeMonitor.h
// 创建者：gaoxd
// 创建时间：2009-10-27
// 内容描述：设备无关型任务调度类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>
#include "ace/Task.h"

class XMLTask;
class TaskRealTimeExecute;

class TaskRealTimeMonitor: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskRealTimeMonitor(void);
	~TaskRealTimeMonitor(void);

public:
	int open(void*);

	virtual int svc();

	int Stop();

public:
	bool AddTask(XMLTask* task);
private:
	bool bFlag;
	ACE_Thread_Mutex MonitorMapMutex;

	std::list<TaskRealTimeExecute*> TaskRealTimeExecuteList;
};
