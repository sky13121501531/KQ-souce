
#include "InfoCheckRequestTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>

using namespace std;

InfoCheckRequestTask::InfoCheckRequestTask() : DeviceIndependentTask()
{
	;
}

InfoCheckRequestTask::InfoCheckRequestTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser taskParser( strXML.c_str() );

	string checkType;
	pXMLNODE InfoCheckNode = taskParser.GetNodeFromPath("Msg/InfoCheckRequest");
	taskParser.GetAttrNode(InfoCheckNode, "TaskType", checkType);

	if (checkType == "001")		
		m_eInfoCheckType = CHANNEL_CHECK;
	else if (checkType == "002")
		m_eInfoCheckType = RECORD_CHECK;
	else if (checkType == "003")
		m_eInfoCheckType = ALARMPARAM_CHECK;
	else
		m_eInfoCheckType = UNKNOWN_CHECK;
}

InfoCheckRequestTask::~InfoCheckRequestTask()
{
	;
}
void InfoCheckRequestTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]配置信息查询任务执行 !\n",DeviceID));
	
	bRun = true;
	SetRunning(); 
	RetValue = RUN_SUCCESS;

	switch ( m_eInfoCheckType )
	{
	case CHANNEL_CHECK:
		ChannelCheck();
		break;
	case RECORD_CHECK:
		RecordCheck();
		break;
	case ALARMPARAM_CHECK:
		AlarmParamCheck();
		break;
	default:
		break;
	}

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]配置信息查询任务停止 !\n",DeviceID));
	return;
}


void InfoCheckRequestTask::ChannelCheck()
{
	//获取某监测类型所有频道信息
	vector<sChannelInfo> vecChanInfo;
	bool ret = CHANNELINFOMGR::instance()->GetChannelInfoByDvbtype( GetDVBType(), vecChanInfo );
// 	if (!ret)
// 		RetValue = RUN_FAILED;
// 	else
// 		RetValue = RUN_SUCCESS;
	SendXML( TranslateUpXML::TranslateChannelCheck(this, vecChanInfo) );
	return;
}
void InfoCheckRequestTask::RecordCheck()
{
	//获取做自动录像的通道
	std::list<int> recordDevList;
	PROPMANAGER::instance()->GetTaskDeviceList("AutoRecord", GetDVBType(), recordDevList);

	//对正在自动录像的通道，将任务信息放入map中
	std::map<int, sTaskInfo> mapTaskInfo;
	std::list<int>::iterator devListIter = recordDevList.begin();
	for (; devListIter!=recordDevList.end(); devListIter++)
	{
		sTaskInfo runTask;
		bool ret = BUSINESSLAYOUTMGR::instance()->QueryRunTaskInfo( *devListIter, runTask);
		if (!ret || runTask.taskname!="AutoRecord")
			continue;

		mapTaskInfo.insert( make_pair(*devListIter,runTask) );
	}
	
	SendXML( TranslateUpXML::TranslateRecordCheck(this, mapTaskInfo) );
	return;
}
void InfoCheckRequestTask::AlarmParamCheck()
{
	//查询某一监测类型所有报警参数
	AlarmParamVec vecAlarmParam;
	ALARMPARAMINFOMGR::instance()->GetAlarmParamByDvbtype(GetDVBType(), vecAlarmParam);

	CodeAlarmParamMap mapAlarmParam;
	TransAlarmVecToMap(vecAlarmParam, mapAlarmParam);

	SendXML( TranslateUpXML::TranslateAlarmParamCheck(this, mapAlarmParam) );
	return;
}

void InfoCheckRequestTask::TransAlarmVecToMap(const AlarmParamVec& vecAlarmParam, CodeAlarmParamMap& mapAlarmParam)
{
	AlarmParamVec::const_iterator paramIter = vecAlarmParam.begin();
	for (; paramIter!=vecAlarmParam.end(); paramIter++)
	{
		string tempFreq = (*paramIter).Freq;
		string tempCode = "";
		if (tempFreq == "all" || tempFreq == "All")
		{
			tempCode = "all";
		}
		else
		{
			CHANNELINFOMGR::instance()->GetChannelCodeByFreq(GetDVBType(), tempFreq, tempCode);
		}

		if ( mapAlarmParam.find(tempCode)!=mapAlarmParam.end() )
		{
			mapAlarmParam[tempCode].push_back( *paramIter );
		}
		else
		{
			std::vector<sAlarmParam> tempAlarmParamVec;
			tempAlarmParamVec.push_back( *paramIter );
			mapAlarmParam.insert( make_pair(tempCode, tempAlarmParamVec) );
		}
	}

	return;
}


string InfoCheckRequestTask::GetTaskName()
{
	return std::string("配置信息查询任务");
}
std::string InfoCheckRequestTask::GetObjectName()
{
	return std::string("InfoCheckRequestTask");
}

