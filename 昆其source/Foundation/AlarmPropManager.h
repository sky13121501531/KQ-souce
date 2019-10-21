#pragma once
#include <string>
#include <vector>
#include "typedef.h"
#include "StrUtil.h"
#include "ace/Singleton.h"

class CAlarmPropManager
{
public:
	CAlarmPropManager(void);
	~CAlarmPropManager(void);

	bool GetDownThreShold(eDVBType dvbtype,string freq,string& downthreshold);
	bool SetAlarmThreShold(eDVBType dvbtype,string freq,string alarmthreshold);
private:
	bool InitiateProgData();
	PropConfig* mConfig;
private:
	vector<ThresHold> mThreShold;
};

typedef ACE_Singleton <CAlarmPropManager, ACE_Thread_Mutex>  ALARMPROPMANAGER;
