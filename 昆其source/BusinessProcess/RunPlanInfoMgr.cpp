#include "RunPlanInfoMgr.h"
#include "DownXMLProcess.h"
#include "../Communications/CommunicationMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation//XmlParser.h"
#include "../Foundation/StrUtil.h"
using namespace std;

RunPlanInfoMgr::RunPlanInfoMgr()
{
	InitPlanInfo();//初始化
}

RunPlanInfoMgr::~RunPlanInfoMgr()
{
}

bool RunPlanInfoMgr::InitPlanInfo()
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
	DBMANAGER::instance()->QueryAllRunPlanParam(mRunPlanParamVec);//数据库中查询运行图信息
	return true;
}

bool RunPlanInfoMgr::UpdatePlanInfo(enumDVBType dvbtype,const std::string& ChannelID,const RunPlanParamVec& NewRunPlanParamVec)
{
	try
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
		//删除原来dvbtype的运行图
		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();)
		{
			if (ptr->dvbtype == dvbtype /*&& ptr->ChannelID == ChannelID*/)
				ptr = mRunPlanParamVec.erase(ptr);
			else
				++ ptr;
		}
		//添加新的ChannelID的运行图
		RunPlanParamVec::const_iterator ptr_new = NewRunPlanParamVec.begin();
		for (;ptr_new!=NewRunPlanParamVec.end();++ptr_new)
		{
			mRunPlanParamVec.push_back(*ptr_new);
		}

		DBMANAGER::instance()->UpdateRunPlanParam(dvbtype,ChannelID,NewRunPlanParamVec);//更新运行图信息
	}
	catch ( ... )
	{
		string msg = "更新运行图信息失败!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
		//ACE_DEBUG((LM_ERROR,"(%T| %t)更新运行图信息失败!!\n"));
		return false;
	}
	
	return  true;
}
bool RunPlanInfoMgr::InRunPlan(enumDVBType dvbtype,const std::string& ChannelID)
{
	std::string curdatetime=TimeUtil::GetCurDateTime();
	std::string curday=TimeUtil::GetCurDay();
	std::string curmonth=TimeUtil::GetCurMonth();
	std::string dateweek=StrUtil::Long2Str(TimeUtil::DateIsWeekDay(curdatetime));
	std::string curtime=TimeUtil::GetCurTime();

	try
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
		
		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();++ptr)
		{
			//模拟模块判断，模拟中所有设置运行图均为停播或者检修，运行图中的Type无效
			if(ATV==dvbtype||CTV==dvbtype||RADIO==dvbtype||AM==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((*ptr).DayOfWeek=="ALL" || dateweek==(*ptr).DayOfWeek) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//循环
					{
						return false;
					}
					else if((*ptr).StartDateTime!="" && (*ptr).EndDateTime!="" && \
						(curdatetime>=(*ptr).StartDateTime && curdatetime<=(*ptr).EndDateTime))	//单次
					{
						return false;
					}
				}
			}
			else if(DVBC==dvbtype||DVBS==dvbtype||CTTB==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && ((*ptr).Type==0 ||(*ptr).Type==2) && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((((*ptr).Month=="ALL" || (*ptr).Month==curmonth) && curday==(*ptr).DayOfMonth)||((*ptr).DayOfWeek=="ALL" ||(*ptr).DayOfWeek=="0"|| dateweek==(*ptr).DayOfWeek)) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//循环
					{
						return false;
					}
					else if((*ptr).StartDateTime!=""&&(*ptr).EndDateTime!=""&&(curdatetime>=(*ptr).StartDateTime&&curdatetime<=(*ptr).EndDateTime))
					{
						return false;
					}
				}
			}
		}
	}
	catch (...)
	{
		string msg = "运行图判断函数异常!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
	}
	
	return true;
}



//根据dvbtype获取运行图信息
bool RunPlanInfoMgr::GetRunPlanByDvbtype(enumDVBType eDvbtype, RunPlanParamVec& vecRunPlanInfo)	
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);

	RunPlanParamVec::iterator runplanIter = mRunPlanParamVec.begin();
	for (; runplanIter!=mRunPlanParamVec.end(); runplanIter++)
	{
		if ( (*runplanIter).dvbtype == eDvbtype )
		{
			vecRunPlanInfo.push_back( *runplanIter );
		}
	}
	//没有该类型的运行图信息，认为获取失败
	if (vecRunPlanInfo.empty())
		return false;

	return true;
}


// 检测报警时间是否在运行图以内
bool RunPlanInfoMgr::CheckAlarmTime(enumDVBType dvbtype,const std::string& ChannelID,time_t& AlarmTime)
{
	try
	{
		std::string curdatetime=TimeUtil::DateTimeToStr(AlarmTime);		
		std::string curDate = TimeUtil::GetDateFromDatetime(AlarmTime);
		std::string curday=TimeUtil::GetDayFromDatetime(curdatetime);
		std::string curmonth=TimeUtil::GetMonthFromDatetime(curdatetime);
		std::string dateweek=StrUtil::Long2Str(TimeUtil::DateIsWeekDay(curdatetime));
		std::string curtime=TimeUtil::TimeToStr(AlarmTime);

		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);

		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();ptr++)
		{
			//模拟模块判断，模拟中所有设置运行图均为停播或者检修，运行图中的Type无效
			if(ATV==dvbtype||CTV==dvbtype||RADIO==dvbtype||AM==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((*ptr).DayOfWeek=="ALL" || dateweek==(*ptr).DayOfWeek) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//循环
					{
						std::string tempEndDateTime = curDate + " " + (*ptr).EndTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//将时间设置为运行图的结束时间+1
					}
					else if((*ptr).StartDateTime!="" && (*ptr).EndDateTime!="" && \
						(curdatetime>=(*ptr).StartDateTime && curdatetime<=(*ptr).EndDateTime))	//单次
					{
						std::string tempEndDateTime = (*ptr).EndDateTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//将时间设置为运行图的结束时间+1
					}
				}
			}
			else if(DVBC==dvbtype||DVBS==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && ((*ptr).Type==0 ||(*ptr).Type==2) && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((((*ptr).Month=="ALL" || (*ptr).Month==curmonth) && curday==(*ptr).DayOfMonth)||(dateweek==(*ptr).DayOfWeek)) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//循环
					{
						std::string tempEndDateTime = curDate + " " + (*ptr).EndTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//将时间设置为运行图的结束时间+1
					}
					else if((*ptr).StartDateTime!=""&&(*ptr).EndDateTime!=""&&(curdatetime>=(*ptr).StartDateTime&&curdatetime<=(*ptr).EndDateTime))
					{
						std::string tempEndDateTime = (*ptr).EndDateTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//将时间设置为运行图的结束时间+1
					}
				}
			}
		}
	}
	catch (...)
	{
		string msg = "报警时间根据运行图校正函数异常!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
	}
	
	return true;
}