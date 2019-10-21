
#include "CleanAlarmStateTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Alarm/AlarmMgr.h"
#include <vector>
#include <iostream>

using namespace std;

CleanAlarmStateTask::CleanAlarmStateTask() : DeviceIndependentTask()
{

}

CleanAlarmStateTask::CleanAlarmStateTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

CleanAlarmStateTask::~CleanAlarmStateTask()
{
}
void CleanAlarmStateTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警状态清除任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE cleanAlarmStatNode = parser.GetNodeFromPath("Msg/ClearAlarmState");
	pXMLNODELIST cleanAlarmStatList = parser.GetNodeList(cleanAlarmStatNode);
	int targetCount = parser.GetChildCount(cleanAlarmStatNode);
	for (int i=0; i!=targetCount; i++)
	{
		string freq;
		pXMLNODE targetNode = parser.GetNextNode(cleanAlarmStatList);
		parser.GetAttrNode(targetNode, "Freq", freq);
		DBMANAGER::instance()->AlarmStateClear(freq);
		ALARMMGR::instance()->ClearAlarm(freq);
	}
	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateCleanAlarmStateTask(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警状态清除任务停止 !\n",DeviceID));
}

string CleanAlarmStateTask::GetTaskName()
{
	return std::string("报警状态清除任务");
}
std::string CleanAlarmStateTask::GetObjectName()
{
	return std::string("CleanAlarmStateTask");
}

