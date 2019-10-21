///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskExecuteMgr.h
// 创建者：gaoxd
// 创建时间：2009-11-18
// 内容描述：任务执行管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <list>

class TaskExecute;
class XMLTask;
class TaskExecuteMgr: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskExecuteMgr(void);
	~TaskExecuteMgr(void);
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();
public:
	bool SetTask(XMLTask* task);		//设置一个任务
	bool StopTask();					//停止所有任务
	bool HasRunTask();					//判断是否有运行的任务
	bool TaskInExectute(XMLTask* task);	//判断该任务是否在执行队列中
	bool ShareDevice(XMLTask* task);	//判断新任务能否共享硬件
	std::string GetExecutePriority();	//得到当期通道内任务的最高优先级
	std::string GetExecuteMsgID();	//得到当期通道内任务的最高优MsgID
private:
	bool bFlag;
	ACE_Thread_Mutex TaskExecuteMutex;
	std::list<TaskExecute*> TaskExecuteList;
};
