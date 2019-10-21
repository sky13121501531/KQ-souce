///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForDVBC.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"
#include "../Foundation/Config.h"


class TranslateDownXMLForDVBC
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
	//返回标准的运行图查询任务xml
	static bool TranslateRunPlanCheck(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的自动录像设置查询任务xml
	static bool TranslateRecordSetQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的手动录制任务xml
	static bool TranslateManualRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的数据业务分析时间设置任务xml
	static bool TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//返回标准的数据业务分析时间查询任务xml
	static bool TranslateAutoAnalysisTimeQuery(const std::string& strOriginalXML,std::string& strStandardXML);
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
	//返回报警参数查询标准xml
	static bool TranslateAlarmParamQuery(const std::string& strOriginalXML,std::string& strStandardXML);
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

#ifdef ZONG_JU_ZHAO_BIAO
	//视频播放停止任务
	static bool TranslateStopPlayingVideo(const std::string& strOriginalXML,std::string& strStandardXML);
	//小卡卡号查询任务
	static bool TranslateICInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//板卡(通道)设置任务
	static bool TranslateNvrStatusSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//前端属性配置任务
	static bool TranslateAgentInfoSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//前端属性配置查询任务
	static bool TranslateAgentInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//前端重启任务
	static bool TranslateRebootSet(const std::string& strOriginalXML,std::string& strStandardXML);
	
	//板卡状态查看
	static bool TranslateNvrStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//前端属性配置任务
	static bool TranslateGetRecVideoTime(const std::string& strOriginalXML,std::string& strStandardXML);
	//小卡节目信息授权查询任务
	static bool TranslateChanEncryptQuery(const std::string& strOriginalXML,std::string& strStandardXML);
#endif
	//返回 红外遥控器 虚拟键 标准XML
	static bool TranslateInfraredRemoteControlQuery(
		const std::string& strOriginalXML, std::string& strStandardXML);
	//返回 OSD设置 标准XML
	static bool TranslateOSDFormatSetQuery(const std::string& strOriginalXML, std::string& strStandardXML);	
	//返回 OSD设置查询 标准XML
	static bool TranslateOSDFormatQuery(const std::string& strOriginalXML, std::string& strStandardXML);	
	//返回 前端信息查询 标准XML
	static bool TranslateGetAgentInfoQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateCardDevStatusQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateCameraQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateEquipmentHeartBeatQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateAutoRecordUploadTask(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateLimitedBandwidth(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateControlCardTimeQueryTask(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateSetCardStatusTask(const std::string& strOriginalXML, std::string& strStandardXML);
	static bool TranslateServerStatusSet(const std::string& strOriginalXML,std::string& strStandardXML);
	static bool TranslateDevStatusQueryTask(const std::string& strOriginalXML,std::string& strStandardXML);
};