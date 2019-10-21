#pragma once
#include "Ado.h"
#include "DBConnection.h"
#include "DBParamTypeDef.h"

class SqlServerDBConnection : public DBConnection
{
public:
	SqlServerDBConnection(void);
	~SqlServerDBConnection(void);
public:
	bool InitConnect(void);//初始化sqlserver数据库连接
	bool DisConnect(void);//断开sqlserver数据库连接
	bool IsConnected(void);
public:
	bool AddXmlTask(eDVBType type,std::string xml);//添加xml任务
	bool DeleteTask(eDVBType type,std::string xml);//删除xml任务
	bool QueryTask(eDVBType type,std::vector<std::string>& vecXml);//查询任务
	bool QueryTask(std::vector<std::string>& vecXml);//查询任务
	//频道扫描数据库操作
	bool AddChannel(eDVBType type,std::string channelid,std::string xml);//添加频道信息
	bool DeleteChannel(eDVBType type,std::string channelid);//删除频道信息
	bool DeleteChannelByType( eDVBType type);  //删除此类型的频道
	bool QueryChannel(eDVBType type,std::string channelid,std::string& xml);//查询指定类型和频道id的频道信息
	bool QueryAllChannel(std::vector<std::string>&vecXml);//查询所有频道信息
	bool QueryTypeChannel(eDVBType type,std::vector<std::string>&vecXml);//查询指定类型的频道信息
	bool QueryChannelID(eDVBType type,std::vector<std::string>&vecChannelID);//查询指定类型的频道id
	//录像信息数据库操作
	bool AddRecord30V(sRecordInfo record);
	bool AddRecord(sRecordInfo record);//添加录像信息
	//依据监测类型和任务id查找文件
    bool QueryRecordByTaskid(eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	//依据监测类型和ts流信息查找录像文件
	bool QueryRecordByChannelID(eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	//依据监测类型和通道信息查找录像文件
	bool QueryRecordByDeviceID(eDVBType type,std::string taskid,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	bool UpdateFileEndTime(std::string filename,std::string endtime);
	bool QueryLostRecordByChannelID(eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	bool GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string &EarliestStartTime);//通过对应DeviceID  and ChanneID 获取数据库该频道最早录制时间
	//磁盘管理使用接口
	bool DeleteRecord(std::string filename);//依据文件名删除录像信息
	bool QueryRecordByDays(unsigned int days,std::vector<std::string>& vecFilename);//依据过期时间获得录像信息
	bool QueryRecordTop(unsigned int count,std::vector<std::string>& vecFilename);//查询前几条录像记录
	bool QueryRecordExpire(std::vector<std::string>& vecFilename);//查询过期记录
	bool QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename);	//获取recPath路径下count个最老的录像文件	add by jidushan 11.05.04
	//报警参数
	bool UpdateAlarmParam(sAlarmParam& alarmparam);//更新报警参数
	bool QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec);//查询报警参数
	bool QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec);//查询报警参数
	//报警结果
	bool AddAlarmInfo(sCheckParam& alarminfo,std::string mode);
	bool UpdateAlarmInfo(eDVBType dvbtype,std::string Alarmid,string mode);
	bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode);
	bool QueryAlarmInfo(string mode,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryFreqAlarmID(eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid);
	bool QueryProgramAlarmID(eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid);
	bool DeleteAlarmInfo(std::string strAlarmTime);
	bool DeleteAlarmfromAlarmid(std::string strAlarmid);
	bool UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid);
	//运行图参数
	bool AddRunPlanParam(sRunPlanParam runPlanParam);//添加运行图参数
	bool QueryRunPlanParam(eDVBType type,std::vector<sRunPlanParam>& runPlanParam);//查询指定类型的运行图参数
	bool QueryAllRunPlanParam(std::vector<sRunPlanParam>& runPlanParam);//查询所有的运行图参数
	bool DeleteRunPlanParam(eDVBType type,std::string channelid);//删除指定类型的运行图参数
	bool UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam);//更新运行图参数
	//指标信息
	bool AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality);//添加指标信息
	bool AddQualityInfo(const std::string &strInsertSql);
	bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime);//查询指标信息
	bool DelQualityInfo(std::string strCheckTime);
	//频谱信息
	bool AddSpectrumInfo(eDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec,std::string checktime);//添加频谱信息
	bool QuerySpecInfo(eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecSpec,std::string startdatetime,std::string enddatetime);//查询频谱信息
	//MsgID
	bool GetMsgID(std::string &MsgID);//获得MsgID
	bool UpdateMsgID(std::string MsgID);//更新MsgID
	//指标补偿
	bool QueryCompensationValu(std::vector<sQualityCompensation>& qualityCmpVec);//查询指标补偿值
	bool UpdateCompensationValu(sQualityCompensation& qualityCmp);//查询指标补偿值
	bool AddCompensationValu(sQualityCompensation& qualityCmp);//添加指标补偿值
	bool DelCompensationValu(sQualityCompensation& qualityCmp);//删除指标补偿值

	//wz_101229
	//临时文件入库相关函数
	bool AddTempFile(string filename, string createtime, string expiredays);
	bool QueryTempFilesExpire( std::vector<std::string>& vecFilename );
	bool DeleteTempFile(std::string filename);
	bool QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename);
	bool QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename);
	//wz_101129
	//报警查询 2011-03-19
	bool AlarmInfoQuery(vector<sCheckParam> &vecAlarm);
	bool AlarmStateClear(string Freq);
	//配置信息查询
	bool QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue);

	bool QuerySystemConfig(eDVBType dvbtype, sSysConfigParam &config);
	bool QueryDvbConfig( eDVBType type,vector<sDvbConfigParam> &VecConfig);
	bool QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold);
	bool SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold);
	bool QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority);
	bool QueryScheduleAlarmInfo( string taskid );

	bool QueryLastRecordDateTime(std::string strFilePath,std::string &strDateTime);

	bool TruncateTable(std::string strTableName);

