
#include "StopClientTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <iostream>

using namespace std;

StopClientTask::StopClientTask() : DeviceIndependentTask()
{
	
}

StopClientTask::StopClientTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	//获取要停止用户的通道号
	std::string URL,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code;
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/StopClient");
	parser.GetAttrNode(queryNode,"URL",URL);

	UserDeviceID = -1;
	if (URL.length() > 0)
	{
		char szSeps[] = "\n";
		char *cpToken = strtok((char*)URL.c_str(), szSeps);
		string dsp(cpToken);

		dsp = dsp.substr(dsp.find_last_of("/"));
		/*dsp = dsp.substr(0,dsp.find_first_of(" "));
		std::stringstream stream(dsp);
		string strTemp;
		while (getline(stream,strTemp,'/'))
		{
			if (strTemp != "")
			{
				UserDeviceID = StrUtil::Str2Int(strTemp);//得到连接的通道号
				break;
			}
		}*/
		dsp=dsp.substr(1);
		UserDeviceID = StrUtil::Str2Int(dsp);
	}
}

StopClientTask::~StopClientTask()
{
}
void StopClientTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]停止视频用户任务执行 !\n",DeviceID));
	bRun = true;
	
	SetRunning(); 

	if (BUSINESSLAYOUTMGR::instance()->StopUser(UserDeviceID) == false || TSSENDERMGR::instance()->StopAllClient(UserDeviceID) == false)
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateStopClient(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]停止视频用户任务停止 !\n",DeviceID));
}

string StopClientTask::GetTaskName()
{
	return std::string("停止视频用户任务");
}
std::string StopClientTask::GetObjectName()
{
	return std::string("StopClientTask");
}

