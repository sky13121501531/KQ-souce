
#include "ClientInfoQueryTask.h"
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

ClientInfoQueryTask::ClientInfoQueryTask() : DeviceIndependentTask()
{
	
}

ClientInfoQueryTask::ClientInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ClientInfoQueryTask::~ClientInfoQueryTask()
{
}
void ClientInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频用户查询任务执行 !\n",DeviceID));
	bRun = true;
	
	SetRunning(); 

	std::vector<sVedioUserInfo> vecClient;
	if( BUSINESSLAYOUTMGR::instance()->GetUser(DVBType,vecClient) ==false )
       RetValue = RUN_FAILED;
	else
	   RetValue = RUN_SUCCESS;

    SendXML(TranslateUpXML::TranslateClientInfoQuery(this,vecClient));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频用户查询任务停止 !\n",DeviceID));
}

string ClientInfoQueryTask::GetTaskName()
{
	return std::string("视频用户查询任务");
}
std::string ClientInfoQueryTask::GetObjectName()
{
	return std::string("ClientInfoQueryTask");
}

