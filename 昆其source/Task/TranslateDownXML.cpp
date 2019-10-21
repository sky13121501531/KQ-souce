
#include "TranslateDownXML.h"
#include "TranslateDownXMLForCTTB.h"
#include "TranslateDownXMLForDVBC.h"
#include "TranslateDownXMLForRADIO.h"
#include "TranslateDownXMLForATV.h"
#include "TranslateDownXMLForAM.h"
#include "TranslateDownXMLForDVBS.h"
#include "TranslateDownXMLForCTV.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"

bool TranslateDownXML::TranslateTaskXML(const std::string& strOriginalXML,std::string& strStandardXML)
{
	pXMLNODE RootNode = NULL;
	pXMLNODE childNode = NULL;
	std::string RootName,TaskName,DVBType,DstCode,Version;

	try
	{
		if (strOriginalXML.empty()) return false;
		if (!(strOriginalXML.find("Msg")!=-1||strOriginalXML.find("msg")!=-1)) return false;

		XmlParser pXP((const char *)strOriginalXML.c_str());
		RootName= pXP.GetRootNodeName();				//根节点名字
		
		RootNode = pXP.GetRootNode();					//根节点

		if (RootName != "Msg" && RootName != "msg")//判断根节点的名字是否为msg
		{
			string msg = "XML文件格式错误，无法生成标准XML:" + strOriginalXML;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			return false;
		}

		childNode = pXP.GetNodeFirstChild(RootNode);		//根节点的第一个子节点
		pXP.GetAttrNode(RootNode,"Type",DVBType);			//获得消息类型
		pXP.GetAttrNode(RootNode,"DstCode",DstCode);
		pXP.GetAttrNode(RootNode,"Version",Version);        //版本
		if (DVBType.empty())
		{
			pXP.GetAttrNode(RootNode,"DVBType",DVBType);	//获得消息类型
		}

		TaskName = pXP.GetNodeName(childNode);				//获得任务名称

		if (DVBType=="SMSDown"||DVBType=="CASDown")
		{
			strStandardXML=strOriginalXML;
			if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0" && TaskName == "EntitlementQuery")
			{
				XmlParser parser(strStandardXML.c_str());
				pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
				string SrcUrl = PROPMANAGER::instance()->GetAgentInfoSet("14");
				parser.SetAttrNode( "SrcURL",SrcUrl,rootNode );
				parser.SaveToString( strStandardXML );
			}
			return true;
		}
		
		if (DVBType.empty() || TaskName.empty())
		{
			string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			return false;
		}

		//录像、指标任务
		if ((DVBType == "CTTB" || DVBType == "DVBC" || DVBType == "DVBS" || DVBType == "RADIO" || DVBType == "ATV" || DVBType == "AM" || DVBType == "CTV" || DVBType == "THREED") && 
			(TaskName=="AutoRecord" || TaskName=="TaskRecord" || TaskName=="QualityTask" ||TaskName=="SpectrumTask" || TaskName=="RoundRecord"
#ifdef ZONG_JU_ZHAO_BIAO
			||TaskName=="EncryptCheckTask"
#endif
			))
		{
			strStandardXML = strOriginalXML;
			return true;
		}
		if(DVBType.length()<1&&TaskName.length()<1)
		{
			return TranslateDownXMLForDVBC::TranslateServerStatusSet(strOriginalXML,strStandardXML);
		}
		if(DVBType == "CommonDown" && TaskName=="DeviceManage" )
		{
			pXP.SetAttrNode("TaskType",string("DeviceManage"),RootNode);
			pXP.SaveToString(strStandardXML);
			return true;
		}
		if(/*DVBType.length()<1 && */TaskName=="DeviceInfo")
		{
			pXP.SetAttrNode("TaskType",string("DeviceInfo"),RootNode);//总局新接口：设备信息查询没有监测类型全设为开路电视类型
			pXP.SetAttrNode("DVBType",string("ATV"),RootNode);
			pXP.SaveToString(strStandardXML);
			return true;
		}
		if (DVBType == "DVBTHDown")	//地面类型任务
		{
			if (TaskName=="StreamRealtimeQuery")										//实时视频
			{
				return TranslateDownXMLForCTTB::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundQuery")										//视频轮播
			{
				return TranslateDownXMLForCTTB::TranslateStreamRoundQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel" || TaskName=="TaskRecordSet")		//录像设置
			{
				return TranslateDownXMLForCTTB::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AutoRecordFileQuery" || TaskName=="AutoRecordQuery" || 
					 TaskName=="TaskRecordFileQuery" || TaskName=="TaskRecordQuery")	//录像查询、下载
			{
				return TranslateDownXMLForCTTB::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityRealtimeQuery")									//实时指标查询
			{
				return TranslateDownXMLForCTTB::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityTaskSet")										//定时指标设置
			{
				return TranslateDownXMLForCTTB::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelSet")                                            //频道设置
			{
				return TranslateDownXMLForCTTB::TranslateChannelSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ManualRecordQuery")										//手动录制
			{
				return TranslateDownXMLForCTTB::TranslateManualRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ClientInfoQuery")                                       //客户信息查询
			{
				return TranslateDownXMLForCTTB::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="GetDeviceStatus")                                       //通道状态查询
			{
				return TranslateDownXMLForCTTB::TranslateDeviceStatusQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="IndexCompensationSet")								    //指标补偿设置
			{
				return TranslateDownXMLForCTTB::TranslateQualityCompensationSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="RunPlanSet")                                            //运行图设置
			{
				return TranslateDownXMLForCTTB::TranslateRunPlanSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName =="AutoAnalysisTimeQuery")								//数据业务分析时间设置
			{
				return TranslateDownXMLForCTTB::TranslateAutoAnalysisTimeSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ChannelScanQuery")										//频道扫描
			{
				return TranslateDownXMLForCTTB::TranslateChannelScan(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="TableQuery")                                             //表查询
			{
				return TranslateDownXMLForCTTB::TranslateTableQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName =="EPGQuery")												//EPG查询
			{
				return TranslateDownXMLForCTTB::TranslateEPGQuery(strOriginalXML,strStandardXML);	
			}
			else if (TaskName=="MHPQuery")												//MHP数据查询
			{
				return TranslateDownXMLForCTTB::TranslateMHPQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityQuery")											//定时指标结果查询
			{
				return TranslateDownXMLForCTTB::TranslateQualityQuery(strOriginalXML,strStandardXML); 
			}
			else if(TaskName=="AlarmParamSet")											//报警参数设置
			{
				return TranslateDownXMLForCTTB::TranslateAlarmParamSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="GetIndexESet")											//环境指标查询
			{
				return TranslateDownXMLForCTTB::TranslateEnvironmentQualityQuery(strOriginalXML,strStandardXML); 
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,CTTB,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if ((DVBType == "MonDown")||(DVBType == "MONDown")||(DVBType == "DVBCDown")||(DVBType == "DTMBDown")
			||(DVBType == "DVBSDown")||(DVBType == "RADIODown")||(DVBType == "AMDown"))
		{
			if (TaskName=="ChangeProgramQuery" || TaskName=="RealStreamQuery")													//实时视频
			{
				return TranslateDownXMLForDVBC::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundInfoQuery")											//视频轮播
			{
				return TranslateDownXMLForDVBC::TranslateStreamRoundQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel" || TaskName=="ProvisionalRecordTaskSet")	//录像设置
			{
				return TranslateDownXMLForDVBC::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RoundRecordSet")												//偱切录像
			{
				return TranslateDownXMLForDVBC::TranslateRoundRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="NVRVideoHistoryInquiry" || TaskName=="NVRVideoHistoryDownInquiry" ||
				TaskName=="NVRTaskRecordInquiry"   || TaskName=="NVRTaskRecordDownInquiry" ||
				TaskName=="NVRVideoHistoryLostInquiry" || TaskName=="AutoRecordQuery"||
				TaskName=="AutoRecordDownload" || TaskName=="AutoRecordIntegraity")//录像查询、下载、完整性查询
			{
				return TranslateDownXMLForDVBC::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="GetIndexSet")||(TaskName=="IndexQuery"))													//实时指标查询
			{
				string strXml = strOriginalXML;
				string sBeginTime, sEndTime;
				if ((strXml.find("beginTime") != std::string::npos)&& (strXml.find("endTime") != std::string::npos))
				{
					pXP.GetAttrNode(childNode, "beginTime", sBeginTime);
					pXP.GetAttrNode(childNode, "endTime", sEndTime);
				}
				else if((strXml.find("BeginTime") != std::string::npos)&& (strXml.find("EndTime") != std::string::npos))
				{
					pXP.GetAttrNode(childNode, "BeginTime", sBeginTime);
					pXP.GetAttrNode(childNode, "EndTime", sEndTime);
				}
				else if((strXml.find("StartDateTime") != std::string::npos)&& (strXml.find("EndDateTime") != std::string::npos))
				{
					pXMLNODE timechildNode = pXP.GetNodeFirstChild(childNode);		//根节点的第一个子节点
					pXP.GetAttrNode(timechildNode, "StartDateTime", sBeginTime);
					pXP.GetAttrNode(timechildNode, "EndDateTime", sEndTime);
				}
				if (sBeginTime.size() > 0 && sEndTime.size() > 0)
				{//201512数字 指标查询
					return TranslateDownXMLForDVBC::TranslateQualityQuery(strOriginalXML,strStandardXML); 
				}
				else
				{
					return TranslateDownXMLForDVBC::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML);
				}
			}
			else if (TaskName=="QualityTaskSet")												//定时指标设置
			{
				return TranslateDownXMLForDVBC::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityQuery")													//定时指标结果查询
			{
				return TranslateDownXMLForDVBC::TranslateQualityQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ChannelSet")													//频道设置
			{
				return TranslateDownXMLForDVBC::TranslateChannelSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ManualRecordQuery")												//手动录制
			{
				return TranslateDownXMLForDVBC::TranslateManualRecordSet(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="AutoAnalysisTimeQuery" && Version!="3.0")||TaskName=="AutoAnalysisTimeSet")		//数据业务分析时间设置
			{
				return TranslateDownXMLForDVBC::TranslateAutoAnalysisTimeSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AutoAnalysisTimeQuery" && Version=="3.0")											//数据业务分析时间查询3.0
			{
				return TranslateDownXMLForDVBC::TranslateAutoAnalysisTimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelScanQuery")												//频道扫描
			{
				return TranslateDownXMLForDVBC::TranslateChannelScan(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="EPGQuery")||(TaskName=="EPGScanQuery"))														//EPG查询
			{
				return TranslateDownXMLForDVBC::TranslateEPGQuery(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="table")||(TaskName=="TableScanQuery"))														//表查询
			{
				return TranslateDownXMLForDVBC::TranslateTableQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="MHPQuery")														//MHP数据查询
			{
				return TranslateDownXMLForDVBC::TranslateMHPQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumScanQuery")												//频谱扫描
			{
				return TranslateDownXMLForDVBC::TranslateSpectrumScan(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AlarmTimeSet" || TaskName=="RunningChartSet")					//运行图设置
			{
				return TranslateDownXMLForDVBC::TranslateRunPlanSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="RunningChartQuery")				                                //运行图查询 3.0
			{
				return TranslateDownXMLForDVBC::TranslateRunPlanCheck(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="AutoRecordChannelQuery")				                        //自动录像设置查询 3.0
			{
				return TranslateDownXMLForDVBC::TranslateRecordSetQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="IndexCompensationSet")											//指标补偿设置
			{
				return TranslateDownXMLForDVBC::TranslateQualityCompensationSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ClientInfoQuery")												//客户信息查询
			{
				return TranslateDownXMLForDVBC::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
#ifdef ZONG_JU_ZHAO_BIAO
			else if (TaskName=="GetNvrStatus")													//通道状态查询
			{
				return TranslateDownXMLForDVBC::TranslateNvrStatusQuery(strOriginalXML,strStandardXML); 
			}
#else
			else if (TaskName=="GetNvrStatus")													//通道状态查询
			{
				return TranslateDownXMLForDVBC::TranslateDeviceStatusQuery(strOriginalXML,strStandardXML); 
			}
#endif
			else if (TaskName=="GetIndexESet")													//环境指标查询
			{
				return TranslateDownXMLForDVBC::TranslateEnvironmentQualityQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="AlarmThresholdSet"||TaskName=="AlarmProgramSwitchSet"||			//报警参数设置
				TaskName=="AlarmSwitchSet"||TaskName=="AlarmProgramThresholdSet"||
				TaskName=="AlarmRFThresholdSet"||TaskName=="AlarmStreamThresholdSet"||
				TaskName=="AlarmStreamSwitchSet"||TaskName=="AlarmRfSwitchSet"||
				TaskName=="AlarmChannelSwitchSet"||TaskName=="AlarmChannelThresholdSet"||
				TaskName=="AlarmRFSwitchSet")														//环境指标查询
			{
				return TranslateDownXMLForDVBC::TranslateAlarmParamSet(strOriginalXML,strStandardXML); 
			}

			else if (TaskName=="AlarmRFInfoQuery"||TaskName=="AlarmStreamInfoQuery"||TaskName=="AlarmChannelInfoQuery")		//报警参数查询									
			{
				return TranslateDownXMLForDVBC::TranslateAlarmParamQuery(strOriginalXML,strStandardXML); 
			}

			else if(TaskName=="AlarmTypeSet")													//报警方式类型设置
			{
				return TranslateDownXMLForDVBC::TranslateAlarmReportSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="OSDSet")
			{
				return TranslateDownXMLForDVBC::TranslateOSDSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="NVRSteamRateSet")
			{
				return TranslateDownXMLForDVBC::TranslateStreamRealtimeRate(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="RecordCapabilityQuery")
			{
				return TranslateDownXMLForDVBC::TranslateRecordCapabilityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="HDDefAudioParamSet")
			{
				return TranslateDownXMLForDVBC::TranslateHDDefAudioParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="AudioParamSet")
			{
				return TranslateDownXMLForDVBC::TranslateAudioParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="ClearAlarmState")  //报警状态清除
			{
				return TranslateDownXMLForDVBC::TranslateClearAlarmState(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="MatrixQuery")  //矩阵切换
			{
				return TranslateDownXMLForDVBC::TranslateMatrixQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="ChangeQAMQuery")  //QAM设置
			{
				return TranslateDownXMLForDVBC::TranslateChangeQAMQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="LoopAlaInf")  //循切报警设置
			{
				return TranslateDownXMLForDVBC::TranslateLoopAlarmInfo(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="RecordParamSet")  //视频转码录像默认参数设置
			{
				return TranslateDownXMLForDVBC::TranslateRecordParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="AlarmSearchESet"||TaskName=="AlarmSearchFSet"||TaskName=="AlarmSearchPSet")
			{
				return TranslateDownXMLForDVBC::TranslateAlarmQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="VideoClientStop")
			{
				return TranslateDownXMLForDVBC::TranslateStopClient(strOriginalXML,strStandardXML);
			}
#ifdef ZONG_JU_ZHAO_BIAO
			else if(TaskName=="StopPlayingVideo")	//视频播放停止任务
			{
				return TranslateDownXMLForDVBC::TranslateStopPlayingVideo(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="ICInfoQuery")		//小卡卡号查询任务
			{
				return TranslateDownXMLForDVBC::TranslateICInfoQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="NvrStatusSet")		//板卡(通道)设置任务
			{
				return TranslateDownXMLForDVBC::TranslateNvrStatusSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AgentInfoSet")		//前端属性配置任务
			{
				return TranslateDownXMLForDVBC::TranslateAgentInfoSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AgentInfoQuery")		//前端属性配置查询任务
			{
				return TranslateDownXMLForDVBC::TranslateAgentInfoQuery(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="GetRecordVideoTime")||(TaskName=="AutoRecordExpiryDate"))		//录像询时
			{
				return TranslateDownXMLForDVBC::TranslateGetRecVideoTime(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RebootSet")		//前端重启
			{
				return TranslateDownXMLForDVBC::TranslateRebootSet(strOriginalXML,strStandardXML);
			}
			else if ((TaskName=="ICInfoChannelEncryptQuery")||(TaskName=="ICCardEncryptQuery"))	//
			{
				return TranslateDownXMLForDVBC::TranslateChanEncryptQuery(strOriginalXML,strStandardXML);
			}
#endif
			else if ((TaskName == "SendVirtualKey")||(TaskName == "STBVirtualController"))
			{
				return TranslateDownXMLForDVBC::TranslateInfraredRemoteControlQuery(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "OSDFormatSet")
			{
				return TranslateDownXMLForDVBC::TranslateOSDFormatSetQuery(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "OSDFormatQuery")
			{
				return TranslateDownXMLForDVBC::TranslateOSDFormatQuery(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "GetAgentInfo")
			{
				return TranslateDownXMLForDVBC::TranslateGetAgentInfoQuery(strOriginalXML, strStandardXML);
			}
			/*else if (TaskName == "DevStatus" || TaskName == "DevStatusQuery")
			{
				return TranslateDownXMLForDVBC::TranslateCardDevStatusQuery(strOriginalXML, strStandardXML);
			}*/
			else if (TaskName == "CameraQuery")
			{
				return TranslateDownXMLForDVBC::TranslateCameraQuery(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "EquipmentHeartBeatQuery")
			{
				return TranslateDownXMLForDVBC::TranslateEquipmentHeartBeatQuery(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "LimitedBandwidth")
			{
				return TranslateDownXMLForDVBC::TranslateLimitedBandwidth(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "AutoRecordUploadTask")
			{
				return TranslateDownXMLForDVBC::TranslateAutoRecordUploadTask(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "ControlCardTimeQuery")  //主控卡当前时间查询
			{
				return TranslateDownXMLForDVBC::TranslateControlCardTimeQueryTask(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "SetCardStatus")  //机箱板卡使用状态设置
			{
				return TranslateDownXMLForDVBC::TranslateSetCardStatusTask(strOriginalXML, strStandardXML);
			}
			else if(TaskName == "ServerStatusSet")
			{
				return TranslateDownXMLForDVBC::TranslateServerStatusSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "DevStatusQuery")
			{
				return TranslateDownXMLForDVBC::TranslateDevStatusQueryTask(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,DVBC,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if (DVBType == "SatDown")//卫星类型任务
		{
			if (TaskName=="ChangeProgramQuery")													//实时视频
			{
				return TranslateDownXMLForDVBS::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundInfoQuery")											//视频轮播
			{
				return TranslateDownXMLForDVBS::TranslateStreamRoundQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel" || TaskName=="ProvisionalRecordTaskSet")	//录像设置
			{
				return TranslateDownXMLForDVBS::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RoundRecordSet")												//偱切录像
			{
				return TranslateDownXMLForDVBS::TranslateRoundRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="NVRVideoHistoryInquiry" || TaskName=="NVRVideoHistoryDownInquiry" ||
				TaskName=="NVRTaskRecordInquiry"   || TaskName=="NVRTaskRecordDownInquiry" )//录像查询、下载
			{
				return TranslateDownXMLForDVBS::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="GetIndexSet")													//实时指标查询
			{
				return TranslateDownXMLForDVBS::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityTaskSet")												//定时指标设置
			{
				return TranslateDownXMLForDVBS::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityQuery")													//定时指标结果查询
			{
				return TranslateDownXMLForDVBS::TranslateQualityQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ChannelSet")													//频道设置
			{
				return TranslateDownXMLForDVBS::TranslateChannelSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ManualRecordQuery")												//手动录制
			{
				return TranslateDownXMLForDVBS::TranslateManualRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AutoAnalysisTimeQuery")											//数据业务分析时间设置
			{
				return TranslateDownXMLForDVBS::TranslateAutoAnalysisTimeSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelScanQuery")												//频道扫描
			{
				return TranslateDownXMLForDVBS::TranslateChannelScan(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="EPGQuery")														//EPG查询
			{
				return TranslateDownXMLForDVBS::TranslateEPGQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="table")															//表查询
			{
				return TranslateDownXMLForDVBS::TranslateTableQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="MHPQuery")														//MHP数据查询
			{
				return TranslateDownXMLForDVBS::TranslateMHPQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumScanQuery")												//频谱扫描
			{
				return TranslateDownXMLForDVBS::TranslateSpectrumScan(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="AlarmTimeSet")													//运行图设置
			{
				return TranslateDownXMLForDVBS::TranslateRunPlanSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="IndexCompensationSet")											//指标补偿设置
			{
				return TranslateDownXMLForDVBS::TranslateQualityCompensationSet(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ClientInfoQuery")												//客户信息查询
			{
				return TranslateDownXMLForDVBS::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="GetNvrStatus")													//通道状态查询
			{
				return TranslateDownXMLForDVBS::TranslateDeviceStatusQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="GetIndexESet")													//环境指标查询
			{
				return TranslateDownXMLForDVBS::TranslateEnvironmentQualityQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="AlarmThresholdSet"||TaskName=="AlarmProgramSwitchSet"||			//报警参数设置
				TaskName=="AlarmSwitchSet"||TaskName=="AlarmProgramThresholdSet")														//环境指标查询
			{
				return TranslateDownXMLForDVBS::TranslateAlarmParamSet(strOriginalXML,strStandardXML); 
			}
			else if(TaskName=="AlarmTypeSet")													//报警方式类型设置
			{
				return TranslateDownXMLForDVBS::TranslateAlarmReportSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="OSDSet")
			{
				return TranslateDownXMLForDVBS::TranslateOSDSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="NVRSteamRateSet")
			{
				return TranslateDownXMLForDVBS::TranslateStreamRealtimeRate(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="RecordCapabilityQuery")
			{
				return TranslateDownXMLForDVBS::TranslateRecordCapabilityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="HDDefAudioParamSet")
			{
				return TranslateDownXMLForDVBS::TranslateHDDefAudioParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="AudioParamSet")
			{
				return TranslateDownXMLForDVBS::TranslateAudioParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="ClearAlarmState")  //报警状态清除
			{
				return TranslateDownXMLForDVBS::TranslateClearAlarmState(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="MatrixQuery")  //矩阵切换
			{
				return TranslateDownXMLForDVBS::TranslateMatrixQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="ChangeQAMQuery")  //QAM设置
			{
				return TranslateDownXMLForDVBS::TranslateChangeQAMQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="LoopAlaInf")  //循切报警设置
			{
				return TranslateDownXMLForDVBS::TranslateLoopAlarmInfo(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="RecordParamSet")  //视频转码录像默认参数设置
			{
				return TranslateDownXMLForDVBS::TranslateRecordParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="AlarmSearchESet"||TaskName=="AlarmSearchFSet"||TaskName=="AlarmSearchPSet")
			{
				return TranslateDownXMLForDVBS::TranslateAlarmQuery(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,DVBS,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if (DVBType == "RadioDown")//模拟广播类型任务
		{
			if (TaskName=="StreamRealtimeQuery")												//实时视频
			{
				return TranslateDownXMLForRADIO::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel"||TaskName=="TaskSet")											//录像设置
			{
				return TranslateDownXMLForRADIO::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamSimpleQuery"||"TaskRecordQuery"==TaskName || TaskName == "TaskRecordFileQuery" || TaskName == "RecordFileQuery")												//录像查询、下载
			{
				return TranslateDownXMLForRADIO::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumRealtimeScan")											//频谱扫描
			{
				return TranslateDownXMLForRADIO::TranslateSpectrumScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityRealtimeQuery")											//实时指标查询
			{
				return TranslateDownXMLForRADIO::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML); 
			}	
			else if (TaskName=="ChannelScanQuery")
			{
				return TranslateDownXMLForRADIO::TranslateChannelScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundQuery")
			{
				return TranslateDownXMLForRADIO::TranslateStreamRound(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelSet")
			{
				return TranslateDownXMLForRADIO::TranslateChannelSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityAlarmParamSet")
			{
				return TranslateDownXMLForRADIO::TranslateAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="QualityReportTaskSet")
			{
				return TranslateDownXMLForRADIO::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="SpectrumScanTaskSet")
			{
				return TranslateDownXMLForRADIO::TranslateSpectrumTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName =="FreqScanQuery")
			{
				return TranslateDownXMLForRADIO::TranslateSpectrumQueryTask(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "QualityQuery")
			{
				return TranslateDownXMLForRADIO::TranslateQualityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "ClientInfoQuery")
			{
				return TranslateDownXMLForRADIO::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "VideoClientStop")
			{
				return TranslateDownXMLForRADIO::TranslateStopClient(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RunPlanSet")                                            //运行图设置
			{
				return TranslateDownXMLForRADIO::TranslateRunPlanSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="TaskSchedulerQuery")                                            
			{
				return TranslateDownXMLForRADIO::TranslateTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ManualTaskSchedulerQuery")                                            
			{
				return TranslateDownXMLForRADIO::TranslateManualTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="EquipmentAlarmParamSet")
			{
				return TranslateDownXMLForRADIO::TranslateEquipmentAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName == "InfoCheckRequest")		//配置信息查询
			{
				return TranslateDownXMLForRADIO::TranslateInfoCheckRequest(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "RunPlanCheckRequest")		//运行图信息查询
			{
				return TranslateDownXMLForRADIO::TranslateRunPlanCheck(strOriginalXML, strStandardXML);
			}
			else if(TaskName == "TaskAndAlarmExpireRequest")
			{
				return TranslateDownXMLForRADIO::TranslateAlarmExpireTask(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,RADIO,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if (DVBType == "TVMonDown" &&(DstCode==PROPMANAGER::instance()->GetDefSrcCode(ATV)))//开路模拟电视类型任务
		{
			if (TaskName=="StreamRealtimeQuery")												//实时视频
			{
				return TranslateDownXMLForATV::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel"||TaskName=="TaskSet")											//录像设置
			{
				return TranslateDownXMLForATV::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamSimpleQuery"||"TaskRecordQuery"==TaskName || TaskName == "TaskRecordFileQuery")												//录像查询、下载
			{
				return TranslateDownXMLForATV::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumRealtimeScan")											//频谱扫描
			{
				return TranslateDownXMLForATV::TranslateSpectrumScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityRealtimeQuery")											//实时指标查询
			{
				return TranslateDownXMLForATV::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ChannelScanQuery")
			{
				return TranslateDownXMLForATV::TranslateChannelScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelSet")
			{
				return TranslateDownXMLForATV::TranslateChannelSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundQuery")
			{
				return TranslateDownXMLForATV::TranslateStreamRound(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityAlarmParamSet")
			{
				return TranslateDownXMLForATV::TranslateAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="QualityReportTaskSet")
			{
				return TranslateDownXMLForATV::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="SpectrumScanTaskSet")
			{
				return TranslateDownXMLForATV::TranslateSpectrumTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName =="FreqScanQuery")
			{
				return TranslateDownXMLForATV::TranslateSpectrumQueryTask(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "QualityQuery")
			{
				return TranslateDownXMLForATV::TranslateQualityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "ClientInfoQuery")
			{
				return TranslateDownXMLForATV::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "VideoClientStop")
			{
				return TranslateDownXMLForATV::TranslateStopClient(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RunPlanSet")                                            //运行图设置
			{
				return TranslateDownXMLForATV::TranslateRunPlanSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="TaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForATV::TranslateTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ManualTaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForATV::TranslateManualTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="EquipmentAlarmParamSet")
			{
				return TranslateDownXMLForATV::TranslateEquipmentAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName == "InfoCheckRequest")
			{
				return TranslateDownXMLForATV::TranslateInfoCheckRequest(strOriginalXML, strStandardXML);
			}
			else if (TaskName == "RunPlanCheckRequest")		//运行图信息查询
			{
				return TranslateDownXMLForATV::TranslateRunPlanCheck(strOriginalXML, strStandardXML);
			}
			else if(TaskName == "TaskAndAlarmExpireRequest")
			{
				return TranslateDownXMLForATV::TranslateAlarmExpireTask(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "ServerStatusSet")
			{
				return TranslateDownXMLForATV::TranslateServerStatusSet(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,ATV,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if (DVBType == "TVMonDown" &&(DstCode==PROPMANAGER::instance()->GetDefSrcCode(CTV)))//有线模拟电视类型任务
		{
			if (TaskName=="StreamRealtimeQuery")												//实时视频
			{
				return TranslateDownXMLForCTV::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel"||TaskName=="TaskSet")											//录像设置
			{
				return TranslateDownXMLForCTV::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamSimpleQuery"||"TaskRecordQuery"==TaskName || TaskName == "TaskRecordFileQuery")												//录像查询、下载
			{
				return TranslateDownXMLForCTV::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumRealtimeScan")											//频谱扫描
			{
				return TranslateDownXMLForCTV::TranslateSpectrumScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityRealtimeQuery")											//实时指标查询
			{
				return TranslateDownXMLForCTV::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML); 
			}
			else if (TaskName=="ChannelScanQuery")
			{
				return TranslateDownXMLForCTV::TranslateChannelScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelSet")
			{
				return TranslateDownXMLForCTV::TranslateChannelSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundQuery")
			{
				return TranslateDownXMLForCTV::TranslateStreamRound(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityAlarmParamSet")
			{
				return TranslateDownXMLForCTV::TranslateAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="QualityReportTaskSet")
			{
				return TranslateDownXMLForCTV::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="SpectrumScanTaskSet")
			{
				return TranslateDownXMLForCTV::TranslateSpectrumTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName =="SpectrumScanQuery")
			{
				return TranslateDownXMLForCTV::TranslateSpectrumQueryTask(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "QualityQuery")
			{
				return TranslateDownXMLForCTV::TranslateQualityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "ClientInfoQuery")
			{
				return TranslateDownXMLForCTV::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "VideoClientStop")
			{
				return TranslateDownXMLForCTV::TranslateStopClient(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RunPlanSet")                                            //运行图设置
			{
				return TranslateDownXMLForCTV::TranslateRunPlanSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="TaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForCTV::TranslateTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ManualTaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForCTV::TranslateManualTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,CTV,VS_MSG_SYSALARM);

				return false;
			}
		}
		else if (DVBType == "AMDown")//模拟广播类型任务
		{
			if (TaskName=="StreamRealtimeQuery")												//实时视频
			{
				return TranslateDownXMLForAM::TranslateStreamRealtimeQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SetAutoRecordChannel"||TaskName=="TaskSet")											//录像设置
			{
				return TranslateDownXMLForAM::TranslateRecordSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamSimpleQuery"||"TaskRecordQuery"==TaskName || TaskName == "TaskRecordFileQuery")												//录像查询、下载
			{
				return TranslateDownXMLForAM::TranslateRecordQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="SpectrumRealtimeScan")											//频谱扫描
			{
				return TranslateDownXMLForAM::TranslateSpectrumScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityRealtimeQuery")											//实时指标查询
			{
				return TranslateDownXMLForAM::TranslateQualityRealtimeQuery(strOriginalXML,strStandardXML); 
			}	
			else if (TaskName=="ChannelScanQuery")
			{
				return TranslateDownXMLForAM::TranslateChannelScanQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="StreamRoundQuery")
			{
				return TranslateDownXMLForAM::TranslateStreamRound(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ChannelSet")
			{
				return TranslateDownXMLForAM::TranslateChannelSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="QualityAlarmParamSet")
			{
				return TranslateDownXMLForAM::TranslateAlarmParamSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="QualityReportTaskSet")
			{
				return TranslateDownXMLForAM::TranslateQualityTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName=="SpectrumScanTaskSet")
			{
				return TranslateDownXMLForAM::TranslateSpectrumTaskSet(strOriginalXML,strStandardXML);
			}
			else if(TaskName =="FreqScanQuery")
			{
				return TranslateDownXMLForAM::TranslateSpectrumQueryTask(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "QualityQuery")
			{
				return TranslateDownXMLForAM::TranslateQualityQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "ClientInfoQuery")
			{
				return TranslateDownXMLForAM::TranslateClientInfoQuery(strOriginalXML,strStandardXML);
			}
			else if(TaskName == "VideoClientStop")
			{
				return TranslateDownXMLForAM::TranslateStopClient(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="RunPlanSet")                                            //运行图设置
			{
				return TranslateDownXMLForAM::TranslateRunPlanSet(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="TaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForAM::TranslateTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else if (TaskName=="ManualTaskSchedulerQuery")                                            //运行图设置
			{
				return TranslateDownXMLForAM::TranslateManualTaskSchedulerQuery(strOriginalXML,strStandardXML);
			}
			else
			{
				string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
				//SYSMSGSENDER::instance()->SendMsg(msg,AM,VS_MSG_SYSALARM);

				return false;
			}
		}
		else
		{
			string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			return false;
		}
	}
	catch(...)
	{
		string msg = "监测类型或者任务类型错误，无法生成标准XML:" + strOriginalXML;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

		return false;
	}
	string msg = "问题XML，无法生成标准XML:" + strOriginalXML;
	//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

	return false;
}

bool TranslateDownXML::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	string strType;
	XmlParser parser;
	parser.Set_xml(strOriginalXML);

	pXMLNODE rootnode=parser.GetRootNode();			//根节点msg
	parser.GetAttrNode(rootnode,"DVBType",strType);	//消息类型

	if ((strType=="DVBC")||(strType=="CTTB")||(strType=="DVBS")||(strType=="ATV")||(strType=="RADIO")||(strType=="AM")||(strType=="CTV"))
	{
		return TranslateDownXMLForDVBC::TranslateRecordTask(strOriginalXML,strStandardXMLVec);
	}
	return false;
}
//返回标准的偱切录像执行任务XML
 bool TranslateDownXML::TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	XmlParser tempparser( strOriginalXML.c_str() );
	pXMLNODE tempRootNode = tempparser.GetRootNode();
	std::string dvbtype;
	tempparser.GetAttrNode(tempRootNode, "DVBType", dvbtype);
	if (dvbtype =="DVBC")
	{
		return TranslateDownXMLForDVBC::TranslateRoundRecordTask(strOriginalXML, strStandardXMLVec);
	}
	else if(dvbtype =="DVBS")
	{
		return TranslateDownXMLForDVBS::TranslateRoundRecordTask(strOriginalXML, strStandardXMLVec);
	}

	return false;
}
bool TranslateDownXML::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	string strType;
	XmlParser parser;
	parser.Set_xml(strOriginalXML);

	pXMLNODE rootnode=parser.GetRootNode();			//根节点msg
	parser.GetAttrNode(rootnode,"DVBType",strType);	//消息类型

	if (strType=="CTTB")
	{
		return TranslateDownXMLForCTTB::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if (strType=="DVBC")
	{
		return TranslateDownXMLForDVBC::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if (strType=="DVBS")
	{
		return TranslateDownXMLForDVBS::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="ATV")
	{
		return TranslateDownXMLForATV::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="RADIO")
	{
		return TranslateDownXMLForRADIO::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="AM")
	{
		return TranslateDownXMLForAM::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="CTV")
	{
		return TranslateDownXMLForCTV::TranslateQualityTask(strOriginalXML,strStandardXMLVec);
	}
	return false;
}

bool TranslateDownXML::TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	string strType;
	XmlParser parser;
	parser.Set_xml(strOriginalXML);

	pXMLNODE rootnode=parser.GetRootNode();			//根节点msg
	parser.GetAttrNode(rootnode,"DVBType",strType);	//消息类型

	if(strType=="ATV")
	{
		return TranslateDownXMLForATV::TranslateSpectrumTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="RADIO")
	{
		return TranslateDownXMLForRADIO::TranslateSpectrumTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="AM")
	{
		return TranslateDownXMLForAM::TranslateSpectrumTask(strOriginalXML,strStandardXMLVec);
	}
	else if(strType=="CTV")
	{
		return TranslateDownXMLForCTV::TranslateSpectrumTask(strOriginalXML,strStandardXMLVec);
	}
	return false;

}