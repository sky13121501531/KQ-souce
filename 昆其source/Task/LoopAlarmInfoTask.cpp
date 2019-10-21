
#include "LoopAlarmInfoTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include <vector>
#include <iostream>

using namespace std;

LoopAlarmInfoTask::LoopAlarmInfoTask() : DeviceIndependentTask()
{

}

LoopAlarmInfoTask::LoopAlarmInfoTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

LoopAlarmInfoTask::~LoopAlarmInfoTask()
{
}
void LoopAlarmInfoTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]循切报警设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateLoopAlarmInfo(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]循切报警设置任务停止 !\n",DeviceID));
}

string LoopAlarmInfoTask::GetTaskName()
{
	return std::string("循切报警设置任务");
}
std::string LoopAlarmInfoTask::GetObjectName()
{
	return std::string("LoopAlarmInfoTask");
}

