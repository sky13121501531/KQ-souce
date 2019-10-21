
#ifndef __DBCONNECTION__
#define __DBCONNECTION__

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DBConnection.h
// 创建者：gaoxd
// 创建时间：2009-05-27
// 内容描述：数据库操作纯虚基类，定义不同监测类型中对不同表的各种操作
///////////////////////////////////////////////////////////////////////////////////////////

#include "DBParamTypeDef.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>
class DBConnection
{
public:    
    DBConnection(void);
    virtual ~DBConnection(void) = 0;
public:
	virtual bool InitConnect(void){return true;};//初始化数据库连接
	 virtual bool DisConnect(void){return true;};//断开数据库连接
	 virtual bool IsConnected(void){return true;};
public:
	//任务数据库操作
	virtual bool AddXmlTask(eDVBType type,std::string xml){return true;};//添加xml任务
	virtual bool DeleteTask(eDVBType type,std::string xml){return true;};//删除任务
	virtual bool QueryTask(eDVBType type,std::vector<std::string>&  vecXml){return true;};//查询任务
	virtual bool QueryTask(std::vector<std::string>&  vecXml){return true;};//查询任务
	//频道扫描数据库操作
	virtual bool AddChannel(eDVBType type,std::string channelid,std::string xml){return true;};//添加频道信息
	virtual bool DeleteChannel(eDVBType type,std::string channelid){return true;};//删除频道信息
	virtual bool DeleteChannelByType(eDVBType type){return true;};//根据类型删除频道
	virtual bool QueryChannel(eDVBType type,std::string channelid,std::string& xml){return true;};//依据类型和频道id查询频道信息
	virtual bool QueryTypeChannel(eDVBType type,std::vector<std::string>&vecXml){return true;};//依据类型查询频道信息
	virtual bool QueryAllChannel(std::vector<std::string>&vecXml){return true;};//查询所有频道信息
	virtual bool QueryChannelID(eDVBType type,std::vector<std::string>&vecChannelID){return true;};//查询特定类型的频道id
	//录像信息数据库操作
	virtual bool AddRecord30V(sRecordInfo record){return true;};//添加录像信息
	virtual bool AddRecord(sRecordInfo record){return true;};//添加录像信息
	virtual bool QueryRecordByTaskid(eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime=""){return true;};//依据监测类型和任务id查找文件
	virtual bool QueryRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID=""){return true;};//依据监测类型和频道信息查找录像文件
	virtual	bool QueryRecordByDeviceID(eDVBType type,std::string taskid,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime=""){return true;};  //依据监测类型和通道信息查找录像文件
	virtual bool UpdateFileEndTime(std::string filename,std::string endtime){return true;};
	virtual bool QueryLostRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID=""){return true;};//依据监测类型和频道信息查找录像文件
	virtual	bool GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string &EarliestStartTime){return true;};
	//磁盘管理使用接口
	virtual bool DeleteRecord(std::string filename){return true;};//依据文件名删除录像信息
	virtual bool QueryRecordByDays(unsigned int days,std::vector<std::string>& vecFilename){return true;};//依据过期时间删除录像信息
	virtual bool QueryRecordTop(unsigned int count,std::vector<std::string>& vecFilename){return true;};//查询前天条录像记录
	virtual bool QueryRecordExpire(std::vector<std::string>& vecFilename){return true;};//查询过期记录
	virtual bool QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename) {return true;}	//获取recPath路径下count个最老的录像文件	add by jidushan 11.05.04
	//报警参数
	virtual bool UpdateAlarmParam(sAlarmParam& alarmparam){return true;};//更新报警参数
	virtual bool QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec){return true;};//查询报警参数
	virtual bool QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec){return true;};//查询报警参数
	//报警方式
	virtual bool AddAlarmInfo(AlarmInfo& alarminfo){return true;};
	virtual bool QueryAlarmInfo(eDVBType type,std::string freq,std::string channelid,std::string typeId,vector<AlarmInfo>& vecAlarmInfo){return true;};
	virtual bool DeleteAlarmInfo(std::string strAlarmTime)	{ return true ; }
	virtual bool DeleteAlarmfromAlarmid(std::string strAlarmid)	{ return true ; }
	//运行图参数
	virtual bool QueryRunPlanParam(eDVBType type,std::vector<sRunPlanParam>& runPlanParam){return true;};//查询指定类型的运行图参数
	virtual bool QueryAllRunPlanParam(std::vector<sRunPlanParam>& runPlanParam){return true;};//查询所有的运行图参数
	virtual bool UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam){return true;};//更新运行图参数
	//指标信息
	virtual bool AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality){return true;};//添加指标信息
	virtual bool AddQualityInfo(const std::string &strInsertSql)	{ return true ; }
	virtual bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime){return true;};//查询指标信息
	virtual bool DelQualityInfo(std::string strCheckTime)	{return true ; }
	//频谱信息
	virtual bool AddSpectrumInfo(eDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec,std::string checktime){return true;};//添加频谱信息
	virtual bool QuerySpecInfo(eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecSpec,std::string startdatetime,std::string enddatetime){return true;};//查询频谱信息
	//MsgID
	virtual bool GetMsgID(std::string &MsgID){return true;};//获得MsgID
	virtual bool UpdateMsgID(std::string MsgID){return true;};//更新MsgID
	//AnalysisTime
	virtual bool GetAutoAnalysisTime(std::string &AnalysisTime){return true;};//获得MsgID
	virtual bool SetAutoAnalysisTime(std::string AnalysisTime){return true;};//更新MsgID
	//指标补偿
	virtual bool QueryCompensationValu(std::vector<sQualityCompensation>& qualityCmpVec){return true;};//查询指标补偿值
	virtual bool UpdateCompensationValu(sQualityCompensation& qualityCmp){return true;};//查询指标补偿值
	//AlarmID
	virtual bool AddAlarmID(string type,string alarmid){return true;};
	virtual bool QueryAlarmID(string type,string&  alarmid){return true;};
	virtual bool DeleteAlarmID(string type){return true;};
	//报警结果
	virtual bool AddAlarmInfo(sCheckParam& alarminfo,std::string mode){return true;};
	virtual bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string mode){return true;};
	virtual bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode){return true;};
	virtual bool QueryAlarmInfo(string mode,std::vector<sCheckParam>& vecAlarmInfo){return true;};
	virtual bool QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo){return true;};
	virtual bool QueryFreqAlarmID(eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid){return true;};
	virtual bool QueryProgramAlarmID(eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid){return true;};
	virtual bool UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid){return true;};
	//wz_101229
	//临时文件入库
	virtual bool AddTempFile(string filename, string createtime, string expiredays){return true;};
	virtual bool QueryTempFilesExpire( std::vector<std::string>& vecFilename ){return true;};
	virtual bool DeleteTempFile(std::string filename){return true;};
	virtual bool QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename){return true;};
	virtual bool QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename){return true;};
	//wz_101229
	//报警查询2011-03-19
	virtual bool AlarmInfoQuery(vector<sCheckParam> &vecAlarm){return true;};
	//报警状态清除 2011-03-19
	virtual bool AlarmStateClear(string Freq){return true;};

	virtual bool QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue){return true;};

	virtual bool QuerySystemConfig(eDVBType type, sSysConfigParam &config){return true;};
	virtual bool QueryDvbConfig(eDVBType type, vector<sDvbConfigParam> &VecConfig){return true;};
	virtual bool QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold){return true;};
	virtual bool SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold){return true;};
	virtual bool QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority){return true;};
	virtual bool QueryScheduleAlarmInfo( string taskid ){return true;};

	virtual bool QueryLastRecordDateTime(std::string strPath, std::string &strDateTime)	{ return true ; }

	virtual bool TruncateTable(std::string strTableName)	{return true ; }

