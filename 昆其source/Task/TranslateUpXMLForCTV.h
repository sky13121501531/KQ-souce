
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"

class TranslateUpXMLForCTV
{
public:
	//返回实时视频XML
	static std::string TranslateStreamRealtimeQuery(const XMLTask* task);
	//返回录像设置XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	//自动录像视频流查询XML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//频道扫描
	static std::string TranslateChannelScanQuery(const XMLTask* task,std::string strxml);
	//轮播
	static std::string TranslateStreamRound(const XMLTask* task,std::string deviceID);
	//频道设置
	static std::string TranslateChannelSet(const XMLTask* task);
	//报警参数设置
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	static std::string TranslateSpectrumSet(const XMLTask* task);
	static std::string TranslateQualitySet( const XMLTask* task );
	static std::string TranslateSpectrumQuery( const XMLTask* task,std::vector<eSpecInfo> vecSpecInfo);
	//定时录像查看
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//录像下载 add by jidushan 11.04.19
	static std::string TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//指标查询
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//停止指定通道用户
	static std::string TranslateStopClient( const XMLTask* task );
	//返回运行图设置XML
	static std::string TranslateRunPlanSet(const XMLTask* task);
	//任务状态查询
	static std::string TranslateTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
	//人工任务状态查询
	static std::string TranslateManualTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
private:
	//构造返回XML头信息
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);
};