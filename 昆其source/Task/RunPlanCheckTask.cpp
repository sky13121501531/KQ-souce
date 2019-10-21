


#include "./RunPlanCheckTask.h"
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "./TranslateUpXML.h"
#include <vector>

using namespace std;



RunPlanCheckTask::RunPlanCheckTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser taskParser( strXML.c_str() );
	pXMLNODE checkNode = taskParser.GetNodeFromPath("Msg/RunPlanCheckRequest");
	taskParser.GetAttrNode(checkNode, "EquCode", m_strEquCode);
}

void RunPlanCheckTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行图信息查询任务执行 !\n",DeviceID));

	bRun = true;
	SetRunning(); 

	/**	获取运行图信息
	*/
	RunPlanParamVec vecRunplanInfo;
	if(RUNPLANINFOMGR::instance()->GetRunPlanByDvbtype( GetDVBType(), vecRunplanInfo) == false)
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;
	/** 把包含运行图信息的容器，转化为以channelCode为主键的map
	*/
	RunPlanParamMap mapCodeToParamVec;
	TransVecToMap(vecRunplanInfo, mapCodeToParamVec);

	SendXML( TranslateUpXML::TranslateRunPlanCheck(this, m_strEquCode, mapCodeToParamVec) );
	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行图信息查询任务停止 !\n",DeviceID));
	return;
}


void RunPlanCheckTask::TransVecToMap(const RunPlanParamVec& vecRunplanInfo, RunPlanParamMap& mapCodeToParamVec)
{
	RunPlanParamVec::const_iterator cRunplanIter = vecRunplanInfo.begin();
	for (; cRunplanIter!=vecRunplanInfo.end(); cRunplanIter++)
	{
		string tempChanId = (*cRunplanIter).ChannelID;
		string tempChanCode = "";
		if (tempChanId != "All" && tempChanId != "all")
		{
			CHANNELINFOMGR::instance()->GetChanCodeByChanId(GetDVBType(), tempChanId, tempChanCode);
		}
		else
			tempChanCode = tempChanId;	//如果是all的话，就返回all
		
		if ( mapCodeToParamVec.find(tempChanCode)==mapCodeToParamVec.end() )
		{
			RunPlanParamVec tempVec;
			tempVec.push_back( *cRunplanIter );
			mapCodeToParamVec.insert( make_pair(tempChanCode, tempVec) );
		}
		else
		{
			mapCodeToParamVec[tempChanCode].push_back( *cRunplanIter );
		}
	}
	return;
}


std::string RunPlanCheckTask::GetObjectName()
{
	return string("RunPlanCheckTask");
}

std::string RunPlanCheckTask::GetTaskName()
{
	return string("运行图信息查询任务");
}
