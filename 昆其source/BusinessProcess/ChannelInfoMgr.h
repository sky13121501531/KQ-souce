///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：ChannelInfoMgr.h
// 创建者：gaoxd
// 创建时间：2009-06-13
// 内容描述：频道信息管理
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "./TaskMonitor.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>


class ChannelInfoMgr
{
public:
	ChannelInfoMgr();
	virtual ~ChannelInfoMgr();

public:
	bool UpdateChannelInfo(eDVBType type, std::string channelInfo);			//更新频道信息：更新内存数据同时更新数据库
	bool UpdateChannelByID(eDVBType type,std::vector<std::string> channelvec);
	bool UpdateChannelInfo(eDVBType type,std::vector<sChannelInfo> info);

public:
	bool GetVideoID(enumDVBType dvbtype,const std::string freq,const std::string serviceID,std::string& VideoID);
	bool GetAudioID(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID,std::string& audioPID);
	bool GetVideoID(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID,std::string& videoPID);
	bool GetServiceType(enumDVBType dvbtype,const std::string freq,const std::string serviceID,const std::string videoPID,const std::string audioPID,std::string& servicetype);    //FRee_fanrong_20140110
	bool GetServiceType2(enumDVBType dvbtype,const std::string freq,const std::string strserviceID,std::vector<std::string>& servicetype);
	bool GetVideoStreamType(enumDVBType dvbtype,const std::string freq,const std::string serviceID,const std::string videoPID,const std::string audioPID,std::string& videostreamtype);
	bool GetFreq(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID,std::string& freq);
	bool GetProName(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID,std::string& proname);
	bool GetProName(enumDVBType dvbtype,const std::string freq,const std::string serviceID,std::string& proname);
	bool GetChannelID(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID,const std::string videoPID,const std::string audioPID,std::string chcode,std::string& channelID);
	bool GetChannelID(enumDVBType dvbtype,const std::string freq,const std::string serviceID,std::string& channelID);
	bool GetOrgNetID(enumDVBType dvbtype,const std::string freq,const std::string audiopid,const std::string videopid,const std::string serviceid,std::string& orgnetid);
	bool GetTsID(enumDVBType dvbtype,const std::string freq,const std::string audiopid,const std::string videopid,const std::string serviceid,std::string& tsid);
	bool GetChannelInfo(enumDVBType dvbtype,const std::string channelid,std::string& vpid,std::string& apid,std::string& serviceid,std::string& prgname);
	bool GetFreqByChannelCode(enumDVBType dvbtype,std::string code,std::string&freq);
	bool GetProNameByFreq(enumDVBType dvbtype,std::string freq,std::string&name);
	bool GetChannelCodeByFreq(enumDVBType dvbtype,std::string freq,std::string&channelcode);
	bool GetSymbolRateQAMByFreq(enumDVBType dvbtype,std::string freq,std::string& symbolrate,std::string& qam);
	bool GetPmtPidPcrPid(enumDVBType dvbtype,std::string freq,std::string serviceid,std::string& pmtpid,std::string& pcrpid);
	bool GetPmtidListByFreq(enumDVBType eDvbtype,  std::string freq, std::vector<string>& vecPmtidInfo );	
	bool IsHDTV(enumDVBType dvbtype,const std::string orgNetID,const std::string tsID,const std::string serviceID);
	bool GetServIdListByFreq(enumDVBType dvbtype, std::string freq,std::string servid, std::string existServIdList, std::string campronum, std::string& newServIdList);
	bool FindServIdInServIdList(std::string servid, std::string ServIdList);
	bool FindServIdInServIdList(std::string servid, std::vector<string> ServIdList);
	bool GetServIdNumfromServIdList(int& servidnum, std::string ServIdList);
#ifdef ZONG_JU_ZHAO_BIAO
	bool IsHDTV(enumDVBType eDvbType, std::string freq, std::string serviceId);		//根据freq和serviceid判断是否高清
	int HDTVType( enumDVBType eDvbType, std::string freq, std::string serviceId );
	bool IsEncrypt(enumDVBType eDvbType, std::string freq, std::string serviceId);		//根据freq和serviceid判断是否加密流
	bool IsCAFree( enumDVBType eDvbType, std::string freq, std::string serviceId );		//根据freq和serviceid判断是否加授权 true清流或已授权 false 未授权
	bool GetChanIdByFreqServId(enumDVBType dvbtype, std::string freq, std::string serviceId, std::string& channelId);	
	bool GetServIdByChanId(enumDVBType dvbtype, std::string& serviceId, std::string channelId)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(ChannelMapMutex);
		//根据类型 网络id ts流id 及serviceid获得频道标识
		vector<sChannelInfo>::iterator ptr = mChannelInfoMap[dvbtype].begin();
		for (;ptr!= mChannelInfoMap[dvbtype].end();++ptr)
		{
			if ((*ptr).ChannelID == channelId)
			{
				serviceId = (*ptr).ServiceID;
				return true;
			}
		}
		return false;
	}
	bool GetFreqByChanId(enumDVBType dvbtype, std::string& strFreq, std::string channelId)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(ChannelMapMutex);
		//根据类型 网络id ts流id 及serviceid获得频道标识
		vector<sChannelInfo>::iterator ptr = mChannelInfoMap[dvbtype].begin();
		for (;ptr!= mChannelInfoMap[dvbtype].end();++ptr)
		{
			if ((*ptr).ChannelID == channelId)
			{
				strFreq = (*ptr).Freq;
				return true;
			}
		}
		return false;
	}
	bool GetProgNameByChanId(enumDVBType dvbtype, std::string strChanId, std::string& strProgName)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(ChannelMapMutex);
		//根据类型 网络id ts流id 及serviceid获得频道标识
		vector<sChannelInfo>::iterator ptr = mChannelInfoMap[dvbtype].begin();
		for (;ptr!= mChannelInfoMap[dvbtype].end();++ptr)
		{
			if ((*ptr).ChannelID == strChanId)
			{
				strProgName = (*ptr).ProgramName;
				return true;
			}
		}
		return false;
	}
	bool GetProgramIdByChanId(enumDVBType dvbtype, std::string strChanId, std::string& strProgramId)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(ChannelMapMutex);
		//根据类型 网络id ts流id 及serviceid获得频道标识
		vector<sChannelInfo>::iterator ptr = mChannelInfoMap[dvbtype].begin();
		for (;ptr!= mChannelInfoMap[dvbtype].end();++ptr)
		{
			if ((*ptr).ChannelID == strChanId)
			{
				strProgramId = (*ptr).PmtPID;
				return true;
			}
		}
		return false;
	}
