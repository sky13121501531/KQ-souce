#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "./TaskMonitor.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>

class AlarmParamInfoMgr
{
public:
	AlarmParamInfoMgr(void);
	~AlarmParamInfoMgr(void);

	bool InitAlarmParamInfo();		//从数据库中读取报警参数设置信息，初始化到内存中
	bool UpdateAlarmParam(std::vector<sAlarmParam>& vecalaramparam);				//更新指定频点的报警参数信息
	bool GetAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam);		//根据报警的测量信息获得对应的报警参数信息
	bool UpdateAllAlarmParam(std::vector<sAlarmParam>& vecalaramparam);				//更新所有频点的报警参数信息
	bool GetAlarmParamByDvbtype(enumDVBType eDvbtype, AlarmParamVec& mapAlarmParam);		//通过eDvbtype获取某一监测类型的所有报警参数
private:
	std::vector<sAlarmParam> vecAlarmParam;
	ACE_Thread_Mutex AlarmMapMutex;
};
typedef ACE_Singleton<AlarmParamInfoMgr,ACE_Mutex>  ALARMPARAMINFOMGR;
