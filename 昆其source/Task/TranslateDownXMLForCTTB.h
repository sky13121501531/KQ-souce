///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForCTTB.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"
//所有接口从外部不区分什么样的检测类型，接口内部判断相应的检测类型并添加到相应的统一接口的 DownType 属性中

class TranslateDownXMLForCTTB
{
public:
	//返回标准的频道扫描任务XML
	static bool TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的实时视频任务XML
	static bool TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的视频轮播任务XML
	static bool TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi = false);
	//返回标准的录像设置任务XML
	static bool TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的录像执行任务XML
	static bool TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的录像查询、下载XML
	static bool TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的实时指标查询XML
	static bool TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的定时指标设置任务xml
	static bool TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的定时指标执行任务xml
	static bool TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的定时指标结果查询任务xml
	static bool TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的频道设置任务xml
	static bool TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的客户信息查询任务xml
	static bool TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的通道状态查询任务xml
	static bool TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的PSI/SI查询任务xml
	static bool TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的运行图设置任务xml
	static bool TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的手动录制任务xml
	static bool TranslateManualRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的报警参数设置xml
	static bool TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的EPG查询xml
	static bool TranslateEPGQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回指标补偿标准xml
	static bool TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//环境指标查询
	static bool TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的数据业务分析时间设置任务xml
	static bool TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//MHP数据查询
	static bool TranslateMHPQuery(const std::string& strOriginalXML,std::string& strStandardXML);
};