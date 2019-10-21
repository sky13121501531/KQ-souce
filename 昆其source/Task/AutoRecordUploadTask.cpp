#include "AutoRecordUploadTask.h"
#include "./TranslateUpXML.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/XmlParser.h"
#include "../Communications/CommunicationMgr.h"
#include "../Communications/TsSenderMgr.h"
#include "../Task/XMLTask.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include <algorithm> 
sRecordUploadTaskInfo m_utaskinfo;
AutoRecordUploadTask::AutoRecordUploadTask(void) : DeviceIndependentTask()
{
	FupLoad = false;
}
AutoRecordUploadTask::~AutoRecordUploadTask(void)
{
}
AutoRecordUploadTask::AutoRecordUploadTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}
void AutoRecordUploadTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传任务执行 !\n"));
	
	g_pRecordUploadProcess = new RecordUploadProcessMgr();
	bRun = true;
	SetRunning(); 

	string strUserName,strPassword,strFreq,strServiceID,strAddress,strChassisID,strPosition,strModuleID,strTranscode,strStartTime,strEndTime,strType;
	sRecordUploadTaskInfo uploadtask;
	XmlParser parser(strStandardXML.c_str());
	uploadtask.s_taskinfo = strStandardXML;
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Type",strType);  
	
	pXMLNODE TaskInfoNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo");
	parser.GetAttrNode(TaskInfoNode,"TaskID",uploadtask.s_taskid);  
	parser.GetAttrNode(TaskInfoNode,"Action",uploadtask.s_active); 
	pXMLNODE ServerInfoNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ServerInfo");
	parser.GetAttrNode(ServerInfoNode,"UserName",strUserName);
	parser.GetAttrNode(ServerInfoNode,"Password",strPassword);
	parser.GetAttrNode(ServerInfoNode,"Address",strAddress);
	pXMLNODE ChannelInfoNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ChannelInfo");
	parser.GetAttrNode(ChannelInfoNode,"Freq",strFreq);
	parser.GetAttrNode(ChannelInfoNode,"ServiceID",strServiceID);
	parser.GetAttrNode(ChannelInfoNode,"StartDateTime",strStartTime);
	parser.GetAttrNode(ChannelInfoNode,"EndDateTime",strEndTime);
	pXMLNODE TranscodeNode = parser.GetNodeFromPath("Msg/AutoRecordUploadTask/TaskInfo/ChannelInfo/Transcode");
	pXMLNODELIST pInfoList = parser.GetNodeList(TranscodeNode);
	string STRTRANSCODE;//高请频道0;1;2;3
	for(int i =0;i<pInfoList->Size();++i)
	{
		pXMLNODE pInfoNode = parser.GetNextNode(pInfoList);
		parser.GetAttrNode(pInfoNode,"ChassisID",strChassisID);
		parser.GetAttrNode(pInfoNode,"Position",strPosition);
		parser.GetAttrNode(pInfoNode,"ModuleID",strModuleID);
		parser.GetAttrNode(pInfoNode,"Transcode",strTranscode);
		STRTRANSCODE += strTranscode + ";";
	}
	string strTRANSCODE = STRTRANSCODE.substr(0,STRTRANSCODE.length()-1);
	vector<sRecordInfo> vecRecordInfo;
	vector<string> vecStr;
	vector<sRecordInfo>::iterator Record_Iter;
	string CoderDeviceID = strChassisID+ "_" + strPosition + "_" + strModuleID + "_" + strTRANSCODE;
	string strchannelId;
	eDVBType strtype;
	if(strType == "DTMBDown")
	{
		strtype = CTTB;
	}
	else if(strType == "DVBCDown")
	{
		strtype = DVBC;
	}
	else if(strType == "RADIODown")
	{
		strtype = RADIO;
	}
	else if(strType == "AMDown")
	{
		strtype = AM;
	}
	else if(strType == "DVBSDown")
	{
		strtype = DVBS;
	}
	bool pRet = false;
	if(strUserName==""|| strPassword=="" || strChassisID=="" || strPosition=="" || strModuleID=="" || strTranscode==""|| strEndTime=="")//用户名或密码 不可为空
	{
		if(strUserName==""||strPassword=="")
		{
			RetValue = USERNAME_ERROR;
		}
		else if(strEndTime=="")
		{
			RetValue = TIMEFORMAT_ERROR;//任务结束时间不可空
		}
		else
		{
			RetValue = CHANNELID_DEVICEID_NO;
		}
		SendXML(TranslateUpXML::TranslateAutoRecordUploadTask(this));
	}
	else
	{
		int Enfpos = strAddress.rfind("$");
		string strDosOrder = strAddress.substr(0,++Enfpos);//映射盘符
		string strOrder = "net use " + strDosOrder + " " + string("\"") +  strPassword +  string("\"") + " "+ "/user:" + string("\"") + strUserName + string("\"");
		int status = system(strOrder.c_str());
		if(0 == WEXITSTATUS(status))
		{
			CHANNELINFOMGR::instance()->GetChanIdByFreqServId(strtype, strFreq, strServiceID, strchannelId);
			if(strStartTime=="")
			{
				DBMANAGER::instance()->GetEarliestStartTimeRecordByChannelIDandDeviceID(strtype,CoderDeviceID,strchannelId,strStartTime);
			}
			if(DBMANAGER::instance()->QueryRecordByDeviceID(strtype,"0",CoderDeviceID,vecRecordInfo,strStartTime,strEndTime))
			{
				if(vecRecordInfo.size() > 0)
				{
					for(vector<sRecordInfo>::iterator Record_Iter = vecRecordInfo.begin();Record_Iter!=vecRecordInfo.end();Record_Iter++)
					{
						if((*Record_Iter).channelID==strchannelId)
						{
							vecStr.push_back((*Record_Iter).channelID);
							uploadtask.s_channelid = strchannelId;
						}
					}
					if(vecStr.size() > 0)
					{
						RetValue = RUN_SUCCESS;
						pRet = true;
					}
					else
					{
						RetValue = CHANLID_CHANGE;
					}
				}
				else
				{
					RetValue = NO_RECORD;
				}
			}
		}
		else
		{
			RetValue =  ADRESS_ERROR;
		}

		SendXML(TranslateUpXML::TranslateAutoRecordUploadTask(this));

		if(pRet)
		{
			string devcode = GetDstCode();
			if(devcode.size() >= 6)
			{
				uploadtask.s_devcodehead = devcode.substr(0,6);
			}
			if(g_pRecordUploadProcess != NULL)
			{
				g_pRecordUploadProcess->AddRecordUploadTask(uploadtask);//????

				Sleep(1000);
				DBMANAGER::instance()->AddUploadTask(uploadtask);//????????
			}
		}
	}
	Sleep(1000);
	SetFinised(); 
	if(g_pRecordUploadProcess!=NULL)
	{
		delete g_pRecordUploadProcess;
		g_pRecordUploadProcess = NULL;
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传任务停止 !\n"));
}
std::string AutoRecordUploadTask::GetObjectName()
{
	return string("AutoRecordUploadTask");
}
std::string AutoRecordUploadTask::GetTaskName()
{
	return string("录像上传任务");
}
