
#include "ChangeQAMQueryTask.h"
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

ChangeQAMQueryTask::ChangeQAMQueryTask() : DeviceIndependentTask()
{

}

ChangeQAMQueryTask::ChangeQAMQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ChangeQAMQueryTask::~ChangeQAMQueryTask()
{
}
void ChangeQAMQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]QAM设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateChangeQAMQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]QAM设置任务停止 !\n",DeviceID));
}

string ChangeQAMQueryTask::GetTaskName()
{
	return std::string("QAM设置任务");
}
std::string ChangeQAMQueryTask::GetObjectName()
{
	return std::string("ChangeQAMQueryTask");
}