#endif
	bool GetChanCodeByChanId(enumDVBType eDvbtype, const std::string chanId, std::string& chanCode);	//通过chanId获取chanCode
public:
	bool GetChannelInfoByDvbtype(enumDVBType eDvbtype, std::vector<sChannelInfo>& vecChanInfo );	//通过edvbtype获取频道信息
	bool GetFreqInfoByDvbtype(enumDVBType eDvbtype, std::vector<std::string>& vecFreqInfo );
private:
	bool InitChannelInfo();		//初始化频道信息

private:
	bool GetChannelInfoFromReportXML(std::string reportxml);	//从上传xml获得频道信息
	bool GetChannelInfoFromDBXML(std::string dbxml);			//从数据库xml获得频道信息

	bool UpdateChannelInfoMap(const eDVBType& dvbtype,sChannelInfo& channelinfo);		//更新频道信息
	
	bool CreatDBXML(const eDVBType& dvbtype,sChannelInfo& channel,std::string& xml);	//根据数据库中的频道信息构造xml
	bool UpdateDB(eDVBType type);

private:
	ChannelInfoMap  mChannelInfoMap;
	ACE_Thread_Mutex ChannelMapMutex;

};

typedef ACE_Singleton<ChannelInfoMgr,ACE_Mutex>  CHANNELINFOMGR;
