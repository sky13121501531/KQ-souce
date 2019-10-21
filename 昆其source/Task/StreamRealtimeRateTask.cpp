
#include "StreamRealtimeRateTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"

#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/ParamSetManager.h"
#endif

#include <vector>
#include <iostream>

using namespace std;

StreamRealtimeRateTask::StreamRealtimeRateTask() : DeviceIndependentTask()
{

}

StreamRealtimeRateTask::StreamRealtimeRateTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

StreamRealtimeRateTask::~StreamRealtimeRateTask()
{
}
void StreamRealtimeRateTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时视频流率任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

#ifdef ZONG_JU_ZHAO_BIAO
	XmlParser parser( strStandardXML.c_str() );
	pXMLNODE NvrSRSetNode = parser.GetNodeFromPath("Msg/NVRSteamRateSet");

	std::string setIndex,setWidth,setHeight,setFps,setBps;
	parser.GetAttrNode(NvrSRSetNode, "Index", setIndex);
	parser.GetAttrNode(NvrSRSetNode, "Width", setWidth);
	parser.GetAttrNode(NvrSRSetNode, "Height", setHeight);
	parser.GetAttrNode(NvrSRSetNode, "Fps", setFps);
	parser.GetAttrNode(NvrSRSetNode, "Bps", setBps);

	RateParam tempRateParam;
	tempRateParam.bps = setBps;
	tempRateParam.fps = setFps;
	tempRateParam.width = setWidth;
	tempRateParam.height = setHeight;

	if ( (tempRateParam.width=="352" && tempRateParam.height=="288") ||
		(tempRateParam.width=="480" && tempRateParam.height=="270")	)
	{
		tempRateParam.bps = "200";
	}
	else if ( (tempRateParam.width=="720" && tempRateParam.height=="576") ||
		(tempRateParam.width=="960" && tempRateParam.height=="540")	)
	{
		tempRateParam.bps = "1000";
	}

	PARAMSETMANAGER::instance()->SetStreamRate( tempRateParam );
#endif

	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateStreamRealtimeRate(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时视频流率任务停止 !\n",DeviceID));
}

string StreamRealtimeRateTask::GetTaskName()
{
	return std::string("实时视频流率任务");
}
std::string StreamRealtimeRateTask::GetObjectName()
{
	return std::string("StreamRealtimeRateTask");
}

