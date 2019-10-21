///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RoundRecordSetTask.h
// 创建者：gaoxd
// 创建时间：2011-04-16
// 内容描述：偱切录像设置任务类
///////////////////////////////////////////////////////////////////////////////////////////

#include "RoundRecordSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "RecordTask.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "./RoundRecordTask.h"
using namespace std;

RoundRecordSetTask::RoundRecordSetTask() : DeviceIndependentTask()
{

}

RoundRecordSetTask::RoundRecordSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}

RoundRecordSetTask::~RoundRecordSetTask()
{

}
void RoundRecordSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置偱切录像任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();

	std::string Action;
	std::vector<std::string> TaskXMLVec;
	std::vector<XMLTask*> vecTask;
	RetValue = RUN_SUCCESS;
	TranslateDownXML::TranslateRoundRecordTask(strStandardXML,TaskXMLVec);
	for (size_t i=0; i< TaskXMLVec.size();++i)
	{
		XmlParser psr;
		int deviceid = -1;
		psr.Set_xml(TaskXMLVec[i]);
		pXMLNODE recordNode=psr.GetNodeFromPath("Msg/RoundRecord");
		psr.GetAttrNode(recordNode,"Action",Action);
		psr.GetAttrNode(recordNode,"TaskID",TaskID);
		psr.GetAttrNode(recordNode,"DeviceID",deviceid);

		if(Action=="Set"||Action=="set")
		{
			RoundRecordTask* recordtask = new RoundRecordTask(TaskXMLVec[i]);
			bool ret = BUSINESSLAYOUTMGR::instance()->AddTask(recordtask);
			if (!ret)
			{
				break;
			}
			vecTask.push_back(recordtask);
			OSFunction::Sleep(0,200);
		}
		else if(Action=="Del"||Action=="del")
		{	
			if(deviceid != 0 && !BUSINESSLAYOUTMGR::instance()->DelTask(deviceid,TaskID))
			{
				RetValue=DATABASEERROR;
			}
			else if(deviceid == 0 && !BUSINESSLAYOUTMGR::instance()->DelTask(TaskID))
			{
				RetValue=DATABASEERROR;
			}
			RetValue=RUN_SUCCESS;
		}
		
	}
	SendXML(TranslateUpXML::TranslateRoundRecord(this,vecTask));//发送到前端
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置循切录像任务停止 !\n",DeviceID));
}

std::string RoundRecordSetTask::GetObjectName()
{
	return std::string("RoundRecordSetTask");
}
string RoundRecordSetTask::GetTaskName()
{
	return "设置偱切录像任务";
}
