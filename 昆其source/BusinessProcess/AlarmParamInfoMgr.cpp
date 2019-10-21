#include "AlarmParamInfoMgr.h"
#include "./ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/StrUtil.h"
AlarmParamInfoMgr::AlarmParamInfoMgr(void)
{
	vecAlarmParam.clear();
	InitAlarmParamInfo();
}

AlarmParamInfoMgr::~AlarmParamInfoMgr(void)
{
}

bool AlarmParamInfoMgr::InitAlarmParamInfo( void )
{
	DBMANAGER::instance()->QueryAlarmParam(vecAlarmParam);
	return true;
}

bool AlarmParamInfoMgr::UpdateAlarmParam(std::vector<sAlarmParam>& vecalaramparam)
{
	for (std::vector<sAlarmParam>::iterator ptr=vecalaramparam.begin();ptr!=vecalaramparam.end();++ptr)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
		std::vector<sAlarmParam>::iterator ptr_in=vecAlarmParam.begin();
		for (;ptr_in!=vecAlarmParam.end();++ptr_in)
		{
			if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) &&((*ptr).AlarmType == ALARM_ENVIRONMENT))//环境报警
			{
				(*ptr_in) = (*ptr);//用新设置的参数信息更新原来的参数信息
				break;
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&((*ptr).AlarmType == ALARM_PROGRAM))//节目报警
			{
				if ((*ptr).ChannelID == (*ptr_in).ChannelID)
				{
					(*ptr_in) = (*ptr);//用新设置的参数信息更新原来的参数信息
					break;
				}
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
					((*ptr).AlarmType == ALARM_FREQ))//射频报警
			{
				if (((*ptr).Freq == (*ptr_in).Freq) && ((*ptr).SymbolRate == (*ptr_in).SymbolRate))
				{
					(*ptr_in) = (*ptr);//用新设置的参数信息更新原来的参数信息
					break;
				}
				
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
				((((*ptr).AlarmType == ALARM_TR101_290))))//102_290报警
			{
				if ((*ptr).SymbolRate == (*ptr_in).SymbolRate)
				{
					(*ptr_in) = (*ptr);//用新设置的参数信息更新原来的参数信息
					break;
				}

			}
			else
			{
				continue;
			}
		}
		if (ptr_in==vecAlarmParam.end())
		{
			vecAlarmParam.push_back(*ptr);//新设置的参数信息添加到内存
		}
		DBMANAGER::instance()->UpdateAlarmParam(*ptr);//更新报警参数信息数据库
	}
	return true;
}

bool AlarmParamInfoMgr::GetAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
	switch(checkparam.AlarmType)
	{
	case ALARM_ENVIRONMENT://环境报警
		{
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&&((*ptr).TypeID == checkparam.TypeID))
				{
					alarmparam = *ptr;
					ret = true;
					break;
				}
			}
			break;
		}
	case ALARM_PROGRAM://节目报警
		{
			int bFindFreq = 0;//0没找到参数 1找到All参数 2找到全匹配
			sAlarmParam popularAlarmParam;//通用型 节目报警参数
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&& 
					((*ptr).TypeID == checkparam.TypeID)&&((*ptr).Freq == checkparam.Freq)&&(((*ptr).TypeID == "1")||((*ptr).TypeID == "0")))//失锁报警不判断ChannelID
				{
					alarmparam = *ptr;
					ret = true;
					bFindFreq = 2;
					break;
				}
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&& 
					((*ptr).TypeID == checkparam.TypeID))
				{
					if(((*ptr).Freq == checkparam.Freq)&&((*ptr).ChannelID == checkparam.ChannelID))
					{
						alarmparam = *ptr;
						ret = true;
						bFindFreq = 2;
						break;
					}
					else if((*ptr).Freq =="All"||(*ptr).Freq =="ALL")
					{
						popularAlarmParam=*ptr;
						popularAlarmParam.ChannelID=checkparam.ChannelID;
						popularAlarmParam.Freq=checkparam.Freq;
						bFindFreq = 1;
					}
				}
			}
			if (bFindFreq == 1)
			{
				alarmparam = popularAlarmParam;
				ret = true;
			}
			break;
		}
	case ALARM_FREQ://频点报警
	case ALARM_TR101_290://290报警
		{
			int bFindFreq = 0;//0没找到参数 1找到All参数 2找到全匹配
			sAlarmParam popularAlarmParam;//通用型 节目报警参数
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&&
					((*ptr).TypeID == checkparam.TypeID))
				{
					if(((*ptr).Freq == checkparam.Freq)&&((*ptr).ChannelID == checkparam.ChannelID))
					{
						alarmparam = *ptr;
						ret = true;
						bFindFreq = 2;
						break;
					}
					else if((*ptr).Freq =="All"||(*ptr).Freq =="ALL")
					{
						popularAlarmParam=*ptr;
						popularAlarmParam.Freq=checkparam.Freq;
						bFindFreq = 1;
					}
				}
			}
			if (bFindFreq == 1)
			{
				alarmparam = popularAlarmParam;
				ret = true;
			}
			break;
		}
	default:
		break;
	}
	return ret;
}

bool AlarmParamInfoMgr::UpdateAllAlarmParam( std::vector<sAlarmParam>& vecalaramparam )
{
	for (std::vector<sAlarmParam>::iterator ptr=vecalaramparam.begin();ptr!=vecalaramparam.end();++ptr)
	{
		if (ptr->Freq!="All")
		{
			continue;
		}
		ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
		std::vector<sAlarmParam>::iterator ptr_in=vecAlarmParam.begin();
		for (;ptr_in!=vecAlarmParam.end();++ptr_in)
		{
		    if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
				(((*ptr).AlarmType == ALARM_FREQ) || (((*ptr).AlarmType == ALARM_PROGRAM))))//射频报警、节目报警
			{
				string freq=ptr_in->Freq;
				(*ptr_in) = (*ptr);//用新设置的参数信息更新原来的参数信息
				ptr_in->Freq=freq;
				DBMANAGER::instance()->UpdateAlarmParam(*ptr_in);//更新报警参数信息数据库
			}
			else
			{
				continue;
			}
		}
		
	}
	return true;
}


bool AlarmParamInfoMgr::GetAlarmParamByDvbtype(enumDVBType eDvbtype, AlarmParamVec& vecRetAlarmParam)
{
	ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);

	std::vector<sAlarmParam>::iterator paramIter = vecAlarmParam.begin();
	for (; paramIter!=vecAlarmParam.end(); paramIter++)
	{
		if ( (*paramIter).DVBType == eDvbtype)	//监测类型相同
		{
			vecRetAlarmParam.push_back( *paramIter );
		}
	}

	return true;
}