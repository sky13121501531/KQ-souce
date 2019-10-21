///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateUpXML.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将原子任务执行结果的XML转化为应用系统所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXML.h"
#include "TranslateUpXMLForCTTB.h"
#include "TranslateUpXMLForDVBC.h"
#include "TranslateUpXMLForRADIO.h"
#include "TranslateUpXMLForATV.h"
#include "TranslateUpXMLForAM.h"
#include "TranslateUpXMLForDVBS.h"
#include "TranslateUpXMLForCTV.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Task//StreamRealtimeQueryTask.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
//返回实时视频XML
std::string TranslateUpXML::TranslateStreamRealtimeQuery(const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case RADIO:
	case ATV:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateStreamRealtimeQuery(task);
		}
	default:
		return "";
	}
	return "";
}
//返回轮播查询XML
std::string TranslateUpXML::TranslateStreamRoundQuery(const XMLTask* task,std::string deviceID,bool multi)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateStreamRoundQuery(task,deviceID,multi);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateStreamRoundQuery(task,deviceID,multi);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateStreamRoundQuery(task,deviceID,multi);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateStreamRound(task,deviceID);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateStreamRound(task,deviceID);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateStreamRound(task,deviceID);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateStreamRound(task,deviceID);
		}
	default:
		return "";
	}
	return "";
}
//返回频道扫描XML
std::string TranslateUpXML::TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateChannelScan(task,strRtnXML);
		}
	default:
		return "";
	}
	return "";
}
//返回频道设置xml
std::string TranslateUpXML::TranslateChannelSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateChannelSet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateChannelSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateChannelSet(task);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateChannelSet(task);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateChannelSet(task);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateChannelSet(task);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateChannelSet(task);
		}
	default:
		return "";
	}
	return "";
}

//返回视频用户查询XML
std::string TranslateUpXML::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateClientInfoQuery(task,vecClient);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateClientInfoQuery(task,vecClient);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateClientInfoQuery(task,vecClient);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateClientInfoQuery(task,vecClient);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateClientInfoQuery(task,vecClient);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateClientInfoQuery(task,vecClient);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateClientInfoQuery(task,vecClient);
		}
	default:
		return "";
	}
	return "";
}
//返回表查询XML
std::string TranslateUpXML::TranslateTableQuery(const XMLTask *task, std::string strFileName)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateTableQuery(task,strFileName);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateTableQuery(task,strFileName);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateTableQuery(task,strFileName);
		}
	default:
		return "";
	}
	return "";

}
//返回录像设置XML
std::string TranslateUpXML::TranslateRecord(const XMLTask* task, const std::vector<XMLTask*>& resultVec)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateRecord(task,resultVec);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateRecord(task,resultVec);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateRecord(task,resultVec);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateRecord(task,resultVec);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateRecord(task,resultVec);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateRecord(task,resultVec);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateRecord(task,resultVec);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateRecord( const XMLTask* task,const std::vector<sRecordSetInfo>& recSetInfoVec )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateRecord(task,recSetInfoVec);
		}
	default:
		return "";
	}
	return "";
}
//返回录像设置查询XML
std::string TranslateUpXML::TranslateRecordSetQuery(const XMLTask* task,const std::vector<string>& recSetQueryInfoVec)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateRecordSetQuery(task,recSetQueryInfoVec);
		}
	default:
		return "";
	}
	return "";
}
//返回偱切录像设置XML
std::string TranslateUpXML::TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateRoundRecord(task,resultVec);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateRoundRecord(task,resultVec);
		}
	default:
		return "";
	}
	return "";
}

