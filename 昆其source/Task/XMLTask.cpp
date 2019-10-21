///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：XMLTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-25
// 内容描述：任务基类
///////////////////////////////////////////////////////////////////////////////////////////
#include "Scheduler.h"
#include "XMLTask.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/BusinessIterfaceMgr.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../Alarm/AlarmMgr.h"
#include "../DBAccess/DBManager.h"

XMLTask::XMLTask()
{
	bSetOK = false;
	TaskStatus = WAITING;
	TaskScheduler = NULL;
	mIsHD = false;
	mIsOSD = false;
	RetValue = WAITFORSET;
	m_bmulti = false;
}

XMLTask::XMLTask(std::string strXML)
{
	
	bSetOK = false;
	TaskStatus = WAITING;
	TaskScheduler = NULL;

	RetValue = WAITFORSET;
	//解析XML表信息
	XmlParser parser;
	parser.Set_xml(strXML);
	pXMLNODE root=parser.GetRootNode();
	parser.GetAttrNode(root,"Version",strVersion);
	parser.GetAttrNode(root,"MsgID",strMsgID);
	parser.GetAttrNode(root,"DVBType",strType);
	parser.GetAttrNode(root,"Protocol",strProtocol);
	parser.GetAttrNode(root,"DateTime",strSetDateTime);
	parser.GetAttrNode(root,"SrcCode",strSrcCode);
	parser.GetAttrNode(root,"DstCode",strDstCode);
	parser.GetAttrNode(root,"Priority",strPriority);
	parser.GetAttrNode(root,"SrcURL",strSrcURL);

	DVBType = OSFunction::GetEnumDVBType(strType);

	if(strProtocol.length()<1)
		strProtocol="HTTP";
	
	strStandardXML = strXML;
}

XMLTask::~XMLTask()
{
}

void XMLTask::Run(void)//各个子类都需要按照下面的模式实现各自的任务以保证调度的正常工作
{
}
bool XMLTask::UpdataStatus()
{
	if(TaskScheduler->IsExpired())//过期
		SetExpired();

	switch (TaskStatus) 
	{
	case WAITING:
	{
		if(true == TaskScheduler->IsRunNow())
		{
			SetReady();

			if((DVBType==ATV||DVBType==RADIO)&&(GetObjectName()=="TaskRecord"||GetObjectName()=="AutoRecord"))
			{		
				string TypeID="";
				if(GetObjectName()=="TaskRecord")
					TypeID=TaskID;
				else if(GetObjectName()=="AutoRecord")
					TypeID=strMsgID;
				sTaskInfo runTask;              //录像任务资源冲突报警
				bool ret = BUSINESSLAYOUTMGR::instance()->QueryRecRunTaskInfo( DeviceID, runTask);	
				if(ret&&StrUtil::Str2Long(GetPriority())<StrUtil::Str2Long(runTask.priority)&&DBMANAGER::instance()->QueryScheduleAlarmInfo(TypeID))
				{
					ALARMMGR::instance()->SendAlarm(CreateSchedulerAlarm());
				}
			}
		}
		break;			
	}
	case RUNNING:
	case READY:
	{
		if(false == TaskScheduler->IsRunNow())
			SetWaiting();
		break;		
	}
	case FINISHED:
	{
		SetWaiting();
		break;		
	}
	case EXPIRED:
	default:
		break;
	}
	return true;
}

bool XMLTask::SetWaiting()
{
	if(TaskStatus == READY || TaskStatus == FINISHED||TaskStatus==RUNNING)
	{
		TaskStatus = WAITING;
		return true;
	}
	return false;
}
bool XMLTask::SetReady()
{
	if(TaskStatus == WAITING)
	{
		TaskStatus = READY;
		return true;
	}
	return false;
}
bool XMLTask::SetRunning()
{
	if(TaskStatus == READY)
	{
		TaskStatus = RUNNING;
		bSetOK = false;
		return true;
	}
	return false;
}
bool XMLTask::SetFinised()
{
	if(TaskStatus == RUNNING)
	{
		TaskStatus = FINISHED;
		return true;
	}
	return false;
}
bool XMLTask::SetExpired()
{
	Stop();
	TaskStatus = EXPIRED;
	return DelTaskXml();
}
bool XMLTask::SetTaskOK()
{
	bSetOK = true;
	return true;
}

bool XMLTask::SendXML(const std::string& strXML)
{
	bool Ret = false;

	if(strXML.empty())
	{
		std::string msg = std::string("向任务回复xml发送队列添加xml失败！") +	"MsgId：" + this->GetMsgID() + "，任务类型：" + this->GetObjectName();
		//SYSMSGSENDER::instance()->SendMsg(msg,GetDVBType(),VS_MSG_SYSALARM);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
	}
	else
	{
		BUSINESSITERFACEMGR::instance()->UplaodXML(strXML);


		string tempTaskName = this->GetTaskName();
		string tempMsgID = this->GetMsgID();

		std::string info = std::string("通道[") + StrUtil::Int2Str(DeviceID) + std::string("]发送返回XML-[") + tempTaskName + \
						   std::string(":") + tempMsgID + std::string("]:") + strXML;

		//SYSMSGSENDER::instance()->SendMsg(info,GetDVBType());

		XmlParser parser(strXML.c_str());
		string filename =  string("C:\\vscttb\\LastReport.xml");
		parser.SaveAsFile(filename.c_str());

		Ret = true;
	}

	return Ret;	
}

time_t XMLTask::GetStartDateTime()
{
	return TaskScheduler->GetStartDateTime();
}
time_t XMLTask::GetEndDateTime()
{
	return TaskScheduler->GetEndDateTime();
}
long XMLTask::GetCycleInterval()
{
	return TaskScheduler->GetCycleInterval();
}

