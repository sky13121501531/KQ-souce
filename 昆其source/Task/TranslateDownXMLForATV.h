///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForATV.h
// 创建者：gaoxd
// 创建时间：2010-04-09
// 内容描述：将下发XML转化为任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"
//所有接口从外部不区分什么样的检测类型，接口内部判断相应的检测类型并添加到相应的统一接口的 DownType 属性中

class TranslateDownXMLForATV
{
public:
	//返回标准的实时视频任务XML
	static bool TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的录像设置任务XML
	static bool TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的录像执行任务XML
	static bool TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的录像查询、下载XML
	static bool TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的频谱扫描XML
	static bool TranslateSpectrumScanQuery( const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的实时指标查询XML
	static bool TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的定时指标执行任务xml
	static bool TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的定时指标结果查询任务xml
	static bool TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//频道扫描
	static bool TranslateChannelScanQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//频道设置
	static bool TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//轮播
	static bool TranslateStreamRound(const std::string& strOriginalXML,std::string& strStandardXML);
	//报警参数设置
	static bool TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//频谱设置任务
	static bool TranslateSpectrumTaskSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//频谱任务
	static bool TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//频谱查询
	static bool TranslateSpectrumQueryTask(const std::string& strOriginalXML,std::string& strStandardXML);
	//指标查询
	static bool TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//客户信息
	static bool TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//运行图设置
	static bool TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//停止指定通道用户
	static bool TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML);
	//任务状态查询
	static bool TranslateTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//人工任务状态查询
	static bool TranslateManualTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//设备报警参数设置
	static bool TranslateEquipmentAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//配置信息查询(新安播)
	static bool TranslateInfoCheckRequest(const std::string& strOriginalXml, std::string& strStandardXml);
	//运行图信息查询(新安播)
	static bool TranslateRunPlanCheck(const std::string& strOriginalXml, std::string& strStandardXml);

	static bool TranslateAlarmExpireTask(const std::string& strOriginalXML,std::string& strStandardXML);

	static bool TranslateServerStatusSet(const std::string& strOriginalXML,std::string& strStandardXML);
};