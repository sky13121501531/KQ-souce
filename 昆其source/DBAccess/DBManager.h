
#ifndef __DBMANAGER_H_
#define __DBMANAGER_H_

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DBManager.h
// 创建者：gaoxd
// 创建时间：2009-05-27
// 内容描述：数据库操作管理类，外部通过统一接口操作数据库
///////////////////////////////////////////////////////////////////////////////////////////

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "../Foundation/TypeDef.h"
#include "DBParamTypeDef.h"

class DBConnection;   /////???????

class DBManager
{
public:    
    DBManager(void);
    virtual ~DBManager(void);
private:
	 bool InitConnect(void);
	 bool DisConnect(void);
	 bool IsConnected(void);
	 bool RetryConnect(DBConnection* db);
public:
	//任务数据库操作
	bool AddXmlTask(eDVBType type,std::string xml);
	bool DeleteTask(eDVBType type,std::string xml);
	bool QueryTask(eDVBType type,std::vector<std::string>&  vecXml);
	bool QueryTask(std::vector<std::string>&  vecXml);
	//频道扫描数据库操作
	bool AddChannel(eDVBType type,std::string channelid,std::string xml);
	bool DeleteChannel(eDVBType type,std::string channelid);
	bool DeleteChannelByType(eDVBType type);
	bool QueryChannel(eDVBType type,std::string channelid,std::string& xml);
	bool QueryTypeChannel(eDVBType type,std::vector<std::string>&vecXml);
	bool QueryAllChannel(std::vector<std::string>&vecXml);
	bool QueryChannelID(eDVBType type,std::vector<std::string>&vecChannelID);
	//录像信息数据库操作
	bool AddRecord30V(sRecordInfo record);
	bool AddRecord(sRecordInfo record);
	//依据监测类型和任务ID查找文件
	bool QueryRecordByTaskid(eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	//依据监测类型和频道信息查找录像文件
	bool QueryRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	//依据监测类型和通道信息查找录像文件
	bool QueryRecordByDeviceID(eDVBType type,std::string taskid,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	//更新录像文件的结束时间
	bool UpdateFileEndTime(std::string filename,std::string endtime);
	//
	bool QueryLostRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	bool GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string &EarliestStartTime);//通过对应DeviceID  and ChanneID 获取数据库该频道最早录制时间
	//磁盘管理使用接口
	bool DeleteRecord(std::string filename);//依据文件名删除录像信息
	bool QueryRecordByDays(unsigned int days,std::vector<std::string>& vecFilename);//依据过期时间删除录像信息
	bool QueryRecordTop(unsigned int count,std::vector<std::string>& vecFilename);//查询前几条录像记录
	bool QueryRecordExpire(std::vector<std::string>& vecFilename);//查询过期记录
	bool QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename);	//获取recPath路径下count个最老的录像文件	add by jidushan 11.05.04
	//报警参数
	bool UpdateAlarmParam(sAlarmParam& alarmparam);//更新报警参数
	bool QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec);//查询报警参数
	bool QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec);
	//运行图参数
	bool QueryRunPlanParam(eDVBType dvbtype,std::vector<sRunPlanParam>& runPlanParam);
	bool QueryAllRunPlanParam(std::vector<sRunPlanParam>& runPlanParam);
	bool UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam);
	//指标测量
	bool AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality);
	bool AddQualityInfo(const std::string &strInsertSql);
	bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime);
	bool DelQualityInfo(std::string strCheckTime);
	//频谱测量
	bool AddSpectrumInfo(enumDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec,std::string checktime);
	bool QuerySpecInfo(eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecQuality,std::string startdatetime,std::string enddatetime);
	//MsgID
	bool GetMsgID(std::string &MsgID);//获得MsgID
	bool UpdateMsgID(std::string MsgID);//更新MsgID
	//指标补偿
	bool QueryCompensationValu(std::vector<sQualityCompensation>& qualityCmpVec);//查询指标补偿值
	bool UpdateCompensationValu(sQualityCompensation& qualityCmp);//更新指标补偿值
	//报警信息
	bool AddAlarmInfo(sCheckParam& alarminfo,std::string mode);
	bool UpdateAlarmInfo(eDVBType dvbtype,std::string Alarmid,string mode);
	bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode);
	bool QueryAlarmInfo(string mode,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryFreqAlarmID(eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid);
	bool QueryProgramAlarmID(eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid);
	bool DeleteAlarmInfo(std::string strAlarmTime);
	bool DeleteAlarmfromAlarmid(std::string strAlarmid);
	//更新异态录像过期时间
	bool UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid);
	//wz_101229
	//临时文件入库
	bool AddTempFile(string filename, string createtime, string expiredays); //向数据库中添加临时文件信息
	bool QueryTempFileExpire(std::vector<std::string>& vecFilename);//查询过期临时文件
	bool DeleteTempFile( std::string filename );  //从数据库中删除filename对应的临时文件信息
	bool QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename);  //按天数查询临时文件信息
	bool QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename);  //按时间查询从头开始的count个临时文件信息
	//wz_101229
	//报警查询2011-03-19
	bool AlarmInfoQuery(vector<sCheckParam> &vecAlarm);
	//报警状态清除2011-03-19
	bool AlarmStateClear(string Freq);
	//配置信息查询
    bool QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue);


	bool QuerySystemConfig(eDVBType dvbtype, sSysConfigParam &config);
	bool QueryDvbConfig(eDVBType type, vector<sDvbConfigParam> &VecConfig);
	bool QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold);
	bool SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold);

	bool QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority);
	bool QueryScheduleAlarmInfo( string taskid );

	bool QueryLastRecordDateTime(std::string strFilePath,std::string &strDateTime);

	bool TruncateTable(std::string strTableName);

