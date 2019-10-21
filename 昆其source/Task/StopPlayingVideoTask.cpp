
#include "StopPlayingVideoTask.h"
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
#include <list>
#include <iostream>

using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO


StopPlayingVideoTask::StopPlayingVideoTask() : DeviceIndependentTask()
{

}

StopPlayingVideoTask::StopPlayingVideoTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	if(this->GetVersion() == "3.0")
	{
		XmlParser parser(strXML.c_str());
		pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
		parser.GetAttrNode(rootNode,"CoderList",m_CoderIdList);         
	}
}

StopPlayingVideoTask::~StopPlayingVideoTask()
{
}
void StopPlayingVideoTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频播放停止任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning(); 

	std::list<int> videoDevList;
	if(this->GetVersion() == "3.0")
	{
		PROPMANAGER::instance()->SeparateStr(m_CoderIdList,videoDevList);
	}
	else
	{
		int devnum = PROPMANAGER::instance()->GetCoderDevNum();
		for(int devi = 0;devi < devnum;devi++)
		{
			videoDevList.push_back(devi);
		}
		PROPMANAGER::instance()->GetVirDevList( videoDevList );
	}
	//停止videoDevList内通道的视频任务
	std::list<int>::iterator tempIter = videoDevList.begin();
	for (; tempIter!=videoDevList.end(); tempIter++)
	{
		bool ret = TSSENDERMGR::instance()->StopAllClient( *tempIter );	
		if (!ret)
			ACE_DEBUG((LM_DEBUG, "(%T| %t) 通道[%d]停止所有连接失败 !\n", *tempIter));
	}
	
	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateStopPlayingVideo(this));	//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]视频播放停止任务停止 !\n",DeviceID));
}

string StopPlayingVideoTask::GetTaskName()
{
	return std::string("视频播放停止任务");
}
std::string StopPlayingVideoTask::GetObjectName()
{
	return std::string("StopPlayingVideoTask");
}

#endif