///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordQueryTask.h
// 创建者：jiangcheng
// 创建时间：2009-06-22
// 内容描述：自动、任务录像查询、下载任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

typedef enum eRecordType
{
	AUTORECORDQUERY		= 0,	//自动录像查询
	AUTORECORDFILEQUERY	= 1,	//自动录像下载
	TASKRECORDQUERY		= 2,	//任务录像查询
	TASKRECORDFILEQUERY	= 3,	//任务录像下载
	RECORDFILELOSTQUERY	= 4,	//录像完整性查询
} enumRecordType;

class RecordQueryTask : public DeviceIndependentTask
{
public:
	RecordQueryTask();
	RecordQueryTask(std::string strXML);
	virtual ~RecordQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	//创建wpl或者m3u文件
	bool CreatePlayListFile(std::string listtype,std::string tempfilename,std::vector<sRecordInfo> vecFile,std::string Protocol);
	bool CreateDownFile(std::string tempfilename,std::vector<sRecordInfo> vecFile);	//创建下载文件
	bool CreateDownFileEX(std::string tempfilename,std::vector<sRecordInfo> vecFile);//追加文件内容

	bool RecordFileSpliter(vector < sRecordInfo>& fileVector,bool IsAudio);	//切割录像文件
	bool CreateListURL(std::vector<sRecordInfo> vecFile,std::vector<sRecordFileInfo> &FileInfo,std::string Protocol);//生成多个新安播录像下载URL
	bool CreateQualityQuery(string freq, string sStartTime, string sEndTime);
	string CreateRecordLostReport(string Freq, string ServiceID, sRecordInfo record, bool bStart,string RequireDeviceID);
private:
	enumRecordType RecordType;
	string RequireDeviceID ;
	string RequireOrgNetID,RequireTsID,RequireServiceID,RequireVideoPID,RequireAudioPID,RequireChannelCode;
	string StartDateTime,EndDateTime;
	string QueryStartDateTime,QueryEndDateTime;
	string RequireURL;
	string sQualityIndexSet;

	string VideoStreamProtocol,VideoFileProtocol,VideoFileurltype,FileOffset;	//文件查看参数
	string DownFileProtocol,DownFileurltype,DownOffset;							//文件下载参数
};