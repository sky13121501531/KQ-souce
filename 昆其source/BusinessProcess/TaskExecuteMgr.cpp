
#include <algorithm>
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Task/XMLTask.h"
#include "TaskExecute.h"
#include "TaskExecuteMgr.h"

using namespace std;


TaskExecuteMgr::TaskExecuteMgr(void)
{
	bFlag = false;
}

TaskExecuteMgr::~TaskExecuteMgr(void)
{
}

int TaskExecuteMgr::Init()
{
	TaskExecuteList.clear();
	bFlag = true;
	return 0;
}

int TaskExecuteMgr::open(void*)
{
	this->activate();//启动任务执行线程
	return 0;
}

int TaskExecuteMgr::svc()
{
	bFlag = true;
	while (bFlag)
	{
		OSFunction::Sleep(0,100);

		ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

		std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();
		for (;ptr!=TaskExecuteList.end();++ptr)
		{
			if ((*ptr)!=NULL && (*ptr)->TaskIsStop())
			{
				(*ptr)->Stop();
				delete (*ptr);
				(*ptr) = NULL;
				TaskExecuteList.erase(ptr);
				break;
			}
		}
	}
	//退出处理
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();
	for (;ptr!=TaskExecuteList.end();)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute != NULL && pExecute->TaskIsStop())
		{	
			pExecute->Stop();
			TaskExecuteList.erase(ptr++);
			delete pExecute;
			pExecute = NULL;
			continue;
		}
		++ptr;
	}
	bFlag = false;

	return 0;
}

int TaskExecuteMgr::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}

bool TaskExecuteMgr::SetTask(XMLTask* task)
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	TaskExecute* taskexecute = new TaskExecute();
	if (taskexecute != NULL)
	{
		taskexecute->SetTask(task);
		taskexecute->open(NULL);
		TaskExecuteList.push_back(taskexecute);
	}

	//等待任务设置为running状态
	while ( (!task->IsRunning()) && (!task->IsFinised()))
	{
		Sleep(1);
	}

	return true;
}
bool TaskExecuteMgr::StopTask()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);
	bool ret = true;
	if (TaskExecuteList.empty())
	{
		return ret;
	}
	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL && !pExecute->TaskIsStop())
		{
			if(!(pExecute->StopTask()))
			{
				ret = false;
			}
		}
	}
	return ret;
}
bool TaskExecuteMgr::HasRunTask()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	if (TaskExecuteList.empty())
	{
		return false;
	}

	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL && !pExecute->TaskIsStop())
			return true;
	}
	return false;
}

bool TaskExecuteMgr::TaskInExectute(XMLTask* task)
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	if (TaskExecuteList.empty())
	{
		return false;
	}
	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL)
		{
			if (pExecute->GetCurTask() == task)
			{
				return true;
			}
		}
	}
	return false;
}

bool TaskExecuteMgr::ShareDevice(XMLTask* task)
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	if (TaskExecuteList.empty())
	{
		return true;
	}
	if (task->IsShareTask() == false)
	{
		return false;
	}
	TaskExecute* pExecute = *TaskExecuteList.begin();

	if (pExecute!=NULL && !pExecute->IsShareTask())
	{
		return false;
	}
	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL)
		{
			if (task->GetChannelID().length()<1 && task->GetTaskFreq() == pExecute->GetTaskFreq())
				return true;
			else if (task->GetDeviceXml() == pExecute->GetDeviceXml())
				 return true;
			else
				continue;
		}
	}
	return false;
}
std::string TaskExecuteMgr::GetExecutePriority()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	if (TaskExecuteList.empty())
	{
		return "";
	}

	std::string MaxPriority = "0";
	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL && StrUtil::Str2Int(pExecute->GetPriority()) > StrUtil::Str2Int(MaxPriority))
			MaxPriority = pExecute->GetPriority();
	}
	return MaxPriority;
}
std::string TaskExecuteMgr::GetExecuteMsgID()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

	if (TaskExecuteList.empty())
	{
		return "";
	}

	std::string MaxMsgID = "0";
	for (std::list<TaskExecute*>::iterator ptr = TaskExecuteList.begin();ptr!=TaskExecuteList.end();++ptr)
	{
		TaskExecute* pExecute = *ptr;
		if (pExecute!=NULL && StrUtil::Str2Int(pExecute->GetMsgID()) > StrUtil::Str2Int(MaxMsgID))
			MaxMsgID = pExecute->GetMsgID();
	}
	return MaxMsgID;
}