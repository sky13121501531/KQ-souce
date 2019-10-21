#include "SetCardStatusTask.h"
#include "./TranslateUpXML.h"

SetCardStatusTask::SetCardStatusTask(void) : DeviceIndependentTask()
{
}

SetCardStatusTask::~SetCardStatusTask(void)
{
}

SetCardStatusTask::SetCardStatusTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void SetCardStatusTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 机箱板卡使用状态设置 !\n"));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;
	
	string ChassisID,Positon,Status;
	XmlParser parser(strStandardXML.c_str());
	pXMLNODE pChassisNode = parser.GetNodeFromPath("Msg/SetCardStatus/Chassis");
	parser.GetAttrNode(pChassisNode,"ChassisID",ChassisID);
	pXMLNODELIST nodeList=parser.GetNodeList(pChassisNode);
	for(int i=0;i<nodeList->Size();++i)
	{
		pXMLNODE pCardInfoNode=parser.GetNextNode(nodeList);
		parser.GetAttrNode(pCardInfoNode,"Positon",Positon);
		parser.GetAttrNode(pCardInfoNode,"Status",Status);


	}
	SendXML(TranslateUpXML::TranslateSetCardStatusTask(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)机箱板卡使用状态设置 !\n"));

}
std::string SetCardStatusTask::GetObjectName()
{
	return string("SetCardStatusTask");
}
std::string SetCardStatusTask::GetTaskName()
{
	return string("机箱板卡使用状态设置");
}
