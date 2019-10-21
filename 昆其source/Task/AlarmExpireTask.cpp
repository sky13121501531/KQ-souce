
#include "AlarmExpireTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "./TranslateDownXML.h"
#include "../Foundation/AlarmPropManager.h"
using namespace std;

AlarmExpireTask::AlarmExpireTask() : DeviceIndependentTask()
{

}

AlarmExpireTask::AlarmExpireTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmExpireTask::~AlarmExpireTask()
{

}
void AlarmExpireTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警参数设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	RetValue = RUN_SUCCESS;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE paramSetNode=parser.GetNodeFromPath("Msg/TaskAndAlarmExpireRequest");
	string ExpireDay;
	parser.GetAttrNode(paramSetNode,"ExpireDay",ExpireDay);
	SendXML(TranslateUpXML::TranslateAlarmExpireTask(this));//发送回复xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]报警参数设置任务停止 !\n",DeviceID));
}
string AlarmExpireTask::GetTaskName()
{
	return "报警失效设置任务";
}
std::string AlarmExpireTask::GetObjectName()
{
	return std::string("AlarmExpireTask");
}
