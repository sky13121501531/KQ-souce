
#include "AgentInfoSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../DBAccess/DBManager.h"

#include <vector>
#include <iostream>

extern vector<sAgentSetInfo> g_VecAgentSetInfo;

#ifdef ZONG_JU_ZHAO_BIAO

using namespace std;

AgentInfoSetTask::AgentInfoSetTask() : DeviceIndependentTask()
{

}

AgentInfoSetTask::AgentInfoSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AgentInfoSetTask::~AgentInfoSetTask()
{
}

void AgentInfoSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]前端属性配置任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE AgentNode=parser.GetNodeFromPath("Msg/AgentInfoSet");
	pXMLNODELIST NodeList=parser.GetNodeList(AgentNode);
	
	std::string strDvbtype = OSFunction::GetStrDVBType( GetDVBType() );

	g_VecAgentSetInfo.clear();
	//循环获取每个设置信息
	for(int i=0;i<NodeList->Size();i++)
	{
		pXMLNODE childNode=parser.GetNextNode(NodeList);

		std::string strType, strValue, strDesc;
		parser.GetAttrNode(childNode,"AgentType",strType);
		parser.GetAttrNode(childNode,"AgentDesc",strDesc);
		parser.GetAttrNode(childNode,"AgentValue",strValue);
		
		if (strType == "2")		//获取配置类型
		{
			m_strAgentType = strValue;
		}

		sAgentSetInfo agentInfo;
		agentInfo.s_strType = strType;
		agentInfo.s_strDesc = strDesc;
		agentInfo.s_strValue = strValue;

		g_VecAgentSetInfo.push_back( agentInfo );
	}

	bool ret = true;
	if (this->GetVersion() == "3.0")
	{
		DBMANAGER::instance()->AgentSetInfo(GetDVBType(),g_VecAgentSetInfo);
	}
	else
	{
		if ("SI" == m_strAgentType)
		{
			ret = SetSIInfo();		//修改SI配置信息
		}
		else if ("VIDEO" == m_strAgentType)
		{
			ret = SetVideoInfo();	//修改VIDEO配置信息
		}
		else if ("SMS" == m_strAgentType)
		{
			ret = SetSMSInfo();		//修改SMS配置信息
		}
		else if ("CAS" == m_strAgentType)
		{
			ret = SetCASInfo();		//修改CAS配置信息
		}
	}

	RetValue = ret?RUN_SUCCESS:RUN_FAILED;

	SendXML(TranslateUpXML::TranslateAgentInfoSet(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]前端属性配置任务停止 !\n",DeviceID));
}

string AgentInfoSetTask::GetTaskName()
{
	return std::string("前端属性配置任务");
}
std::string AgentInfoSetTask::GetObjectName()
{
	return std::string("AgentInfoSetTask");
}

bool AgentInfoSetTask::SetVideoInfo()
{
	//1---2---3---4---9---10---11---12
	for (int index=0; index<g_VecAgentSetInfo.size(); index++)
	{
		std::string strType = g_VecAgentSetInfo[index].s_strType;
		std::string strValue = g_VecAgentSetInfo[index].s_strValue;

		if (strType == "3")
		{
			//设置前端code
			PROPMANAGER::instance()->SetUpCodeByType(m_strAgentType, strValue);
			PROPMANAGER::instance()->SetDefSrcCode(GetDVBType(), strValue);
			DBMANAGER::instance()->SetGeneralSrcCode(GetDVBType(),strValue);
		}
		if (strType == "4")
		{
			//中心到前端的URL   
			PROPMANAGER::instance()->SetUpUrlByType(m_strAgentType, strValue);
			//PROPMANAGER::instance()->SetUpUr(GetDVBType(), strValue);
			DBMANAGER::instance()->SetGeneralUpUr(GetDVBType(),strValue);
		}
		if(strType == "9")
		{
			DBMANAGER::instance()->SetRecordSavePath(strValue);
			PROPMANAGER::instance()->Creatfolder(strValue);				//FRee_fanrong_20140109
		}
		if(strType == "10")
		{	
			DBMANAGER::instance()->SetVideoLogSavePath(strValue);		//FRee_fanrong_20140109
			PROPMANAGER::instance()->Creatfolder(strValue);				//FRee_fanrong_20140109
		}
		if (strType == "11")	//报警url ok
		{
			PROPMANAGER::instance()->SetUpAlarmUrl(GetDVBType(), strValue);
			DBMANAGER::instance()->SetAlarmUrl(GetDVBType(), strValue);
		}
		
		if ( strType=="12")  //马赛克轮播上报url//FRee_fanrong_20140117
		{
			PROPMANAGER::instance()->SetUpStreamRoundUrl(GetDVBType(),strValue);			//FRee_fanrong_20140117
			DBMANAGER::instance()->SetUpStreamRoundUrl(GetDVBType(),strValue);				//FRee_fanrong_20140117
		}
	}

	return true;
}