#ifdef ZONG_JU_ZHAO_BIAO

public:
	//设置监测类型的SrcCode
	bool SetGeneralSrcCode(eDVBType eType, std::string strSrcCode);
	//设置监测类型的DestCode
	bool SetGeneralDstCode(eDVBType eType, std::string strSrcCode);
	//设置监测类型的SrcCode
	bool SetGeneralUpUr(eDVBType eType, std::string strUpUr);

	//设置报警url
	bool SetAlarmUrl(eDVBType eDvbtype, std::string strAlarmUrl);
	//设置马赛克轮播上报URL   //FRee_fanrong_20140117
	bool SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strStreamRoundUrl);
	//设置录像路径
	bool SetRecordSavePath(std::string strRecordPath);
	//设置日志保存路径
	bool SetLogSavePath(std::string strLogdPath);

	//有线数字各前端日志保存路径    //FRee_fanrong_20140106
	bool SetVIDEOLogSavePath(std::string strLogPath) ;	//FRee_fanrong_20140106
	bool SetSILogSavePath(std::string strLogPath) ;		//FRee_fanrong_20140106
	bool SetCASLogSavePath(std::string strLogPath);		//FRee_fanrong_20140106
	bool SetSMSLogSavePath(std::string strLogPath) ;	//FRee_fanrong_20140106

	//设置cas sms si文件保存路径
	bool SetSmsFilePath(std::string strSmsPath);
	bool SetCasFilePath(std::string strCasPath);
	bool SetSIFilePath(std::string strCasPath);

	//设置录像的码率
	bool SetHDRecordBps(eDVBType eDvbtype, std::string strBps);
	bool SetSDRecordBps(eDVBType eDvbtype, std::string strBps);
	//设置录像的分辨率
	bool SetHDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth);
	bool SetSDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth);

	//查询某个频道录像文件的最早和最晚时间
	bool QueryRecVideoTime(eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID="");

	//
	bool SetAutoAnalysisTime(std::string strTime );
	bool GetAutoAnalysisTime(std::string& strTime );
	//
	bool AgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool GetAgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID);
	bool GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID);
	bool OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);	
	bool GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);
	bool AddUploadTask( sRecordUploadTaskInfo uploadtask );
	bool UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus);
	bool QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask );
#endif


private:
	CADODatabase* DBCon;
	CADODatabase* BandDBCon;
	bool BandDBOK;
};
