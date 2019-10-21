///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RunPlanInfoMgr.h
// 创建者：gaoxd
// 创建时间：2009-09-01
// 内容描述：运行图信息管理
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>
#include <vector>
#include "../Foundation/TypeDef.h"

class RunPlanInfoMgr
{
public:
	RunPlanInfoMgr();
	virtual ~RunPlanInfoMgr();
public:
	bool InitPlanInfo(); //初始化运行图信息
	bool UpdatePlanInfo(enumDVBType dvbtype,const std::string& ChannelID,const RunPlanParamVec& OneChannelRunPlanParamVec); //更新运行图信息
	bool InRunPlan(enumDVBType dvbtype,const std::string& ChannelID);	//是否在运行图内
	bool GetRunPlanByDvbtype(enumDVBType eDvbtype, RunPlanParamVec& vecRunPlanInfo);		//根据dvbtype获取运行图信息
	bool CheckAlarmTime(enumDVBType dvbtype,const std::string& ChannelID,time_t& AlarmTime);	//检测运行图结束后，报警时间是否在运行图以内
private:
	RunPlanParamVec  mRunPlanParamVec;

	ACE_Thread_Mutex m_RunPlanMutex;
};

typedef ACE_Singleton<RunPlanInfoMgr,ACE_Mutex>  RUNPLANINFOMGR;
