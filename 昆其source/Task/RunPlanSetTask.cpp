#pragma warning(disable:4996)
#include "RunPlanSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "../Foundation/AppLog.h"
using namespace std;

RunPlanSetTask::RunPlanSetTask() : DeviceIndependentTask()
{

}

RunPlanSetTask::RunPlanSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RunPlanSetTask::~RunPlanSetTask()
{

}

void RunPlanSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行图设置任务执行 !\n",DeviceID));
	bRun = true;
    SetRunning();
    if ( TransRunplanSet( strStandardXML ) == false )
       RetValue = RUN_FAILED;
	else
       RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateRunPlanSet(this));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]运行图设置任务停止 !\n",DeviceID));
}

string RunPlanSetTask::GetTaskName()
{
	return "运行图设置任务";
}
std::string RunPlanSetTask::GetObjectName()
{
	return std::string("RunPlanSetTask");
}
bool RunPlanSetTask::TransRunplanSet(std::string strXML)
{
	XmlParser parser(strXML.c_str());
	string channelID,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,Freq;
	string StartTime, EndTime, ValidStartDateTime, ValidEndDateTime, DayOfWeek,Month,DayOfMonth;
	string StartDateTime, EndDateTime,AlarmEndTime;
	int RunPlanType;
	string nodeName;
	pXMLNODE rootNode = NULL ;
	pXMLNODE childNode = NULL;
	rootNode = parser.GetNodeFromPath( "Msg" );
	childNode = parser.GetNodeFirstChild( rootNode );
	nodeName  = parser.GetNodeName( childNode );//获取子元素名
	pXMLNODE runPlanSetChannelNode = NULL;
	pXMLNODE runPlanSetNode = parser.GetNodeFromPath("Msg/RunPlanSet");
	//获取RunPlanSet的子节点列表
	pXMLNODELIST runPlanSetChannelList = parser.GetNodeList( runPlanSetNode );
	//获取RunPlanSet的子节点个数
	int count = parser.GetChildCount( runPlanSetNode );
	RunPlanParamVec mrunPlanParamVec;
	for (int i = 0;i<count;i++)
	{
		
		runPlanSetChannelNode = parser.GetNextNode( runPlanSetChannelList );
		
		parser.GetAttrNode( runPlanSetChannelNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( runPlanSetChannelNode,"TsID",TsID );
		parser.GetAttrNode( runPlanSetChannelNode,"ServiceID",ServiceID );
		parser.GetAttrNode( runPlanSetChannelNode,"VideoPID",VideoPID);
		parser.GetAttrNode( runPlanSetChannelNode,"AudioPID",AudioPID);
		parser.GetAttrNode( runPlanSetChannelNode,"ChannelCode",Code );
		parser.GetAttrNode( runPlanSetChannelNode,"Freq",Freq );
		parser.GetAttrNode( runPlanSetChannelNode,"Month",Month );
		parser.GetAttrNode( runPlanSetChannelNode,"DayOfMonth",DayOfMonth );
		parser.GetAttrNode( runPlanSetChannelNode,"DayOfWeek",DayOfWeek ); 
		parser.GetAttrNode( runPlanSetChannelNode,"StartTime",StartTime );
		parser.GetAttrNode( runPlanSetChannelNode,"EndTime",EndTime );
		parser.GetAttrNode( runPlanSetChannelNode,"StartDateTime",StartDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"EndDateTime",EndDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"ValidStartDateTime",ValidStartDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"ValidEndDateTime",ValidEndDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"Type",RunPlanType);
	
		if (DVBType==DVBC || DVBType==DVBS||DVBType==CTTB)
		{
			//获取运行图参数的channelID
			bool isRight =CHANNELINFOMGR::instance()->GetChannelID( GetDVBType(),OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,channelID );
			if (isRight == false)
			{
				string msg = "运行图失败!OrgNetID:"+OrgNetID+"-TsID:"+TsID+"-ServiceID:"+ServiceID+"-VideoPID:"+VideoPID+"-AudioPID:"+AudioPID;
				APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
				return false;
			}
		}
		
		if(DayOfWeek=="7")  //DVBC星期天"7"改为"0"和模拟的统一起来
			DayOfWeek="0";
		//定义运行图参数
		sRunPlanParam mrunPlanParam;
		mrunPlanParam.dvbtype=GetDVBType();
		mrunPlanParam.DayOfWeek = DayOfWeek;
		mrunPlanParam.Month = Month;
		mrunPlanParam.DayOfMonth = DayOfMonth;
		mrunPlanParam.StartTime = StartTime;
		mrunPlanParam.EndTime = EndTime;

		if (ValidStartDateTime.length()<1)	//如果ValidStartDateTime为空，就赋值为当前时间
			mrunPlanParam.ValidStartDateTime = TimeUtil::GetCurDateTime();
		else
			mrunPlanParam.ValidStartDateTime = ValidStartDateTime;
		
		if (ValidEndDateTime.length()<1)		//如果ValidEndDateTime为空，就赋值为很长时间以后
			mrunPlanParam.ValidEndDateTime = TimeUtil::GetEndLessTime();
		else
			mrunPlanParam.ValidEndDateTime = ValidEndDateTime;
		
		mrunPlanParam.StartDateTime = StartDateTime;
		mrunPlanParam.EndDateTime = EndDateTime;
		
		if (DVBType==DVBC || DVBType==DVBS)
		{
			mrunPlanParam.ChannelID = channelID;
			mrunPlanParam.Type = RunPlanType;
		}
		else if (DVBType==ATV || DVBType==RADIO || DVBType==AM || DVBType==CTV)
		{
			string tempOrgnetId,tempTsId,tempServiceId,tempVideoPid,tempAudioPid;
			//获取channelId
			bool ret = CHANNELINFOMGR::instance()->GetChannelID(DVBType,tempOrgnetId,tempTsId,tempServiceId,
													tempVideoPid,tempAudioPid,Code,channelID);
			if (!ret)
				mrunPlanParam.ChannelID = Freq;
			else
				mrunPlanParam.ChannelID=channelID;

			mrunPlanParam.Type = 0;		//模拟只有停播
		}
		else if(DVBType==CTTB)	//地面和AM没考虑在内
		{
			mrunPlanParam.ChannelID = channelID;
			mrunPlanParam.Type = 0;
		}
		mrunPlanParamVec.push_back( mrunPlanParam );
	}
	if( RUNPLANINFOMGR::instance()->UpdatePlanInfo(GetDVBType(),channelID,mrunPlanParamVec) == false )
	{
		string msg = "运行图失败!UpdatePlanInfo";
		APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
		return false;
	}
	return true;
}