
#include "MHPQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/TimeUtil.h"

#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/ParamSetManager.h"
#endif


using namespace std;

MHPQueryTask::MHPQueryTask() : DeviceIndependentTask()
{

}

MHPQueryTask::MHPQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

MHPQueryTask::~MHPQueryTask()
{

}
void MHPQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]MHP查询任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);

	std::string ftp,userName,passWord;
#ifdef ZONG_JU_ZHAO_BIAO
	std::string strScanTime;
	pXMLNODE QueryNode=parser.GetNodeFromPath("Msg/MHPQuery");
	parser.GetAttrNode(QueryNode,"ScanTime",strScanTime);  

	MHPInfo info;
	PARAMSETMANAGER::instance()->GetMHPInfo(info);
	std::string Curdate=TimeUtil::GetDateFromDatetime(strScanTime);

	ftp=info.ftpurl+"/"+Curdate;
	userName=info.username;
	passWord=info.pass;
#endif

	SetRetValue(RUN_SUCCESS);
	
	SendXML(TranslateUpXML::TranslateMHPQuery(this,ftp,userName,passWord));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]MHP查询任务停止 !\n",DeviceID));
}
string MHPQueryTask::GetTaskName()
{
	return "MHP查询任务";
}
std::string MHPQueryTask::GetObjectName()
{
	return std::string("MHPQueryTask");
}
