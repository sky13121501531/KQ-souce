
#include "NvrStatusQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <list>
#include <iostream>

using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO

NvrStatusQueryTask::NvrStatusQueryTask() : DeviceIndependentTask()
{

}

NvrStatusQueryTask::NvrStatusQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

NvrStatusQueryTask::~NvrStatusQueryTask()
{
}
void NvrStatusQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]板卡状态查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	int devnum = PROPMANAGER::instance()->GetCoderDevNum();

	std::map<int, sTaskInfo> mapTaskInfo;
	for(int devi = 0;devi < devnum;devi++)
	{
		sTaskInfo taskInfo;
		memset(&taskInfo, 0, sizeof(sTaskInfo));

		bool ret = BUSINESSLAYOUTMGR::instance()->QueryRunTaskInfo(devi, taskInfo);
		if (!ret)
		{
			taskInfo.taskname = "NoTask";	//如果没有任务，就把taskname定义为NoTask
		}

		mapTaskInfo.insert( make_pair(devi, taskInfo) );
	}

	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateNvrStatusQuery(this, mapTaskInfo));	//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]板卡状态查询任务停止 !\n",DeviceID));
}

string NvrStatusQueryTask::GetTaskName()
{
	return std::string("板卡状态查询任务");
}
std::string NvrStatusQueryTask::GetObjectName()
{
	return std::string("NvrStatusQueryTask");
}

#endif