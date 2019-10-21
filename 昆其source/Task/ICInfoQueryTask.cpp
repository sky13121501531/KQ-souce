
#include "ICInfoQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../Foundation/ParamSetManager.h"
#include <vector>
#include <iostream>

using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO


ICInfoQueryTask::ICInfoQueryTask() : DeviceIndependentTask()
{

}

ICInfoQueryTask::ICInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ICInfoQueryTask::~ICInfoQueryTask()
{
}
void ICInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡卡号查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;
	std::vector<CACardInfo> cardinfo;
	PARAMSETMANAGER::instance()->GetCACardInfo(cardinfo);
	SendXML(TranslateUpXML::TranslateICInfoQuery(this,cardinfo));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡卡号查询任务停止 !\n",DeviceID));
}

string ICInfoQueryTask::GetTaskName()
{
	return std::string("小卡卡号查询任务");
}
std::string ICInfoQueryTask::GetObjectName()
{
	return std::string("ICInfoQueryTask");
}



#endif