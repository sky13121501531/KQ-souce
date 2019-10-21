///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：Scheduler.h
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：定时任务时间处理类
///////////////////////////////////////////////////////////////////////////////////////////
#include <time.h>
#include "Scheduler.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"


Scheduler::Scheduler(void)
{
	ExpiredDateTime = time(0)-1;//该任务设置为过期，如果不用SetRunTime()初始化时间，该任务直接过期
}

Scheduler::~Scheduler(void)
{
}

void Scheduler::SetRunTime(std::string startDateTime, std::string endDateTime,std::string expiredDateTime,long cycleInterval,long waitInterval,long runDuration)
{
	StartDateTime = TimeUtil::StrToDateTime(startDateTime);
	EndDateTime = TimeUtil::StrToDateTime(endDateTime);

	if (StartDateTime >= EndDateTime)
	{
		string msg = "任务时间设置错误,设置为过期任务";
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

		ExpiredDateTime = time(0)-1;//该任务设置为过期
		return;
	}

	ExpiredDateTime = TimeUtil::StrToDateTime(expiredDateTime);

	CycleInterval = cycleInterval;
	WaitInterval = waitInterval;
	RunDuration = runDuration;

	//设置当前运行时间
	CurrentStartDateTime = StartDateTime; //第一次设置为设置开始时间减等待时间
	CurrentEndDateTime = CurrentStartDateTime + RunDuration;

	if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
	{
		CurrentEndDateTime = EndDateTime;
	}

	SetNextRunTime(true);
	return;
}
void Scheduler::SetNextRunTime(bool firstset)
{
	if (EndDateTime <= time(0) && CycleInterval > 0)//任务单次结束
	{
		do 
		{
			time_t TempRunDuration = EndDateTime - StartDateTime;
			StartDateTime = EndDateTime + CycleInterval;
			EndDateTime = EndDateTime + CycleInterval +TempRunDuration;
		} 
		while (EndDateTime <= time(0));

		CurrentStartDateTime = StartDateTime;
		CurrentEndDateTime = CurrentStartDateTime + RunDuration;

		if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
		{
			CurrentEndDateTime = EndDateTime;
		}
	}
	else
	{
		if (CurrentStartDateTime < time(0))
		{
			CurrentStartDateTime = time(0);
		}
		if (firstset == false)
		{
			CurrentStartDateTime = CurrentStartDateTime +  RunDuration  + WaitInterval;
		}

		CurrentEndDateTime = CurrentStartDateTime + RunDuration;

		if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
		{
			CurrentEndDateTime = EndDateTime;
		}
		if (CurrentStartDateTime >= EndDateTime)
		{
			CurrentStartDateTime = EndDateTime;
		}
	}
	string info = string("下次执行时间：") + TimeUtil::DateTimeToStr(CurrentStartDateTime) + string("--") + TimeUtil::DateTimeToStr(CurrentEndDateTime);
	//SYSMSGSENDER::instance()->SendMsg(info);
}
void Scheduler::SetNextStartDateTime()
{
	CurrentStartDateTime += CycleInterval;
}
void Scheduler::ModifyExpiredTime(std::string expiredDateTime)
{
	ExpiredDateTime = TimeUtil::StrToDateTime(expiredDateTime);
}
bool Scheduler::IsRunNow()
{
	return (CurrentStartDateTime <= time(0) && CurrentEndDateTime >= time(0));
}
bool Scheduler::IsExpired()
{
	return ExpiredDateTime <= time(0);
}