//返回指标任务设置xml
std::string TranslateUpXML::TranslateQualitySet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateQualitySet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateQualitySet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateQualitySet(task);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateQualitySet(task);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateQualitySet(task);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateQualitySet(task);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateQualitySet(task);
		}
	default:
		return "";
	}
	return "";
}
//返回指标查询XML
std::string TranslateUpXML::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateQualityQuery(task,FreqQualityInfo);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateQualityQuery(task,FreqQualityInfo);
		}
	default:
		return "";
	}
	return "";
}
//返回报警参数设置XML
std::string TranslateUpXML::TranslateAlarmParamSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateAlarmParamSet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAlarmParamSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateAlarmParamSet(task);
		}
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateAlarmParamSet(task);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateAlarmParamSet(task);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateAlarmParamSet(task);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateAlarmParamSet(task);
		}
	default:
		return "";
	}
	return "";
}
//返回通道状态查询XML
std::string TranslateUpXML::TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateDeviceStatusQuery(task,VecDeviceStatus);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateDeviceStatusQuery(task,VecDeviceStatus);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateDeviceStatusQuery(task,VecDeviceStatus);
		}
	default:
		return "";
	}
	return "";
}

//返回运行图设置XML
std::string TranslateUpXML::TranslateRunPlanSet(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateRunPlanSet(task);
		}
	default:
		return "";
	}
	return "";
}

//返回自动录像音视频查询XML
std::string TranslateUpXML::TranslateAutoRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateAutoRecordStream(task,URL,startDateTime,endDateTime);
		}
	default:
		return "";
	}
	return "";
}

//返回自动录像节目文件下载XML
std::string TranslateUpXML::TranslateAutoRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime,std::vector<sRecordInfo> vecLostRecordInfo)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateAutoRecordFile(task,URL,startDateTime,endDateTime,vecLostRecordInfo);
		}
	default:
		return "";
	}
	return "";
}

//返回任务录像音视频查询XML
std::string TranslateUpXML::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateTaskRecordStream(task,URL,startDateTime,endDateTime);
		}
	default:
		return "";
	}
	return "";
}

//返回任务录像节目文件下载XML
std::string TranslateUpXML::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateTaskRecordFile(task,URL, startDateTime,endDateTime);
			break;
		}
	default:
		return "";
	}
	return "";
}
//EPG查询XML
std::string TranslateUpXML::TranslateEPGQuery(const XMLTask*task,std::string URL)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBS:
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateEPGQuery(task,URL);
			break;
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateAlarmReport( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAlarmReport(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateAlarmReport(task);
		}
	default:
		return "";
	}
	return "";
}

//wz_110107
std::string TranslateUpXML::TranslateCleanAlarmStateTask(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateCleanAlarmStateTask(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateCleanAlarmStateTask(task);
		}
	default:
		return "";
	}
	return "";
}

//wz_110107
std::string TranslateUpXML::TranslateMatrixQuery( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateMatrixQuery(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateMatrixQuery(task);
		}
	default:
		return "";
	}
	return "";
}

//wz_110107
std::string TranslateUpXML::TranslateChangeQAMQuery( const XMLTask* task )
{
 if (task == NULL)
	 return "";
 switch(task->GetDVBType())
 {
 case CTTB:
	 {
		 break;
	 }
 case DVBC:
	 {
		 return TranslateUpXMLForDVBC::TranslateChangeQAMQuery(task);
	 }
 case DVBS:
	 {
		 return TranslateUpXMLForDVBS::TranslateChangeQAMQuery(task);
	 }
 default:
	 return "";
 }
 return "";
}

//wz_110107
std::string TranslateUpXML::TranslateLoopAlarmInfo( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateLoopAlarmInfo(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateLoopAlarmInfo(task);
		}
	default:
		return "";
	}
	return "";
}

