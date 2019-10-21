
#include "ManualTaskSchedulerQuery.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <iostream>

using namespace std;

ManualTaskSchedulerQuery::ManualTaskSchedulerQuery() : DeviceIndependentTask()
{

}

ManualTaskSchedulerQuery::ManualTaskSchedulerQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ManualTaskSchedulerQuery::~ManualTaskSchedulerQuery()
{
}
void ManualTaskSchedulerQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]任务手工调度查询执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	XmlParser parser(strStandardXML.c_str());

	std::vector<sTaskInfo> taskinfovec;
	if( BUSINESSLAYOUTMGR::instance()->QueryAllTaskInfo(DVBType,taskinfovec) ==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateManualTaskSchedulerQuery(this,taskinfovec));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]任务手工调度查询停止 !\n",DeviceID));
}

string ManualTaskSchedulerQuery::GetTaskName()
{
	return std::string("任务手工调度查询");
}
std::string ManualTaskSchedulerQuery::GetObjectName()
{
	return std::string("ManualTaskSchedulerQuery");
}