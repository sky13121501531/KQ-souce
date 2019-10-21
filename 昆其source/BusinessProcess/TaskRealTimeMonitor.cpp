
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/AppLog.h"
#include "../Task/XMLTask.h"
#include "TaskRealTimeExecute.h"
#include "TaskRealTimeMonitor.h"

#include <iostream>
#include <algorithm>

using namespace std;

TaskRealTimeMonitor::TaskRealTimeMonitor(void)
{
	bFlag = false;
}

TaskRealTimeMonitor::~TaskRealTimeMonitor(void)
{
}

int TaskRealTimeMonitor::open(void*)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 设备无关型任务调度线程开始执行 !\n"));

	this->activate();
	return 0;
}

int TaskRealTimeMonitor::svc()
{
	bFlag = true;
	while (bFlag)
	{
		if (TaskRealTimeExecuteList.empty())
		{
			OSFunction::Sleep(0,50);
		}
		{
			ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
			std::list<TaskRealTimeExecute*>::iterator ptr = TaskRealTimeExecuteList.begin();
			for (;ptr!=TaskRealTimeExecuteList.end();)
			{
				TaskRealTimeExecute* pExecute = *ptr;
				if ((*ptr) != NULL && (*ptr)->CurTaskIsNull())
				{	
					pExecute->Stop();
					TaskRealTimeExecuteList.erase(ptr++);
					delete pExecute;
					pExecute = NULL;
					continue;
				}
				++ptr;
			}
		}

		OSFunction::Sleep(5);	//防止cpu占用率过高
	}

	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
	std::list<TaskRealTimeExecute*>::iterator ptr = TaskRealTimeExecuteList.begin();
	for (;ptr!=TaskRealTimeExecuteList.end();)
	{
		TaskRealTimeExecute* pExecute = *ptr;
		if ((*ptr) != NULL && (*ptr)->CurTaskIsNull())
		{	
			pExecute->Stop();
			TaskRealTimeExecuteList.erase(ptr++);
			delete pExecute;
			pExecute = NULL;
			continue;
		}
		++ptr;
	}

	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 设备无关型任务调度线程停止 !\n"));

	return 0;
}

int TaskRealTimeMonitor::Stop()
{
	bFlag = false;
	
	this->wait();
	return 0;
}

bool TaskRealTimeMonitor::AddTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	TaskRealTimeExecute* pTaskRealTimeExecute = new TaskRealTimeExecute();

	if (pTaskRealTimeExecute != NULL)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
		TaskRealTimeExecuteList.push_back(pTaskRealTimeExecute);

		string successstr = task->GetTaskName() + string("在通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]设置成功");
		//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSTINFO);
		
		pTaskRealTimeExecute->SetTask(task);
		pTaskRealTimeExecute->open(0);

		return true;
	}
	
	string successstr = task->GetTaskName() + string("在通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]设置失败");
	//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSALARM);
	return false;
}
