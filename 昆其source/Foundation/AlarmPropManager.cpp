#include "PropConfig.h"
#include "AlarmPropManager.h"
#include "StrUtil.h"
#include "OSFunction.h"
#include "../DBAccess/DBManager.h"
CAlarmPropManager::CAlarmPropManager(void)
{
	InitiateProgData();
}

CAlarmPropManager::~CAlarmPropManager(void)
{
	if (mConfig != NULL)
	{
		delete mConfig;
		mConfig = NULL;
	}
}

bool CAlarmPropManager::InitiateProgData()
{
	if(DBMANAGER::instance()->QueryAlarmLevelThreshold(mThreShold))
		return true;
	return false;
}
bool CAlarmPropManager::GetDownThreShold(eDVBType dvbtype,string freq,string& downthreshold)
{
	bool ret=false;
	for (std::vector<ThresHold>::iterator pThresHold=mThreShold.begin();pThresHold!=mThreShold.end();pThresHold++)
	{
		if ((dvbtype==(*pThresHold).dvbtype) && (freq==(*pThresHold).freq))
		{
			downthreshold = (*pThresHold).downthreshod;
			ret=true;
			break;
		}
	}
	return ret;
}

bool CAlarmPropManager::SetAlarmThreShold(eDVBType dvbtype,string freq,string alarmthreshold)
{
	string type=OSFunction::GetStrDVBType(dvbtype);
	DBMANAGER::instance()->SetAlarmLevelThreshold(dvbtype,freq,alarmthreshold);
	std::vector<ThresHold>::iterator pThresHold=mThreShold.begin();
	for (;pThresHold!=mThreShold.end();pThresHold++)
	{
		if ((dvbtype==(*pThresHold).dvbtype) && (freq==(*pThresHold).freq))
		{
			(*pThresHold).downthreshod=alarmthreshold;
			break;
		}
	}
	if(pThresHold==mThreShold.end())
	{
		ThresHold temp;
		temp.dvbtype=dvbtype;
		temp.freq=freq;
		temp.downthreshod=alarmthreshold;
		mThreShold.push_back(temp);
	}
	return true;
}