#ifdef ZONG_JU_ZHAO_BIAO
	//设置监测类型的SrcCode
	bool SetGeneralSrcCode(eDVBType eType, std::string strSrcCode);

	//设置监测
	bool SetGeneralUpUr(eDVBType eType, std::string strUpUr);

	//设置报警上报的url
	bool SetAlarmUrl(eDVBType eDvbtype, std::string strAlarmUrl);

	//设置马赛克轮播上报URL   //FRee_fanrong_20140117
	bool SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strStreamRoundUrl);

	//设置录像路径
	bool SetRecordSavePath( std::string strRecordPath);
	//设置日志保存路径
	bool SetLogSavePath( std::string strLogPath);

	bool SetVideoLogSavePath( std::string strLogPath);		//FRee_fanrong_20140107
	bool SetSILogSavePath( std::string strLogPath);			//FRee_fanrong_20140107
	bool SetCASLogSavePath( std::string strLogPath);		//FRee_fanrong_20140107
	bool SetSMSLogSavePath( std::string strLogPath);		//FRee_fanrong_20140107


	//设置sms、cas、si文件保存路径
	bool SetSIFilePath( std::string strSIFilePath);
	bool SetSmsFilePath( std::string strSmsFilePath);
	bool SetCasFilePath( std::string strCasFilePath);
//	bool SetSIFilePath( std::string strSIFilePath);

	//设置数据库中的录像码率
	bool SetHDRecordBps(eDVBType eDvbtype, std::string strBps);
	bool SetSDRecordBps(eDVBType eDvbtype, std::string strBps);

	//设置录像的分辨率
	bool SetHDRecordHW(eDVBType eDvbtype, std::string strHDHeight, std::string strHDWidth);
	bool SetSDRecordHW(eDVBType eDvbtype, std::string strSDHeight, std::string strSDWidth);

	//查询某个频道录像文件的最早和最晚时间
	bool QueryRecVideoTime(eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID="");
	//
	bool SetAutoAnalysisTime( std::string AnalysisTime );
	bool GetAutoAnalysisTime( std::string& AnalysisTime );
	//
	bool AgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool GetAgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID);
	bool GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID);
	bool OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);
	bool GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);
	bool AddUploadTask(sRecordUploadTaskInfo pUpload);
	bool UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus);
	bool QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask );
#endif


private:
	DBConnection* DBTask;
	DBConnection* DBChannel;
	DBConnection* DBRecord;
	DBConnection* DBOther;
	DBConnection* DBAlarmParam;
};
typedef ACE_Singleton<DBManager,ACE_Mutex>  DBMANAGER;
#endif