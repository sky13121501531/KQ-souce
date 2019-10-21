///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskExecute.h
// 创建者：gaoxd
// 创建时间：2009-05-27
// 内容描述：任务执行类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class  XMLTask;
class TaskExecute: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskExecute(void);
	~TaskExecute(void);
public:
	virtual int open(void*);
	virtual int Stop();

	virtual int svc();

public:
	bool SetTask(XMLTask* task);
	bool TaskIsStop();

	XMLTask* GetCurTask();

	bool StopTask();
	bool IsShareTask();
	std::string GetTaskFreq();
	std::string GetDeviceXml();
	std::string GetPriority();
	std::string GetMsgID();

private:
	ACE_Thread_Mutex TasMutex;
	XMLTask* curTask;	 
};
