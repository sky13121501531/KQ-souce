
#include "./GetRecVideoTimeTask.h"
#include "./TranslateUpXML.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/Xmlparser.h"
#include "../Foundation/Typedef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"




#ifdef ZONG_JU_ZHAO_BIAO

using namespace std;

GetRecVideoTimeTask::GetRecVideoTimeTask() : DeviceIndependentTask()
{

}

GetRecVideoTimeTask::GetRecVideoTimeTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

GetRecVideoTimeTask::~GetRecVideoTimeTask()
{
}

void GetRecVideoTimeTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]录像询时任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	std::vector<std::string> vecChanId;
	VIDEOTIMEVEC vecRecVideoTmInfo;
	XmlParser parser( strStandardXML.c_str() );
	pXMLNODE getTimeNode = parser.GetNodeFromPath("Msg/GetRecordVideoTime");
	int nInfoCount = parser.GetChildCount( getTimeNode );
	pXMLNODELIST infoNodeList = parser.GetNodeList( getTimeNode );
	for (int nIndex=0; nIndex<nInfoCount; nIndex ++)
	{
		pXMLNODE infoNode = parser.GetNextNode( infoNodeList );

		sRecVideoTimeInfo timeInfo;
		string strDevID;
		parser.GetAttrNode(infoNode, "ProgramID", timeInfo.strProgramId);
		parser.GetAttrNode(infoNode, "Freq", timeInfo.strFreq);
		parser.GetAttrNode(infoNode, "ServiceID", timeInfo.strServiceId);
		if(this->GetVersion() == "3.0")
		{
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				string Devlist;
				string strChassisID,strPosition,strModuleID,strChanId;
				parser.GetAttrNode(infoNode, "DeviceID", Devlist);
				parser.GetAttrNode(infoNode, "ChassisID", strChassisID);
				parser.GetAttrNode(infoNode, "Position", strPosition);
				parser.GetAttrNode(infoNode, "ModuleID", strModuleID);
				parser.GetAttrNode(infoNode, "Transcode", strChanId);
				strDevID = strChassisID+"_"+strPosition+"_"+strModuleID+"_"+strChanId;
			}
			else
			{
				string Devlist;
				parser.GetAttrNode(infoNode, "DeviceID", Devlist);
				vector<int> codlis;
				PROPMANAGER::instance()->SeparateStrVec(Devlist,codlis);
				if(codlis.size()>0)
				{
					strDevID = StrUtil::Int2Str(codlis[0]);
				}
			}
		}
		std::string strChanId;
		CHANNELINFOMGR::instance()->GetChanIdByFreqServId(GetDVBType(), timeInfo.strFreq,
			timeInfo.strServiceId, strChanId);
		
		std::string sRecordExpire = PROPMANAGER::instance()->GetRecordExpiredays(GetDVBType());
		long expiresecond = StrUtil::Str2Int(sRecordExpire)*24*3600;
		sRecordExpire = TimeUtil::DateTimeToStr(time(0)-expiresecond);
		if(this->GetVersion() == "3.0")
		{
			DBMANAGER::instance()->QueryRecVideoTime(GetDVBType(), strChanId, sRecordExpire, 
				timeInfo.strStartTime, timeInfo.strEndTime,strDevID);
		}
		else
		{
			DBMANAGER::instance()->QueryRecVideoTime(GetDVBType(), strChanId, sRecordExpire, 
				timeInfo.strStartTime, timeInfo.strEndTime);
		}
		if((timeInfo.strStartTime.size()>0)&&(timeInfo.strEndTime.size()>0))
		{
			if (timeInfo.strStartTime.at(6) == '-')
			{
				std::string strTemp = timeInfo.strStartTime.substr(5);
				timeInfo.strStartTime = timeInfo.strStartTime.substr(0,5);
				timeInfo.strStartTime += '0';
				timeInfo.strStartTime += strTemp;
			}
			if (timeInfo.strEndTime.at(6) == '-')
			{
				std::string strTemp = timeInfo.strEndTime.substr(5);
				timeInfo.strEndTime = timeInfo.strEndTime.substr(0,5);
				timeInfo.strEndTime += '0';
				timeInfo.strEndTime += strTemp;
			}
			vecRecVideoTmInfo.push_back( timeInfo );
		}
	}
	
	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateRecVideoTime(this, vecRecVideoTmInfo));//发送xml
	SetFinised(); 
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]录像询时任务停止 !\n",DeviceID));
	return;
}

string GetRecVideoTimeTask::GetTaskName()
{
	return std::string("录像询时任务");
}
std::string GetRecVideoTimeTask::GetObjectName()
{
	return std::string("GetRecVideoTimeTask");
}



#endif