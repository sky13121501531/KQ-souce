///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskRealTimeExecute.h
// 创建者：gaoxd
// 创建时间：2009-10-27
// 内容描述：设备无关型任务执行类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"

class  XMLTask;
class TaskRealTimeExecute: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskRealTimeExecute(void);
	~TaskRealTimeExecute(void);
public:
	virtual int open(void*);
	virtual int Stop();

	virtual int svc();

public:
	bool SetTask(XMLTask* task);
	bool CurTaskIsNull(){return curTask == NULL;};
private:
	XMLTask* curTask;	 
};
