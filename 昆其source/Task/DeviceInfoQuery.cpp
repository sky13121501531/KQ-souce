
#include "DeviceInfoQuery.h"
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
#include <iostream>

using namespace std;

DeviceInfoQuery::DeviceInfoQuery() : DeviceIndependentTask()
{
	
}

DeviceInfoQuery::DeviceInfoQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}

DeviceInfoQuery::~DeviceInfoQuery()
{
}
void DeviceInfoQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备信息查询任务执行 !\n",DeviceID));
	bRun = true;	
	SetRunning(); 
	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateDeviceInfoQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备信息查询任务停止 !\n",DeviceID));
}

string DeviceInfoQuery::GetTaskName()
{
	return std::string("设备信息查询任务");
}
std::string DeviceInfoQuery::GetObjectName()
{
	return std::string("DeviceInfoQuery");
}

