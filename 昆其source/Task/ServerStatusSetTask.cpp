
#include "ServerStatusSetTask.h"
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
#include "./TranslateDownXML.h"
#include "../Foundation/AlarmPropManager.h"
#include "../DeviceMgr/DeviceManager.h"
#include "../Foundation/PropManager.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
using namespace std;

ServerStatusSetTask::ServerStatusSetTask() : DeviceIndependentTask()
{

}

ServerStatusSetTask::ServerStatusSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ServerStatusSetTask::~ServerStatusSetTask()
{

}
void ServerStatusSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备运行状态设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();

	RetValue = RUN_SUCCESS;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE SetNode=parser.GetNodeFromPath("Msg/ServerStatusSet");
	std::string Devindex,Action;

	parser.GetAttrNode(SetNode,"Devindex",Devindex);
	parser.GetAttrNode(SetNode,"Action",Action);
	pXMLNODELIST NodeList=parser.GetNodeList(SetNode);
	int count=parser.GetChildCount(SetNode);
	std::string DayOfWeek,StartTime,StartDateTime,URL;
	if(count==0)
	{
		pXMLNODE chilNode = parser.CreateNodePtr(SetNode,"SingleReportTime");
		parser.SetAttrNode("StartDateTime",TimeUtil::DateTimeToStr(time(0)),chilNode);
		parser.SaveToString(strStandardXML);
	}
	for(int i=0;i<count;i++)
	{
		pXMLNODE ChildNode=parser.GetNextNode(NodeList);

		parser.GetAttrNode(ChildNode,"DayOfWeek",DayOfWeek);//DayofWeek 和StartTime是循环任务包含的属性
		parser.GetAttrNode(ChildNode,"StartTime",StartTime);
		parser.GetAttrNode(ChildNode,"StartDateTime",StartDateTime);//StartDateTime 为单次任务包含的属性
		parser.GetAttrNode(ChildNode,"URL",URL);
	}
    if(TimeUtil::StrToDateTime(StartDateTime) - time(0) <= 0 && count>0 && StartDateTime!="")
    {
        RetValue = RUN_FAILED;
        SendXML(TranslateUpXML::TranslateServerStatusSet(this));//发送回复xml
    }
    else
    {
        std::string strTime;
        int Type=0;
        if(count==0 || StartDateTime=="")
        {
            Type = -1;
            strTime = TimeUtil::DateTimeToStr(time(0) + 5);
        }
       /* else if(DayOfWeek.length()<1)
        {
            Type = -1;
            strTime = StartDateTime;
        }*/
      /*  else 
        {
            Type = StrUtil::Str2Int(DayOfWeek);
            strTime = StartTime;
        }*/

		if(Devindex=="0")
		{
			replace(URL.begin(),URL.end(),'/','\\');
			DEVICEMANGER::instance()->SetStatus(Type,Action,strTime,URL);
		}
		else if(Action=="Restart"&& Devindex!="0")
		{
			int IndexNum = PROPMANAGER::instance()->GetMonitorDevNum();
			for(int i = 0;i<IndexNum;++i)
			{
				sDeviceInfo sDevInfo;
				int CodeNum = StrUtil::Str2Int(Devindex) - 1;
				if(CodeNum==i)
				{
					PROPMANAGER::instance()->GetDevMonitorInfo(i,sDevInfo);
					HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
					string strDevInfoCMD = "POST /device/restart  HTTP/1.1  \r\n\r\n";
					string strRetDevInfo;
					pDeviceAccess->SendTaskMsg(strDevInfoCMD,strRetDevInfo);
					if(strRetDevInfo.find("OK")!=-1)
					{
						cout<<"重启板卡："<<sDevInfo.s_deviceip<<"成功！！！"<<endl;
					}
					delete pDeviceAccess;
				}
			}
		}
        //DelTaskXml();
        //AddTaskXml();
        SendXML(TranslateUpXML::TranslateServerStatusSet(this));//发送回复xml
    }
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备运行状态设置任务停止 !\n",DeviceID));
}
string ServerStatusSetTask::GetTaskName()
{
	return "设备运行状态设置任务";
}
std::string ServerStatusSetTask::GetObjectName()
{
	return std::string("ServerStatusSetTask");
}
bool ServerStatusSetTask::DelTaskXml()
{
	return DBMANAGER::instance()->DeleteTask(UNKNOWN,strStandardXML);
}
bool ServerStatusSetTask::AddTaskXml()
{
	//return DBMANAGER::instance()->AddXmlTask(UNKNOWN,strStandardXML);
	//插入任务命令执行5次，如果全部失败，就返回false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
		if (ret)
		{
			break;
		}
	}

	return ret;
}