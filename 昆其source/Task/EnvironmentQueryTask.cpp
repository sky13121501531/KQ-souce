
#include "EnvironmentQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/StrUtil.h"
#include "../DeviceMgr/OS_Environment.h"
#include "../Foundation/TypeDef.h"
#include "../Alarm/AlarmMgr.h"
using namespace std;

EnvironmentQueryTask::EnvironmentQueryTask() : DeviceIndependentTask()
{

}

EnvironmentQueryTask::EnvironmentQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

EnvironmentQueryTask::~EnvironmentQueryTask()
{

}
void EnvironmentQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行环境指标查询任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	EnvMapInfo mapInfo;
	XmlParser parser(strStandardXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/EnvironmentQuery");
	pXMLNODELIST environmentNodeList = parser.GetNodeList(queryNode);
	int count = parser.GetChildCount(queryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE environmentNode = parser.GetNextNode(environmentNodeList);
		EnvInfo env;
		parser.GetAttrNode(environmentNode,"Type",env.Type);
		parser.GetAttrNode(environmentNode,"Desc",env.Desc);
			
		if (env.Type =="54")		//cpu使用率
		{
			long cpuUsage = OS_Environment::CPURealtimeUsage();//抛弃第一次检测值
			OSFunction::Sleep(0,100);
			cpuUsage = OS_Environment::CPURealtimeUsage();      //cpu使用率
			std::string strCPU = StrUtil::Long2Str(cpuUsage);
			mapInfo.insert(make_pair(env,strCPU));
		}
		else if (env.Type =="55")
		{
			std::string strHardDisk;
			mapInfo.insert(make_pair(env,strHardDisk));
		}
		else if (env.Type =="56")	// 内存使用率
		{
			long memUsage = OS_Environment::MemoryRealtimeUsage();   // 内存使用率
			std::string strMemory = StrUtil::Long2Str(memUsage);
			mapInfo.insert(make_pair(env,strMemory));   
		}
		else		//其他(温度、电压、湿度、状态、硬盘)
		{
			std::string strOtherVal = "";
			mapInfo.insert(make_pair(env,strOtherVal));
		}	   
	}
	std::vector<sCheckParam> paramVec;	
	SetRetValue(RUN_SUCCESS);
	SendXML(TranslateUpXML::TranslateEnvironmentQuery(this,mapInfo,paramVec));
	std::vector<sCheckParam>::iterator ptr;
	for(ptr=paramVec.begin();ptr!=paramVec.end();ptr++)
	{
			ALARMMGR::instance()->CheckAlarm(*ptr,true);
	}

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行环境指标查询任务停止 !\n",DeviceID));
}
string EnvironmentQueryTask::GetTaskName()
{
	return "运行环境指标查询";
}
std::string EnvironmentQueryTask::GetObjectName()
{
	return std::string("EnvironmentQueryTask");
}
