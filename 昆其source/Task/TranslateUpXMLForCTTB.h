///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateUpXMLForCTTB.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将原子任务执行结果的XML转化为应用系统所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"

class TranslateUpXMLForCTTB
{
public:
	
	//返回实时视频XML
	static std::string TranslateStreamRealtimeQuery(const XMLTask* task);
	//返回轮播查询XML
	static std::string TranslateStreamRoundQuery( const XMLTask* task ,std::string deviceID = "",bool multi = false);
	//返回频道扫描XML
	static std::string TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML);
	//返回频道设置xml
	static std::string TranslateChannelSet(const XMLTask* task);
	//返回视频用户查询XML
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//PSI/SI查询XML
	static std::string TranslateTableQuery( const XMLTask* task,std::string strFileName);
	//返回录像设置XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	
	//返回指标任务设置xml
	static std::string TranslateQualitySet(const XMLTask* task);
	//返回指标查询XML
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);

	//返回报警参数设置xml
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	//返回通道状态查询XML
	static std::string TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus);
	//返回运行图设置XML
	static std::string TranslateRunPlanSet(const XMLTask* task);

	//自动录像视频流查询XML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//自动录像文件下载XML
	static std::string TranslateAutoRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//任务录像视频流查询XML
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//任务录像文件下载XML
	static std::string TranslateTaskRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//EPG查询XML
	static std::string TranslateEPGQuery(const XMLTask* task,std::string URL);
	//实时指标查询
	static std::string TranslateQualityRealtimeQuery(const XMLTask* task,std::string strXml);
	//数据业务分析时间设置XML
	static std::string TranslateAutoAnalysisTimeSet(const XMLTask* task);
	//环境指标查询XML
	static std::string TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec);
	//MHP查询XML
	static std::string TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord);
	//指标补偿设置XML
	static std::string TranslateQualityCompensationSet(const XMLTask *task);
	//手动录像设置
	static std::string TranslateManualRecordSet(const XMLTask* task);
	//手动录像主动上报
	static std::string TranslateManualRecordReport(const XMLTask *task,const std::string& strStandXML,std::string URL);
private:
	//构造返回XML头信息
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);
};