bool AgentInfoSetTask::SetSIInfo()
{
	//1---2---3---4---8---10
	for (int index=0; index<g_VecAgentSetInfo.size(); index++)
	{
		std::string strType = g_VecAgentSetInfo[index].s_strType;
		std::string strValue = g_VecAgentSetInfo[index].s_strValue;

		if (strType == "3")	//前端code
		{
			//设置前端code
			PROPMANAGER::instance()->SetUpCodeByType(m_strAgentType, strValue);
			PROPMANAGER::instance()->SetDefSrcCode(GetDVBType(), strValue);
			//DBMANAGER::instance()->SetGeneralSrcCode(GetDVBType(),strValue);
		}
		if (strType == "4")
		{
			//PROPMANAGER::instance()->SetUpUrlByType(m_strAgentType, strValue);
		}
		if (strType=="8" )
		{
			DBMANAGER::instance()->SetSIFilePath(strValue);		
			PROPMANAGER::instance()->Creatfolder(strValue);		//FRee_fanrong_20140109
		}
		if(strType == "10")
		{
			DBMANAGER::instance()->SetSILogSavePath(strValue);	//FRee_fanrong_20140109
			PROPMANAGER::instance()->Creatfolder(strValue);		//FRee_fanrong_20140109
		}
	}
	return true;
}

bool AgentInfoSetTask::SetSMSInfo()
{
	//1---2---3---4---5---7---10
	for (int index=0; index<g_VecAgentSetInfo.size(); index++)
	{
		std::string strType = g_VecAgentSetInfo[index].s_strType;
		std::string strValue = g_VecAgentSetInfo[index].s_strValue;

		if (strType == "3")	//前端code
		{
			//设置前端code
			PROPMANAGER::instance()->SetUpCodeByType(m_strAgentType, strValue);
		}
		if (strType == "4")
		{
			//PROPMANAGER::instance()->SetUpUrlByType(m_strAgentType, strValue);
		}
		if (strType=="5" && strValue!="")
		{
			string strNoHead = strValue.substr(7);

			int pos1 = strNoHead.find(":");
			string strSmsIp = strNoHead.substr(0, pos1);

			int pos2 = strNoHead.find("/");
			string strSmsPort = strNoHead.substr(pos1+1, pos2-pos1-1);

			PROPMANAGER::instance()->SetSmsIp(strSmsIp);
			PROPMANAGER::instance()->SetSmsPort(strSmsPort);
			PROPMANAGER::instance()->SetSmsUrl(strValue);
		}
		if (strType=="7" )
		{
			DBMANAGER::instance()->SetSmsFilePath(strValue);
			PROPMANAGER::instance()->Creatfolder(strValue);		//FRee_fanrong_20140109
		}
		if(strType == "10")
		{
			DBMANAGER::instance()->SetSMSLogSavePath(strValue); //FRee_fanrong_20140109
			PROPMANAGER::instance()->Creatfolder(strValue);		//FRee_fanrong_20140109
		}
	}
	return true;
}

bool AgentInfoSetTask::SetCASInfo()
{
	//1---2---3---4---5---6---10
	for (int index=0; index<g_VecAgentSetInfo.size(); index++)
	{
		std::string strType = g_VecAgentSetInfo[index].s_strType;
		std::string strValue = g_VecAgentSetInfo[index].s_strValue;

		if (strType == "3")	//前端code
		{
			//设置前端code
			PROPMANAGER::instance()->SetUpCodeByType(m_strAgentType, strValue);
		}
		if (strType == "4")
		{
			//PROPMANAGER::instance()->SetUpUrlByType(m_strAgentType, strValue);
		}
		if (strType=="5" && strValue!="")
		{
			string strNoHead = strValue.substr(7);

			int pos1 = strNoHead.find(":");
			string strCasIp = strNoHead.substr(0, pos1);

			int pos2 = strNoHead.find("/");
			string strCasPort = strNoHead.substr(pos1+1, pos2-pos1-1);

			string strCasUrl = strNoHead.substr(pos2);

			PROPMANAGER::instance()->SetCasIp(strCasIp);
			PROPMANAGER::instance()->SetCasPort(strCasPort);
			PROPMANAGER::instance()->SetCasUrl(strCasUrl);
		}
		if (strType=="6")
		{
			DBMANAGER::instance()->SetCasFilePath(strValue);
			PROPMANAGER::instance()->Creatfolder(strValue);			//FRee_fanrong_20140109
		}
		if(strType == "10")
		{	
			DBMANAGER::instance()->SetCASLogSavePath(strValue);		//FRee_fanrong_20140109
			PROPMANAGER::instance()->Creatfolder(strValue);			//FRee_fanrong_20140109
		}
	}
	return true;
}
#endif