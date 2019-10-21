
#include "QualityCompensationSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/QualityCompensationMgr.h"
using namespace std;

QualityCompensationSetTask::QualityCompensationSetTask() : DeviceIndependentTask()
{

}

QualityCompensationSetTask::QualityCompensationSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}

QualityCompensationSetTask::~QualityCompensationSetTask()
{

}
void QualityCompensationSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标补偿设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE RootNode = parser.GetRootNode();
	int count = parser.GetChildCount(RootNode);
	pXMLNODELIST IndexList = parser.GetNodeList(RootNode);
	std::vector<sQualityCompensation> qualityCmpVec;
	for(int i=0;i<count;i++)
	{
		pXMLNODE IndexNode= parser.GetNextNode(IndexList);
		sQualityCompensation tempQualityCom;
	
		parser.GetAttrNode(IndexNode,"DeviceID",tempQualityCom.deviceid);
		tempQualityCom.dvbtype = this->GetDVBType();
		int childcount =parser.GetChildCount(IndexNode);
		pXMLNODELIST childList = parser.GetNodeList(IndexNode);
		for(int k=0;k<childcount;k++)
		{
			
			pXMLNODE childNode=parser.GetNextNode(childList);
		
			parser.GetAttrNode(childNode,"Type",tempQualityCom.type);
			parser.GetAttrNode(childNode,"Value",tempQualityCom.valu);
			qualityCmpVec.push_back(tempQualityCom);
		}

	}
	if(QUALITYCOMPENSATIONMGR::instance()->UpdateQualityCompensation(qualityCmpVec))
		SetRetValue(RUN_SUCCESS);
	SendXML(TranslateUpXML::TranslateQualityCompensationSet(this));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标补偿设置任务停止 !\n",DeviceID));
}
string QualityCompensationSetTask::GetTaskName()
{
	return "指标补偿设置任务";
}
std::string QualityCompensationSetTask::GetObjectName()
{
	return std::string("QualityCompensationSetTask");
}