//wz_110107
std::string TranslateUpXML::TranslateRecordParamSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateRecordParamSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateRecordParamSet(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAlarmQuery(task,vecAlarmResult);
			break;
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateAlarmQuery(task,vecAlarmResult);
			break;
		}
	default:
		return "";
	}
	return "";
}
//实时指标XML
std::string TranslateUpXML::TranslateQualityRealtimeQuery(const XMLTask* task,std::string strXml)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
	case DVBC:
	case DVBS:
	case ATV:
	case RADIO:
	case AM:
	case CTV:
		{
			return TranslateUpXMLForDVBC::TranslateQualityRealtimeQuery(task,strXml);
		}
	default:
		return "";
	}
	return "";
}
//运行环境XML
std::string TranslateUpXML::TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateEnvironmentQuery(task, EnvMap, paramVec);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateEnvironmentQuery(task, EnvMap, paramVec);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateEnvironmentQuery(task, EnvMap, paramVec);
		}
	default:
		return "";
	}
	return "";
}
//MHP查询XML
std::string TranslateUpXML::TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateMHPQuery(task, ftp,userName,passWord);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateMHPQuery(task, ftp,userName,passWord);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateMHPQuery(task, ftp,userName,passWord);
		}
	default:
		return "";
	}
	return "";
}
//指标补偿设置XML
std::string TranslateUpXML::TranslateQualityCompensationSet(const XMLTask *task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateQualityCompensationSet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateQualityCompensationSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateQualityCompensationSet(task);
		}
	default:
		return "";
	}
	return "";
}
//手动录像主动上报
std::string TranslateUpXML::TranslateManualRecordReport(const XMLTask *task, const std::string& strStandXML,std::string URL )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateManualRecordReport(task,strStandXML,URL);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateManualRecordReport(task,strStandXML,URL);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateManualRecordReport(task,strStandXML,URL);
		}
	default:
		return "";
	}
	return "";
}
//数据业务分析时间设置XML
std::string TranslateUpXML::TranslateAutoAnalysisTimeSet(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateAutoAnalysisTimeSet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAutoAnalysisTimeSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateAutoAnalysisTimeSet(task);
		}
	default:
		return "";
	}
	return "";
}

