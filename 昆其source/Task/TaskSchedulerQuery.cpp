
#include "TaskSchedulerQuery.h"
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

TaskSchedulerQuery::TaskSchedulerQuery() : DeviceIndependentTask()
{

}

TaskSchedulerQuery::TaskSchedulerQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
}

TaskSchedulerQuery::~TaskSchedulerQuery()
{
}
void TaskSchedulerQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]任务调度查询执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	XmlParser parser(strStandardXML.c_str());

	std::vector<sTaskInfo> taskinfovec;
	if( BUSINESSLAYOUTMGR::instance()->QueryAllTaskInfo(DVBType,taskinfovec) ==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateTaskSchedulerQuery(this,taskinfovec));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]任务调度查询停止 !\n",DeviceID));
}

string TaskSchedulerQuery::GetTaskName()
{
	return std::string("任务调度查询");
}
std::string TaskSchedulerQuery::GetObjectName()
{
	return std::string("TaskSchedulerQuery");
}