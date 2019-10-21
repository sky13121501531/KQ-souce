
#include "RecordParamSetTask.h"
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
#include "../DBAccess/DBManager.h"
#endif

#include <vector>
#include <iostream>

using namespace std;

RecordParamSetTask::RecordParamSetTask() : DeviceIndependentTask()
{

}

RecordParamSetTask::RecordParamSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RecordParamSetTask::~RecordParamSetTask()
{
}
void RecordParamSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频转码录像默认参数设置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

#ifdef ZONG_JU_ZHAO_BIAO
	XmlParser parser( strStandardXML.c_str() );
	pXMLNODE RecParamSetNode = parser.GetNodeFromPath("Msg/RecordParamSet");

	std::string recParamType,recParamWidth,recParamHeight,recParamFps,recParamBps;
	int paramCount = parser.GetChildCount(RecParamSetNode);
	pXMLNODELIST RecParamList = parser.GetNodeList(RecParamSetNode);
	for (int i=0; i<paramCount; i++)
	{
		pXMLNODE recParamNode = parser.GetNextNode( RecParamList );
		parser.GetAttrNode(recParamNode, "Type", recParamType);
		parser.GetAttrNode(recParamNode, "Width", recParamWidth);
		parser.GetAttrNode(recParamNode, "Height", recParamHeight);
		parser.GetAttrNode(recParamNode, "Fps", recParamFps);
		parser.GetAttrNode(recParamNode, "Bps", recParamBps);

		RateParam tempRateParam;
		tempRateParam.bps = recParamBps;
		tempRateParam.fps = recParamFps;
		tempRateParam.width = recParamWidth;
		tempRateParam.height = recParamHeight;

		int widthparm = StrUtil::Str2Int(tempRateParam.width);
		int heightparm = StrUtil::Str2Int(tempRateParam.height); 

		if (recParamType=="HD")
		{			
			if (widthparm>=960 || heightparm>=540)
			{
				tempRateParam.width = "960";
				tempRateParam.height = "540";
				//tempRateParam.bps = "1000";
			}
			else
			{
				tempRateParam.width = "480";
				tempRateParam.height = "270";
				//tempRateParam.bps = "200";
			}
		}
		else if((recParamType=="SD"))
		{
			if (widthparm>=720 || heightparm>=576)
			{
				tempRateParam.width = "720";
				tempRateParam.height = "576";
				//tempRateParam.bps = "1000";
			}
			else
			{
				tempRateParam.width = "355";
				tempRateParam.height = "288";
				//tempRateParam.bps = "200";
			}
		}

		if (recParamType=="HD")
		{
			PARAMSETMANAGER::instance()->SetHDRecordRate(tempRateParam);
			PROPMANAGER::instance()->SetHDRecordVideoParam(GetDVBType(), tempRateParam);
			DBMANAGER::instance()->SetHDRecordBps(GetDVBType(), tempRateParam.bps);
			DBMANAGER::instance()->SetHDRecordHW(GetDVBType(), tempRateParam.height, tempRateParam.width);
		}
		else if((recParamType=="SD"))
		{	
			PARAMSETMANAGER::instance()->SetSDRecordRate(tempRateParam);
			PROPMANAGER::instance()->SetSDRecordVideoParam(GetDVBType(), tempRateParam);
			DBMANAGER::instance()->SetSDRecordBps(GetDVBType(), tempRateParam.bps);
			DBMANAGER::instance()->SetSDRecordHW(GetDVBType(), tempRateParam.height, tempRateParam.width);
		}
	}
#endif

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateRecordParamSet(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频转码录像默认参数设置任务停止 !\n",DeviceID));
}

string RecordParamSetTask::GetTaskName()
{
	return std::string("视频转码录像默认参数设置任务");
}
std::string RecordParamSetTask::GetObjectName()
{
	return std::string("RecordParamSetTask");
}