#ifdef ZONG_JU_ZHAO_BIAO
	//设置监测类型的SrcCode
	virtual bool SetGeneralSrcCode(eDVBType eType, std::string strSrcCode){ return true ; }

	virtual bool SetGeneralUpUr(eDVBType eType, std::string strUpUr){ return true ; }
	//设置监测类型的SrcCode
	virtual bool SetGeneralDstCode(eDVBType eType, std::string strDstCode){ return true ; }
	//设置报警url
	virtual bool SetAlarmUrl(eDVBType eDvbtype, std::string strAlarmUrl) {return true;};

	//设置马赛克轮播上报URL   //FRee_fanrong_20140117
	virtual bool SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strAlarmUrl) {return true;};

	//设置录像保存路径
	virtual bool SetRecordSavePath(std::string strRecordPath) {return true;};
	//设置服务器日志保存路径
	virtual bool SetLogSavePath(std::string strLogPath) {return true;};
	//有线数字各前端日志保存路径					//FRee_fanrong_20140106
	virtual bool SetVideoLogSavePath(std::string strLogPath) {return true;};    //FRee_fanrong_20140107
	virtual bool SetSILogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	virtual bool SetCASLogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	virtual bool SetSMSLogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	
	//设置sms cas文件保存路径
	virtual bool SetSmsFilePath(std::string strSmsPath) {return true;};
	virtual bool SetCasFilePath(std::string strCasPath) {return true;};

	//设置SI 文件保存路径
	virtual bool SetSIFilePath(std::string strSIPath) {return true;};

	virtual bool SetHDRecordBps(eDVBType eDvbtype, std::string strBps)	{ return true; }
	virtual bool SetSDRecordBps(eDVBType eDvbtype, std::string strBps)	{ return true; }

	//查询某个频道录像文件的最早和最晚时间
	virtual bool QueryRecVideoTime(eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID=""){return true;}

	//设置录像的分辨率
	virtual bool SetHDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth)	{return true ; }
	virtual bool SetSDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth)	{return true ; }

	//
	virtual bool AgentSetInfo(eDVBType eDvbtype, vector<sAgentSetInfo> &VecAgentSetInfo)	{return true ; }
	virtual bool GetAgentSetInfo(eDVBType eDvbtype, vector<sAgentSetInfo> &VecAgentSetInfo)	{return true ; }
	virtual bool AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID)	{return true ; }
	virtual bool GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID)	{return true ; }
	virtual bool OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)	{return true ; }
	virtual bool GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)	{return true ; }
	virtual bool AddUploadTask( sRecordUploadTaskInfo uploadtask )	{return true ; }
	virtual bool UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus)	{return true ; }
	virtual bool QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask )	{return true ; }

#endif

	//virtual bool DeleteQualityInfo() {return true ; }
};

#endif