//数据业务分析时间设置查询XML
std::string TranslateUpXML::TranslateAutoAnalysisTimeQuery(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAutoAnalysisTimeQuery(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateManualRecordSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			return TranslateUpXMLForCTTB::TranslateManualRecordSet(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateManualRecordSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateManualRecordSet(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateOSDSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateOSDSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateOSDSet(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateStreamRealtimeRate(const XMLTask* task)
{
	if(task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateStreamRealtimeRate(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateStreamRealtimeRate(task);
		}
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateRecordCapabilityQuery(const XMLTask* task)
{
	if(task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateRecordCapabilityQuery(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateRecordCapabilityQuery(task);
		}
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateHDDefAudioParamSet(const XMLTask* task)
{
	if(task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateHDDefAudioParamSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateHDDefAudioParamSet(task);
		}
	default:
		return "";
	}
	return "";
}
std::string TranslateUpXML::TranslateAudioParamSet(const XMLTask* task)
{
	if(task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case CTTB:
		{
			break;
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateAudioParamSet(task);
		}
	case DVBS:
		{
			return TranslateUpXMLForDVBS::TranslateAudioParamSet(task);
		}
	default:
		return "";
	}
	return "";
}

//返回频谱任务设置xml
std::string TranslateUpXML::TranslateSpectrumSet( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateSpectrumSet(task);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateSpectrumSet(task);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateSpectrumSet(task);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateSpectrumSet(task);
		}
	default:
		return "";
	}
	return "";
}
std::string TranslateUpXML::TranslateSpectrumQuery( const XMLTask* task,std::vector<eSpecInfo> vecSpecInfo)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateSpectrumQuery(task,vecSpecInfo);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateSpectrumQuery(task,vecSpecInfo);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateSpectrumQuery(task,vecSpecInfo);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateSpectrumQuery(task,vecSpecInfo);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateStopClient( const XMLTask* task )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateStopClient(task);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateStopClient(task);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateStopClient(task);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateStopClient(task);
		}
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateStopClient(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateTaskSchedulerQuery(const  XMLTask* task,std::vector<sTaskInfo> taskinfovec )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateTaskSchedulerQuery(task,taskinfovec);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateTaskSchedulerQuery(task,taskinfovec);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateTaskSchedulerQuery(task,taskinfovec);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateTaskSchedulerQuery(task,taskinfovec);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateManualTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> taskinfovec)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		{
			return TranslateUpXMLForATV::TranslateManualTaskSchedulerQuery(task,taskinfovec);
		}
	case RADIO:
		{
			return TranslateUpXMLForRADIO::TranslateManualTaskSchedulerQuery(task,taskinfovec);
		}
	case AM:
		{
			return TranslateUpXMLForAM::TranslateManualTaskSchedulerQuery(task,taskinfovec);
		}
	case CTV:
		{
			return TranslateUpXMLForCTV::TranslateManualTaskSchedulerQuery(task,taskinfovec);
		}
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateChannelCheck(const XMLTask* task, const std::vector<sChannelInfo>& vecChanInfo )
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
			return TranslateUpXMLForATV::TranslateChannelCheck(task,vecChanInfo);
	case RADIO:
			return TranslateUpXMLForRADIO::TranslateChannelCheck(task,vecChanInfo);
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateRecordCheck(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
			return TranslateUpXMLForATV::TranslateRecordCheck(task, mapTaskInfo);
	case RADIO:
			return TranslateUpXMLForRADIO::TranslateRecordCheck(task, mapTaskInfo);
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateAlarmParamCheck(const XMLTask* task, const CodeAlarmParamMap& mapAlarmParam)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		return TranslateUpXMLForATV::TranslateAlarmParamCheck(task, mapAlarmParam);
	case RADIO:
		return TranslateUpXMLForRADIO::TranslateAlarmParamCheck(task, mapAlarmParam);
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateAlarmParamQuery(const XMLTask* task, const AlarmParamVec& vecAlarmParam)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case DVBC:
		return TranslateUpXMLForDVBC::TranslateAlarmParamQuery(task, vecAlarmParam);
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		return TranslateUpXMLForATV::TranslateRunPlanCheck(task, equCode, mapRunplanParam);
	case RADIO:
		return TranslateUpXMLForRADIO::TranslateRunPlanCheck(task, equCode, mapRunplanParam);
	case DVBC:
		return TranslateUpXMLForDVBC::TranslateRunPlanCheck(task, equCode, mapRunplanParam);
	default:
		return "";
	}
	return "";
}


std::string TranslateUpXML::TranslateAlarmExpireTask(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		return TranslateUpXMLForATV::TranslateAlarmExpireTask(task);
	case RADIO:
		return TranslateUpXMLForRADIO::TranslateAlarmExpireTask(task);
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateDeviceInfoQuery(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		return TranslateUpXMLForATV::TranslateDeviceInfoQuery(task);
	default:
		return "";
	}
	return "";
}

 std::string TranslateUpXML::TranslateServerStatusSet(const XMLTask* task)
{
	if (task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case ATV:
		return TranslateUpXMLForATV::TranslateServerStatusSet(task);
	case DVBC:
		return TranslateUpXMLForDVBC::TranslateServerStatusSet(task);
	default:
		return "";
	}
	return "";
}

 std::string TranslateUpXML::TranslateTaskRecordFile(  const XMLTask* task,std::vector<sRecordFileInfo> FileInfo)
 {
	 if (task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 { 
	 case ATV:
		 {
			 return TranslateUpXMLForATV::TranslateTaskRecordFile(task,FileInfo);
			 break;
		 }
	 case RADIO:
		 {
			 return TranslateUpXMLForRADIO::TranslateTaskRecordFile(task,FileInfo);
			 break;
		 }
	 default:
		 return "";
	 }
	 return "";
 }

#ifdef ZONG_JU_ZHAO_BIAO

 std::string TranslateUpXML::TranslateRecordCapabilityQuery( const XMLTask* task, int newIndexCount, int pessCount, std::string strRecordType, const std::vector<RecChanInfo>& vecRecChanInfo )
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateRecordCapabilityQuery(task, newIndexCount, pessCount, strRecordType, vecRecChanInfo);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateStopPlayingVideo( const XMLTask* task )
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateStopPlayingVideo(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateICInfoQuery( const XMLTask* task ,std::vector<CACardInfo> cardinfo )
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateICInfoQuery(task,cardinfo);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateNvrStatusQuery(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateNvrStatusQuery(task, mapTaskInfo);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateNvrStatusSet( const XMLTask* task )
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateNvrStatusSet(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateAgentInfoSet( const XMLTask* task )
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateAgentInfoSet(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateAgentInfoQuery( const XMLTask* task,const std::vector<sAgentSetInfo> &VecAgentSetInfo)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateAgentInfoQuery(task,VecAgentSetInfo);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateRebootSet( const XMLTask* task, const std::string& strStandXML)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateRebootSet(task, strStandXML);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateRebootSetToOther( const XMLTask* task, const std::string& strStandXML)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateRebootSetToOther(task, strStandXML);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateRecordFileLost(const XMLTask* task, const std::vector<sRecordInfo>& vecRecLostTime)
 {
 	 if(task == NULL)
	 {
		 return "";
	 }
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateRecordFileLost(task, vecRecLostTime);
		 }
	 default:
		 return "";
	 }
	 return "";
}
 std::string TranslateUpXML::TranslateRecVideoTime(const XMLTask* task, const VIDEOTIMEVEC& vecRecVideoTime)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateRecVideoTime(task, vecRecVideoTime);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateChanEncryptQuery(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateChanEncryptQuery(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateEncryptResult(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateEncryptResult(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }


#endif
 
 //返回 红外遥控器 虚拟键 XML
 std::string TranslateUpXML::TranslateInfraredRemoteControlQuery(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateInfraredRemoteControlQuery(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }

 std::string TranslateUpXML::TranslateOSDFormatSetQuery(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateOSDFormatSetQuery(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 } 
 std::string TranslateUpXML::TranslateOSDFormatQuery( const XMLTask* task,const std::vector<OSDFormat> &VecOSDFormat)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateOSDFormatQuery(task,VecOSDFormat);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateGetAgentInfoQuery(const XMLTask* task,string QueryLevel)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateGetAgentInfoQuery(task,QueryLevel);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateCardDevStatusQuery(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateCardDevStatusQuery(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateSetCardStatusTask(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateSetCardStatusTask(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateEquipmentHeartBeatQuery(const XMLTask* task)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateEquipmentHeartBeatQuery(task);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
 std::string TranslateUpXML::TranslateCameraQuery(const XMLTask* task,const std::string& strStandXML)
 {
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateCameraQuery(task,strStandXML);
		 }
	 default:
		 return "";
	 }
	 return "";
 }
std::string TranslateUpXML::TranslateAutoRecordUploadTask(const XMLTask* task)
{
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case CTTB:
	 case DVBC:
	 case DVBS:
	 case ATV:
	 case RADIO:
	 case AM:
	 case CTV:
		 {
			 return TranslateUpXMLForDVBC::TranslateAutoRecordUploadTask(task);
		 }
	 default:
		 return "";
	 }
	 return "";
}
std::string TranslateUpXML::TranslateLimitedBandwidth(const XMLTask* task)
{
	 if(task == NULL)
		 return "";
	 switch(task->GetDVBType())
	 {
	 case DVBC:
		 {
			 return TranslateUpXMLForDVBC::TranslateLimitedBandwidth(task);
		 }
	 default:
		 return "";
	 }
	 return "";
}

std::string TranslateUpXML::TranslateControlCardTimeQuery(const XMLTask* task)
{
	if(task == NULL)
		return "";
	switch(task->GetDVBType())
	{
	case DVBC:
		{
			return TranslateUpXMLForDVBC::TranslateControlCardTimeQuery(task);
		}
	default:
		return "";
	}
	return "";
}

std::string TranslateUpXML::TranslateDevStatusQueryTask( const XMLTask* task,std::list<std::string> DevList)
{
    if(task == NULL)
        return "";
    switch(task->GetDVBType())
    {
    case DVBC:
        {
            return TranslateUpXMLForDVBC::TranslateDevStatusQueryTask(task,DevList);
        }
    default:
        return "";
    }
    return "";
}
