
#include <algorithm>
#include "../Foundation/OSFunction.h"
#include "../Task/XMLTask.h"
#include "TaskExecute.h"

using namespace std;


TaskExecute::TaskExecute(void)
{
	curTask = NULL;
}

TaskExecute::~TaskExecute(void)
{
}

int TaskExecute::open(void*)
{
	this->activate();//启动任务执行线程
	return 0;
}

int TaskExecute::svc()
{
	if (curTask != NULL)
	{
		curTask->Run();	//任务执行
	}
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	curTask = NULL;
	
	return 0;
}

bool TaskExecute::SetTask(XMLTask* task)
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if (curTask == NULL && task != NULL)
	{
		task->SetTaskOK();
		curTask = task;
	}
	return true;
}
int TaskExecute::Stop()
{
	this->wait();	
	return 0;
}

XMLTask* TaskExecute::GetCurTask()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	return curTask;
}

bool TaskExecute::TaskIsStop()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	return curTask == NULL;
}
bool TaskExecute::StopTask()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
	{
		return curTask->Stop();
	}
	return true;
}
bool TaskExecute::IsShareTask()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
		return curTask->IsShareTask();
	return true;
}
std::string TaskExecute::GetTaskFreq()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
		return curTask->GetTaskFreq();
	return "";
}
std::string TaskExecute::GetDeviceXml()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
		return curTask->GetDeviceXml();
	return "";
}
std::string TaskExecute::GetPriority()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
		return curTask->GetPriority();
	return "-9999";
}
std::string TaskExecute::GetMsgID()
{
	ACE_Guard<ACE_Thread_Mutex> guard(TasMutex);
	if(curTask != NULL)
		return curTask->GetMsgID();
	return "-9999";
}