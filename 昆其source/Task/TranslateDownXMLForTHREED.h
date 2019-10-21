///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForTHREED.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"

class TranslateDownXMLForTHREED
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

	//返回标准的偱切录像设置任务XML
	static bool TranslateRoundRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的偱切录像执行任务XML
	static bool TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);

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
	//返回标准的表查询任务xml
	static bool TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的运行图设置任务xml
	static bool TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的手动录制任务xml
	static bool TranslateManualRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的数据业务分析时间设置任务xml
	static bool TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//EPG电子节目单查询
	static bool TranslateEPGQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//MHP数据查询
	static bool TranslateMHPQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//环境指标查询
	static bool TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的频谱扫描任务xml
	static bool TranslateSpectrumScan(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回指标补偿标准xml
	static bool TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回报警参数设置标准xml
	static bool TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//报警上报方式
	static bool TranslateAlarmReportSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//报警查询
	static bool TranslateAlarmQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//OSD设置
	static bool TranslateOSDSet(const std::string& strOriginalXML,std::string& strStandardXML);
	static bool TranslateStreamRealtimeRate(const std::string& strOriginalXML,std::string& strStandardXML);
	//录像路数查询
	static bool TranslateRecordCapabilityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//高清音频默认参数设置
	static bool TranslateHDDefAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//音频参数设置
	static bool TranslateAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//报警状态清除 
	static bool TranslateClearAlarmState(const std::string& strOriginalXML,std::string& strStandardXML);
	//矩阵切换
	static bool TranslateMatrixQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//QAM设置
	static bool TranslateChangeQAMQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//循切报警设置  (页面xml与接口文档不同-->AlaType的有无)
	static bool TranslateLoopAlarmInfo(const std::string& strOriginalXML,std::string& strStandardXML);
	//视频转码录像默认参数设置 (页面xml与接口文档不同-->xml结构就不一样)
	static bool TranslateRecordParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//用户连接中断
	static bool TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML);
};