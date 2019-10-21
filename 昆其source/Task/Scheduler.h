///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：Scheduler.h
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：定时任务时间处理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "../Foundation/TypeDef.h"

/************************************************************************
描述：SetRunTime()初始化时间规则

************************************************************************/

class Scheduler
{
    //Constructors
public:
    Scheduler(void);
    ~Scheduler(void);
protected:
	 Scheduler(Scheduler &self);//不允许拷贝构造

public:
	void SetRunTime(std::string startDateTime, std::string endDateTime=TimeUtil::GetEndLessTime(),std::string expiredDateTime=TimeUtil::GetEndLessTime(),long cycleInterval=0,long waitInterval=0,long runDuration=0);
	void SetNextRunTime(bool firstset = false);

	void SetNextStartDateTime();
	void ModifyExpiredTime(std::string expiredDateTime);

	bool IsRunNow();		//判断是否应该运行
	bool IsExpired();		//判断是否应该过期

public:
	time_t GetStartDateTime(){return StartDateTime;};
	time_t GetEndDateTime(){return EndDateTime;};
	long GetCycleInterval(){return CycleInterval;};

private:
	time_t StartDateTime;		//开始时间
	time_t EndDateTime;			//结束时间

	time_t CurrentStartDateTime;//当前开始时间	
	time_t CurrentEndDateTime;	//当前结束时间
	time_t ExpiredDateTime;		//过期时间

	long CycleInterval;		//周期间隔(单次结束时间 到下次开始时间的间隔)
	long WaitInterval;		//等待间隔
	long RunDuration;		//单次运行持续时间
};
