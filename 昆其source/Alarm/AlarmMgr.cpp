#include "AlarmMgr.h"
#include "AlarmSender.h"
#include "ReSendAlarm.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/AlarmPropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "../DeviceAccess/TCPDeviceAccess.h"

#include "CreateAlarmXMLForAM.h"
#include "CreateAlarmXMLForATV.h"
#include "CreateAlarmXMLForRadio.h"
#include "CreateAlarmXMLForDVBC.h"
#include "CreateAlarmXMLForCTTB.h"
#include "CreateAlarmXMLForDVBS.h"
#include "CreateAlarmXMLForCTV.h"

AlarmMgr::AlarmMgr(void)
{
	AlarmID="";
	bFlag=false;
	pAlarmSender = new AlarmSender();
	pAlarmReSender = new ReSendAlarm();
	std::vector<sCheckParam> vecCheckParam;

	SeperateTypeID StypeID;
	DBMANAGER::instance()->QueryAlarmPriority(vecAlarmPriority);
	DBMANAGER::instance()->QueryAlarmInfo("0",vecCheckParam);
	for (int i=0;i!=vecCheckParam.size();i++)
	{
		sCheckParam checkparam=vecCheckParam[i];
		std::string paramid;
		if ((checkparam.AlarmType==ALARM_FREQ)||(checkparam.AlarmType==ALARM_TR101_290))
		{
			StypeID.head = StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_");
			StypeID.type = checkparam.TypeID;

			paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.TypeID;

			vecTypeID.push_back(paramid);
			vecSeperateTypeID.push_back(StypeID);
		}
		else if (checkparam.AlarmType==ALARM_PROGRAM)
		{
			if((PROPMANAGER::instance()->GetCurWorkVersion() == "2.8")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
			{
				if(checkparam.TypeID == "30")
				{
					StypeID.head = StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_");
					StypeID.type = checkparam.TypeID;
					
					paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.ChannelID;//报警类型标识
					CheckAlarm(checkparam,false);	

					vecTypeID.push_back(paramid);
					vecSeperateTypeID.push_back(StypeID);
				}
			}
			else
			{
				string strmG = "";
				if(checkparam.TypeID=="58"||checkparam.TypeID=="59"||checkparam.TypeID=="60")
				{
					if(checkparam.TypeDesc.find("1")!=-1)
					{
						strmG = checkparam.TypeID + string("1");
					}
					else if(checkparam.TypeDesc.find("2")!=-1)
					{
						strmG = checkparam.TypeID + string("2");
					}
					else if(checkparam.TypeDesc.find("3")!=-1)
					{
						strmG = checkparam.TypeID + string("3");
					}
					else if(checkparam.TypeDesc.find("4")!=-1)
					{
						strmG = checkparam.TypeID + string("4");
					}
					else if(checkparam.TypeDesc.find("5")!=-1)
					{
						strmG = checkparam.TypeID + string("5");
					}
					else if(checkparam.TypeDesc.find("6")!=-1)
					{
						strmG = checkparam.TypeID + string("6");
					}
				}
				else
				{
					strmG = checkparam.TypeID;
				}
				//StrUtil::Int2Str(vecParam[0].DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].ChannelID+std::string("_")
				StypeID.head = StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.ChannelID;
				StypeID.type = strmG;
				/*if(checkparam.DVBType == RADIO || checkparam.DVBType == AM)
				{
					checkparam.ServiceID = checkparam.Freq;
					paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.ChannelID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.ServiceID+std::string("_")+checkparam.TypeID;
					CheckAlarm(checkparam,false);	
					vecTypeID.push_back(paramid);
				}*/
				if(checkparam.DVBType == CTTB || checkparam.DVBType == DVBC || checkparam.DVBType == DVBS)
				{
					paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.ChannelID+std::string("_")+strmG;//报警类型标识
					CheckAlarm(checkparam,false);
					vecTypeID.push_back(paramid);
				}		
				vecSeperateTypeID.push_back(StypeID);
			}
		}
		else if(checkparam.AlarmType==ALARM_EQUIPMENT)
		{
			std::string paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.TypeID;
			AddAlarm(paramid);
		}
	}
}

AlarmMgr::~AlarmMgr(void)
{
	//删除指针，释放内存
	if (pAlarmSender != NULL)
	{
		delete pAlarmSender;
		pAlarmSender = NULL;
	}	 
	if (pAlarmReSender != NULL)
	{
		delete pAlarmReSender;
		pAlarmReSender = NULL;
	}	
	
}
bool AlarmMgr::Start()
{
	if (pAlarmSender == NULL)
		return false;
	//启动报警发送线程
	pAlarmSender->open(NULL);
	if(pAlarmReSender!=NULL)
		pAlarmReSender->open(NULL);
	this->open(0);
	return true;
}

int AlarmMgr::open( void* )
{
	bFlag=true;
	activate();
	return 0;
}
int AlarmMgr::svc()
{
	OSFunction::Sleep(15);
	
	DWORD dwStarttime = 1;
	DWORD dwEndtime = 0;
	while(bFlag)
	{
		//检查ETR290报警
		//ETRMutex.acquire();		
		//if(ETRAlarm.size()>0)
		//{
		//	std::map<std::string,std::vector<sCheckParam> >::iterator ptr=ETRAlarm.begin();
		//	for (;ptr!=ETRAlarm.end();ptr++)
		//	{
		//		if((*ptr).second.size()==0)
		//			continue;
		//		
		//		std::vector<sCheckParam> vecCheckParam=(*ptr).second;
		//		sCheckParam param=vecCheckParam[0];
		//		string alarmxml;
		//		int IsETRAlarm=CheckETRAlarm(time(0),vecCheckParam);

		//		switch(IsETRAlarm)		//检查是否报警
		//		{
		//		case 0:
		//			{
		//				long alarmid=0;
		//				PROPMANAGER::instance()->GetAlarmID(alarmid);
		//				param.AlarmID=StrUtil::Long2Str(alarmid);
		//				param.CheckTime=time(0);
		//				DBMANAGER::instance()->AddAlarmInfo(param,"0");
		//				CreateAlarmXML(param,"0",StrUtil::Long2Str(alarmid),alarmxml);
		//				break;
		//			}
		//		case 1:
		//			{	
		//				param.CheckTime=time(0);
		//				DBMANAGER::instance()->QueryFreqAlarmID(param.DVBType,param.DeviceID,param.Freq,param.TypeID,param.AlarmID);
		//				DBMANAGER::instance()->UpdateAlarmInfo(param.DVBType,param.AlarmID,"1");
		//				if(param.AlarmID.size()>0)
		//				{
		//					CreateAlarmXML(param,"1",param.AlarmID,alarmxml);
		//				}
		//				break;
		//			}
		//		default:
		//			break;
		//		}
		//		if(alarmxml.length()>0)
		//		{
		//			ACE_Message_Block *MBALARM = new ACE_Message_Block(alarmxml.length());
		//			memcpy(MBALARM->wr_ptr(),alarmxml.c_str(),alarmxml.length());
		//			MBALARM->wr_ptr(alarmxml.length());
		//			pAlarmSender->putq(MBALARM);//将报警xml添加到上报队列中
		//			//SYSMSGSENDER::instance()->SendMsg(alarmxml,param.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		//		}
		//	}
		//}
		//ETRMutex.release();
		//检查节目报警
		ProgramMutex.acquire();
		if(ProgramAlarm.size()>0)
		{
			
			std::map<std::string,std::vector<sCheckParam> >::iterator programptr=ProgramAlarm.begin();
			for (;programptr!=ProgramAlarm.end();programptr++)
			{
				if((*programptr).second.size()==0)
					continue;
			
				sCheckParam param=(*programptr).second[0];
				string alarmxml;
				time_t curtime=time(0);
				int IsProgramAlarm = -1;
				if((PROPMANAGER::instance()->GetCurWorkVersion() == "2.8")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
				{
					param.StartTime = "";
					param.EndTime = "";
					IsProgramAlarm = CheckProgramAlarmV3(time(0),(*programptr).second,param,curtime);
				}
				else
				{
					IsProgramAlarm = CheckProgramAlarm(time(0),(*programptr).second,curtime);
					if(IsProgramAlarm == 1)
					{
						param.CheckTime=curtime - 4;
					}
				}
				long alarmid=0;
				
				if(param.TypeID.find("58")!=-1)
				{
					param.TypeID = "58";
				}
				else if(param.TypeID.find("59")!=-1)
				{
					param.TypeID = "59";
				}
				else if(param.TypeID.find("60")!=-1)
				{
					param.TypeID = "60";
				}
				//获取异态持续时间参数，用于将实际时间前移
				sAlarmParam alarmparam;
				bool ret = GetAlarmParm(param,alarmparam);
				if (ret == false)
				{
					alarmparam.Duration = "10";
				}
				int duration=StrUtil::Str2Int(alarmparam.Duration);//异态持续时间
				string strtime = TimeUtil::DateTimeToStr(param.CheckTime);
				if(ret && alarmparam.Switch=="1")
				{
					switch(IsProgramAlarm)	//检查是否报警
					{
					case 0:		//报警
						PROPMANAGER::instance()->GetAlarmID(alarmid);
						param.AlarmID=StrUtil::Long2Str(alarmid);
						DBMANAGER::instance()->AddAlarmInfo(param,"0");
						CreateAlarmXML(param,"0",StrUtil::Long2Str(alarmid),alarmxml);
						break;
					case 1:		//解报 
						
						DBMANAGER::instance()->QueryFreqAlarmID(param.DVBType,param.DeviceID,param.Freq,param.TypeID,param.AlarmID);
						//bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode);
						DBMANAGER::instance()->UpdateAlarmInfo(param.DVBType,param.AlarmID,strtime,"1");
						DBMANAGER::instance()->UpdateAlarmRecordExpireTime(param.DVBType,param.AlarmID);						//ALARM_PROGRAM
						CreateAlarmXML(param,"1",param.AlarmID,alarmxml);
						break;
					default:
						break;
					}
				}
				if(alarmxml.length()>0)
				{
					ACE_Message_Block *MBALARM = new ACE_Message_Block(alarmxml.length());
					memcpy(MBALARM->wr_ptr(),alarmxml.c_str(),alarmxml.length());
					MBALARM->wr_ptr(alarmxml.length());
					pAlarmSender->putq(MBALARM);//将报警xml添加到上报队列中
				}
			}
		}
		ProgramMutex.release();
		OSFunction::Sleep(0,100);
	}
	return 0;
}
int AlarmMgr::Stop()
{
	bFlag=false;
	if (pAlarmSender == NULL)
		return false;
	//停止报警发送线程
	pAlarmSender->Stop();

	if(pAlarmReSender!=NULL)
		pAlarmReSender->Stop();
	this->wait();
	return 0;
}
bool AlarmMgr::CheckAlarm(sCheckParam& checkparam,bool IsConvert)
{
	bool ret = false;//初始化返回值
	sAlarmParam alarmparam;
	std::string alarmxml;
	std::string tmpTypeID;
	std::string tempswith="";

	//映射报警类型ID
	if(IsConvert)
	{
		switch(checkparam.AlarmType)//根据报警类型检测是否报警，并生成上报的xml
		{
		case ALARM_ENVIRONMENT:
			{
				StrUtil::ConvertEnvTypeIDToAlarmID(checkparam);//环境报警
				break;
			}
		case ALARM_FREQ:
			{
				StrUtil::ConvertQualityTypeIDToAlarmID(checkparam);//频点报警
				break;
			}
		case ALARM_PROGRAM:
			{
				StrUtil::ConvertProgramTypeIDToAlarmID(checkparam); //节目报警
				break;
			}
		case ALARM_TR101_290:
			{
				StrUtil::ConvertETR290TypeIDToAlarmID(checkparam); //290报警
				break;
			}
		default:
			break;
		}
	}

	if(checkparam.AlarmType == ALARM_FREQ || checkparam.AlarmType == ALARM_PROGRAM || checkparam.AlarmType == ALARM_TR101_290)
	{
		//获取报警参数
		ret = GetAlarmParm(checkparam,alarmparam);
		if (ret==false)
		{
			if(checkparam.AlarmType == ALARM_PROGRAM)
			{
				//cout<<"获取报警参数失败-AlarmType:"<<checkparam.AlarmType<<" TypeId:"<<checkparam.TypeID<<" Freq:"<<checkparam.Freq<<" ChannelId:"<<checkparam.ChannelID<<" DeviceId:"<<checkparam.DeviceID<<" Typedesc:"<<checkparam.TypeDesc<<endl;
			}
			else
			{
				//cout<<"获取报警参数失败-AlarmType:"<<checkparam.AlarmType<<" TypeId:"<<checkparam.TypeID<<" Freq:"<<checkparam.Freq<<" DeviceId:"<<checkparam.DeviceID<<" Typedesc:"<<checkparam.TypeDesc<<endl;
			}
			return false;
		}
	}

	switch(checkparam.AlarmType)//根据报警类型检测是否报警，并生成上报的xml
	{
	case ALARM_ENVIRONMENT:
		{
			ret = CheckEnvironmenAlarmParam(checkparam,alarmparam,alarmxml);	//环境报警
			break;
		}
	case ALARM_EQUIPMENT:
		{
			ret = CheckequipmenAlarmParam(checkparam,alarmxml);	//设备报警
			break;
		}
	case ALARM_FREQ:
		{
			ret = CheckFreqAlarmParam(checkparam,alarmparam,alarmxml);			//频点报警
			break;
		}
	case ALARM_PROGRAM:
		{
			ProgramMutex.acquire();
			//string OrderNumAuo = "";
			string strmG = "";
			if(checkparam.TypeID=="58"||checkparam.TypeID=="59"||checkparam.TypeID=="60")
			{
				if(checkparam.TypeDesc.find("1")!=-1)
				{
					strmG = checkparam.TypeID + string("1");
				}
				else if(checkparam.TypeDesc.find("2")!=-1)
				{
					strmG = checkparam.TypeID + string("2");
				}
				else if(checkparam.TypeDesc.find("3")!=-1)
				{
					strmG = checkparam.TypeID + string("3");
				}
				else if(checkparam.TypeDesc.find("4")!=-1)
				{
					strmG = checkparam.TypeID + string("4");
				}
				else if(checkparam.TypeDesc.find("5")!=-1)
				{
					strmG = checkparam.TypeID + string("5");
				}
				else if(checkparam.TypeDesc.find("6")!=-1)
				{
					strmG = checkparam.TypeID + string("6");
				}
			}
			else
			{
				strmG = checkparam.TypeID;
			}
			std::string key=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.ChannelID+std::string("_")+strmG;
			if((PROPMANAGER::instance()->GetCurWorkVersion() == "2.8")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
			{
				key=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.ChannelID;
			}
			std::map<std::string,std::vector<sCheckParam> >::iterator ptr=ProgramAlarm.find(key);
			if(ptr==ProgramAlarm.end())
			{
				vector<sCheckParam> vecParam;
				checkparam.TypeID = strmG;
				vecParam.push_back(checkparam);
				ProgramAlarm.insert(make_pair(key,vecParam));
			}
			else
			{
				checkparam.TypeID = strmG;
				ptr->second.push_back(checkparam);
			}
			ProgramMutex.release();
			break;
		}
	case ALARM_TR101_290:
		{
			ETRMutex.acquire();
			std::string key=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.TypeID;
			std::map<std::string,std::vector<sCheckParam> >::iterator ptr=ETRAlarm.find(key);
			if(ptr==ETRAlarm.end())
			{
				vector<sCheckParam> vecParam;
				vecParam.push_back(checkparam);
				ETRAlarm.insert(make_pair(key,vecParam));
			}
			else
			{
				(*ptr).second.push_back(checkparam);
			}
			ETRMutex.release();
			break;
		}
	default:
		break;
	}

	if(ret==true && alarmxml.length()>0)
	{
		ACE_Message_Block *MBALARM = new ACE_Message_Block(alarmxml.length());
		memcpy(MBALARM->wr_ptr(),alarmxml.c_str(),alarmxml.length());
		MBALARM->wr_ptr(alarmxml.length());
		ret = pAlarmSender->putq(MBALARM);//将报警xml添加到上报队列中

		//SYSMSGSENDER::instance()->SendMsg(alarmxml,checkparam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
	}
	return ret;
}

bool AlarmMgr::CheckEnvironmenAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam,std::string& alarmxml)
{
	int check = atoi( checkparam.TypedValue.c_str());//测量值
	int downThreshold = atoi( alarmparam.DownThreshold.c_str());//下限值
	int upThreshold = atoi( alarmparam.UpThreshold.c_str() );//上限值

	if(alarmparam.Switch=="0")
		return true;

	if(check>=downThreshold&&check<=upThreshold&&IsAlarmed(StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID))//正常值已报过警
	{
		std::string typeID=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID;
		ReleaseAlarm(typeID);//解除报警

		std::string msg="解除报警,TypeID="+checkparam.TypeID;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		//CreateAlarmXML(checkparam,"1","测量值正常",alarmxml);//构造解除报警xml
		return true;
	}
	if (check<downThreshold&&!IsAlarmed(StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID))//不正常值未报警
	{
		string reason="小于下限值";
		std::string typeID=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID;
		AddAlarm(typeID);//添加报警

		std::string msg="添加报警,TypeID="+checkparam.TypeID;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		//CreateAlarmXML(checkparam,"0",reason,alarmxml);//构造报警xml
		return true;
	}
	if (check>upThreshold&&!IsAlarmed(StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID))//不正常值未报警
	{
		string reason="大于上限值";
		std::string typeID=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.TypeID;
		AddAlarm(typeID);//添加报警

		std::string msg="添加报警,TypeID="+checkparam.TypeID;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		//CreateAlarmXML(checkparam,"0",reason,alarmxml);//构造报警xml
		return true;
	}
	return true;
}
bool AlarmMgr::CheckequipmenAlarmParam(const sCheckParam& checkparam,std::string& alarmxml)
{
	std::string paramid;
	if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Cardindex+std::string("_")+checkparam.Modindex+std::string("_")+checkparam.TypeID;
	}
	else
	{
		paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.TypeID;
	}
	if(checkparam.mode == "1")
	{
		if(IsAlarmed(paramid))//正常已报过警
		{
			std::cout<<"故障报警解报"<<std::endl;
			ReleaseAlarm(paramid);//解除报警
			sCheckParam tmpParam=checkparam;
			DBMANAGER::instance()->QueryFreqAlarmID(checkparam.DVBType,checkparam.DeviceID,checkparam.Freq,checkparam.TypeID,tmpParam.AlarmID);
			
			tmpParam.CheckTime=time(0);

			DBMANAGER::instance()->UpdateAlarmInfo(tmpParam.DVBType,tmpParam.AlarmID,"1");
			CreateAlarmXML(tmpParam,"1",tmpParam.AlarmID,alarmxml);//构造解除报警xml
			
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_WARNNING,alarmxml,LOG_OUTPUT_BOTH);
				
			string strcmd = string("---------- [") + checkparam.DeviceID + string("] 号 解调解扰设备 报警解除");
			
			APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_WARNNING,strcmd,LOG_OUTPUT_BOTH);
			

			//SYSMSGSENDER::instance()->SendMsg(alarmxml,tmpParam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		
			if(tmpParam.AlarmType == ALARM_EQUIPMENT)
			{
				string strRdcmd = string("-------- [") + checkparam.DeviceID + string("] 号设备异常恢复，重启主程序..."); 
				APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_WARNNING,strRdcmd,LOG_OUTPUT_BOTH);
				
				//退出主程序
				//_exit(0);
			}
			return true;
		}
	}
	else if (checkparam.mode == "0")
	{
		if (!IsAlarmed(paramid))//不正常未报警
		{
			std::cout<<"故障报警"<<std::endl;
			AddAlarm(paramid);//添加报警
			long alarmid=0;
			PROPMANAGER::instance()->GetAlarmID(alarmid);

			sCheckParam tmpParam=checkparam;
			tmpParam.AlarmID=StrUtil::Long2Str(alarmid);
			tmpParam.CheckTime=time(0);
			
			DBMANAGER::instance()->AddAlarmInfo(tmpParam,"0");
			CreateAlarmXML(tmpParam,"0",StrUtil::Long2Str(alarmid),alarmxml);//构造报警xml
			
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_WARNNING,alarmxml,LOG_OUTPUT_BOTH);
			
			//SYSMSGSENDER::instance()->SendMsg(alarmxml,tmpParam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
			return true;
		}
	}
	return true;
}
bool AlarmMgr::CheckFreqAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam,std::string& alarmxml)
{
	float check = atof(checkparam.TypedValue.c_str());//测量值
	float downThreshold = atof( alarmparam.DownThreshold.c_str());//下限值
	float upThreshold = atof( alarmparam.UpThreshold.c_str() );//上限值

	//std::cout<<check<<" ---- "<<downThreshold<<" ---- "<<upThreshold<<std::endl;

	if(alarmparam.Switch=="0")
		return true;

	std::string paramid=StrUtil::Int2Str(checkparam.DVBType)+std::string("_")+checkparam.DeviceID+std::string("_")+checkparam.Freq+std::string("_")+checkparam.TypeID;
	if(check>=downThreshold&&check<=upThreshold&&IsAlarmed(paramid))//正常值已报过警
	{
		std::cout<<"解报"<<std::endl;
		ReleaseAlarm(paramid);//解除报警
		sCheckParam tmpParam=checkparam;
		DBMANAGER::instance()->QueryFreqAlarmID(checkparam.DVBType,checkparam.DeviceID,checkparam.Freq,checkparam.TypeID,tmpParam.AlarmID);
		
		tmpParam.CheckTime=time(0);

		DBMANAGER::instance()->UpdateAlarmInfo(tmpParam.DVBType,tmpParam.AlarmID,"1");
		CreateAlarmXML(tmpParam,"1",tmpParam.AlarmID,alarmxml);//构造解除报警xml

		//SYSMSGSENDER::instance()->SendMsg(alarmxml,tmpParam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		return true;
	}

	if (check<downThreshold&&!IsAlarmed(paramid))//不正常值未报警
	{
		std::cout<<"小于下限值"<<std::endl;
		string reason="小于下限值";
		AddAlarm(paramid);//添加报警
		long alarmid=0;
		PROPMANAGER::instance()->GetAlarmID(alarmid);

		sCheckParam tmpParam=checkparam;
		tmpParam.AlarmID=StrUtil::Long2Str(alarmid);
		tmpParam.CheckTime=time(0);
		
		DBMANAGER::instance()->AddAlarmInfo(tmpParam,"0");
		CreateAlarmXML(tmpParam,"0",StrUtil::Long2Str(alarmid),alarmxml);//构造报警xml

		//SYSMSGSENDER::instance()->SendMsg(alarmxml,tmpParam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		return true;
	}
	if (check>upThreshold&&!IsAlarmed(paramid))//不正常值未报警
	{
		std::cout<<"大于上限值"<<std::endl;
		string reason="大于上限值";
		AddAlarm(paramid);//添加报警
		long alarmid=0;
		PROPMANAGER::instance()->GetAlarmID(alarmid);
		sCheckParam tmpParam=checkparam;
		tmpParam.AlarmID=StrUtil::Long2Str(alarmid);
		tmpParam.CheckTime=time(0);
		DBMANAGER::instance()->AddAlarmInfo(tmpParam,"0");
		CreateAlarmXML(tmpParam,"0",StrUtil::Long2Str(alarmid),alarmxml);//构造报警xml

		//SYSMSGSENDER::instance()->SendMsg(alarmxml,tmpParam.DVBType,VS_MSG_PROALARM,DATAANALYSER,LOG_EVENT_DEBUG,false,true);
		return true;
	}
	return true;
}
int AlarmMgr::CheckETRAlarm( time_t curTime,vector<sCheckParam>& vecParam )//290报警逻辑
{
	if (vecParam.size()==0)
		return -1;

	//获取报警参数
	sAlarmParam param;
	bool bgetAlarmparam = GetAlarmParm(vecParam[0],param);
	if((param.Switch == "0")||(!bgetAlarmparam))
	{
		vecParam.clear();
		return -1;
	}

	int timeinterval=StrUtil::Str2Int(param.TimeInterval);//报警时间间隔
	int errornum=StrUtil::Str2Int(param.Num);//错误次数
	std::string typeID=StrUtil::Int2Str(param.DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].Freq+std::string("_")+param.TypeID;//报警类型标识

	int num=0;//初始化错误次数
	int i=vecParam.size()-1;
	bool bFind=false;//标识是否找到与当前时间间隔大于等于报警时间
	for(;i>=0;--i)
	{
		if(curTime-vecParam[i].CheckTime<=timeinterval)
			num++;//=StrUtil::Str2Int(vecParam[i].TypedValue);//累加错误
		else
		{
			bFind=true;//找到删除点
			break;
		}
	}
	if(bFind)
	{
		if(vecParam.size()>0)
		{
			for(int j=0;j<=i;j++)//删除时间点以前的数据
			{
				std::vector<sCheckParam>::iterator ptr=vecParam.begin();
				vecParam.erase(ptr);
			}
		}
	}
	if(num>=errornum&&!IsAlarmed(typeID))//实际错误次数大于报警次数且没报过警
	{
		AddAlarm(typeID);	//添加报警
		return 0;			//需要报警
	}
	if(num<errornum&&bFind&&IsAlarmed(typeID))//没有达到报警次数但找到时间点
	{
		ReleaseAlarm(typeID);//解除报警
		return 1;
	}
	return -1;
}
int AlarmMgr::CheckProgramAlarmV3( time_t curTime,vector<sCheckParam>& vecParam,sCheckParam& ResultAlarm,time_t& alarmtime )
{
	if (vecParam.size()<=0)
	{
		return -1;
	}
	std::string typeID=StrUtil::Int2Str(vecParam[0].DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].ChannelID+std::string("_")+vecParam[0].Freq+std::string("_")+vecParam[0].ServiceID+std::string("_30");//报警类型标识
	time_t lastalarmtime = time(0);
	sAlarmParam param;
	sCheckParam tempparam=vecParam[0];
	ResultAlarm = vecParam[0];
	ResultAlarm.TypeID = "30";
	ResultAlarm.TypeDesc = "内容异态";
	bool bgetAlarmparam = GetAlarmParm(ResultAlarm,param);
	if((param.Switch == "0")||(!bgetAlarmparam))
	{
		vecParam.clear();
		return -1;
	}
	int duration=StrUtil::Str2Int(param.Duration);	//异态持续时间
	if(duration <= 3)//报警最小时间3秒
	{
		duration = 3;
	}
	int ialarmparam = 0;
	time_t Unalarmtime = 0;;
	time_t Alarmtime = 0;
	int index = 0;
	int i = 0;
	for (vector<sCheckParam>::iterator ptr = vecParam.begin();ptr!=vecParam.end();)
	{
		if (ptr->mode == "0")
		{
			if(Alarmtime == 0)
			{
				Alarmtime = ptr->CheckTime;
				index = i;
			}
			ialarmparam++;
		}
		else if(ptr->mode == "1")
		{
			Unalarmtime = ptr->CheckTime;
			ialarmparam--;
			if(ialarmparam == 0)
			{
				Alarmtime = 0;
			}
		}
		ptr++;
		i++;
	}
	//运行图内的报警信息清空，不做处理或解报警
	if(vecParam.size()>0 && RUNPLANINFOMGR::instance()->InRunPlan(vecParam[index].DVBType,vecParam[index].ChannelID)==false)
	{
		if(IsAlarmed(typeID))
		{
			alarmtime=time(0)+duration;
			ReleaseAlarm(typeID);
			return 1;
		}
		//
		vecParam.clear();
		return -1;
	}
	//
	if(ialarmparam>0)
	{
		if((!IsAlarmed(typeID))&&(time(0)-vecParam[index].CheckTime >= duration)&&(vecParam[index].mode == "0"))
		{
			ResultAlarm.CheckTime = vecParam[index].CheckTime;
			AddAlarm(typeID);
			return 0;
		}
	}
	else if(ialarmparam<=0)
	{
		if(IsAlarmed(typeID))
		{
			//
			time_t alarmtime[3] = {0,0,0};//typeid31 32 33
			time_t unalarmtime[3] = {0,0,0};//typeid31 32 33
			//
			for (vector<sCheckParam>::iterator ptr = vecParam.begin();ptr!=vecParam.end();)
			{
				if(ptr->mode == "0")
				{
					bool valid = false;
					if(ptr->TypeID == "31")
					{
						if(ptr->CheckTime-unalarmtime[0] > 3)
						{
							valid = true;
						}
					}
					else if(ptr->TypeID == "32")
					{
						if(ptr->CheckTime-unalarmtime[1] > 3)
						{
							valid = true;
						}
					}
					else if(ptr->TypeID == "33")
					{
						if(ptr->CheckTime-unalarmtime[2] > 3)
						{
							valid = true;
						}
					}
					if(valid)
					{
						long alarmid = 0;
						PROPMANAGER::instance()->GetAlarmID(alarmid);
						ptr->AlarmID=StrUtil::Long2Str(alarmid);
						DBMANAGER::instance()->AddAlarmInfo(*ptr,"0");
						if(ResultAlarm.StartTime.size()<=0)
						{
							ResultAlarm.StartTime = TimeUtil::DateTimeToStr(ptr->CheckTime);
						}
						if(ptr->TypeID == "31")
						{
							alarmtime[0] = ptr->CheckTime;
						}
						else if(ptr->TypeID == "32")
						{
							alarmtime[1] = ptr->CheckTime;
						}
						else if(ptr->TypeID == "33")
						{
							alarmtime[2] = ptr->CheckTime;
						}
					}
				}
				else if(ptr->mode == "1")
				{
					bool bdel = false;
					if(ptr->TypeID == "31")
					{
						if(ptr->CheckTime-alarmtime[0] < 2)
						{
							bdel = true;
						}
					}
					else if(ptr->TypeID == "32")
					{
						if(ptr->CheckTime-alarmtime[1] < 2)
						{
							bdel = true;
						}
					}
					else if(ptr->TypeID == "33")
					{
						if(ptr->CheckTime-alarmtime[2] < 2)
						{
							bdel = true;
						}
					}
					if(bdel)
					{
						DBMANAGER::instance()->QueryFreqAlarmID(ptr->DVBType,ptr->DeviceID,ptr->Freq,ptr->TypeID,ptr->AlarmID);
						DBMANAGER::instance()->DeleteAlarmfromAlarmid(ptr->AlarmID);
					}
					else
					{
						DBMANAGER::instance()->QueryFreqAlarmID(ptr->DVBType,ptr->DeviceID,ptr->Freq,ptr->TypeID,ptr->AlarmID);
						DBMANAGER::instance()->UpdateAlarmInfo(ptr->DVBType,ptr->AlarmID,TimeUtil::DateTimeToStr(ptr->CheckTime-1),"1");
						DBMANAGER::instance()->UpdateAlarmRecordExpireTime(ptr->DVBType,ptr->AlarmID);
						if(ptr->TypeID == "31")
						{
							unalarmtime[0] = ptr->CheckTime;
						}
						else if(ptr->TypeID == "32")
						{
							unalarmtime[1] = ptr->CheckTime;
						}
						else if(ptr->TypeID == "33")
						{
							unalarmtime[2] = ptr->CheckTime;
						}
					}
				}
				ptr++;
			}
			//
			vecParam.clear();
			ResultAlarm.CheckTime = Unalarmtime-1;
			ResultAlarm.EndTime = TimeUtil::DateTimeToStr(Unalarmtime-1);
			ReleaseAlarm(typeID);
			return 1;
		}
		vecParam.clear();
	}
	return -1;

}
int AlarmMgr::CheckProgramAlarm( time_t curTime,vector<sCheckParam>& vecParam,time_t& alarmtime )
{
	if (vecParam.size()==0)
	{
		return -1;
	}

	//获取报警参数
	sAlarmParam param;
	sCheckParam tempparam=vecParam[0];
	GetAlarmParm(vecParam[0],param);
	enumDVBType tempDvbType = vecParam[0].DVBType;
	std::string tempChanID = vecParam[0].ChannelID;
	//
	int duration=StrUtil::Str2Int(param.Duration);	//异态持续时间
	//
	UpAlarmInfo info;
	PROPMANAGER::instance()->GetUpAlarmInfo(vecParam[0].DVBType,info);
	if(vecParam[0].DVBType == RADIO || vecParam[0].DVBType == AM)
	{
		vecParam[0].ServiceID = vecParam[0].Freq;
	}
	std::string typeID=StrUtil::Int2Str(vecParam[0].DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].ChannelID+std::string("_")+vecParam[0].Freq+std::string("_")+vecParam[0].ServiceID+std::string("_")+vecParam[0].TypeID;//报警类型标识
	enumDVBType eDvbtypeForSpec = vecParam[0].DVBType;
	std::string strTypeIdForSpec = vecParam[0].TypeID;
	//
	SeperateTypeID StypeID;
	StypeID.head = StrUtil::Int2Str(vecParam[0].DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].ChannelID+std::string("_")+vecParam[0].Freq+std::string("_")+vecParam[0].ServiceID+std::string("_");
	StypeID.type = vecParam[0].TypeID;;
	//
	if(vecParam[0].DVBType == CTTB || vecParam[0].DVBType == DVBC || vecParam[0].DVBType == DVBS)
	{
		typeID=StrUtil::Int2Str(vecParam[0].DVBType)+std::string("_")+vecParam[0].DeviceID+std::string("_")+vecParam[0].ChannelID+std::string("_")+ vecParam[0].TypeID;
	}	
	time_t lastalarmtime = time(0);
	string strmode = "1";
	//

	if(vecParam.size()>0)
	{
		for (vector<sCheckParam>::iterator ptr = vecParam.begin();ptr!=vecParam.end();)
		{
			lastalarmtime = ptr->CheckTime;
			strmode = ptr->mode;
			if(vecParam.size() > 1)
			{
				if ((curTime-lastalarmtime) > duration)
				{
					ptr = vecParam.erase(ptr);
					continue;
				}
			}
			ptr++;
		}
	}
	//
	if(info.onceday=="1")
	{
		string strtime=TimeUtil::GetCurTime();
		string daytime=TimeUtil::GetCurDate();
		long nowTime=TimeUtil::StrToSecondTime(strtime);
		time_t thistime=time(0);
		long daysecond=3600*24;
		if((daysecond-nowTime)<=2)
		{	
			if(IsAlarmed(typeID))
			{
				string strnowtime=daytime+" "+"23:59:59";
				alarmtime=TimeUtil::StrToDateTime(strnowtime)+duration;
				ReleaseAlarm(typeID);
				return 1;
			}
		}	
		else if(nowTime<=2)
		{
			if(IsAlarmed(typeID))
			{
				alarmtime=thistime-nowTime-1+duration;
				ReleaseAlarm(typeID);
				return 1;
			}
		}
	}  
	//运行图内的报警信息清空，不做处理或解报警
	if(vecParam.size()>0 && RUNPLANINFOMGR::instance()->InRunPlan(vecParam[0].DVBType,vecParam[0].ChannelID)==false)
	{
		if(IsAlarmed(typeID))
		{
			alarmtime=time(0)+duration;
			ReleaseAlarm(typeID);
			return 1;
		}
		//
		vecParam.clear();
		return -1;
	}        
	//报警数据大于报警间隔，而且该类型的报警ID不存在
	time_t timeNow = time(0);
	if (((timeNow-lastalarmtime) > duration) && (strmode == "0") && (!IsAlarmed(typeID)))
	{
		
		alarmtime=time(0)-duration;
		//判断报警时间是否在运行图时间内
		RUNPLANINFOMGR::instance()->CheckAlarmTime(tempDvbType,tempChanID,alarmtime);		
		alarmtime += duration;

		string AlarmType=GetAlarmTypeID(StypeID.head);
		if(AlarmType!="")
		{
			string lastpriority=GetAlarmPriority(tempDvbType,AlarmType);
			string curpriority =GetAlarmPriority(tempDvbType,StypeID.type);
			//当前报警优先级低则直接返回 
			if(StrUtil::Str2Int(lastpriority)>StrUtil::Str2Int(curpriority))
			{
				return -1;  
			}
			//当前报警优先级较高，先解除以前的报警
			else if(StrUtil::Str2Int(lastpriority)<StrUtil::Str2Int(curpriority))
			{
				string alarmxml;
				string temptypeid=StypeID.head+AlarmType;
				//先解除以前的报警
				ReleaseAlarm(temptypeid);

				tempparam.CheckTime=time(0);
				//查询报警ID信息表，更新数据库，更新异态录像过期时间保存数据库过期标志
				DBMANAGER::instance()->QueryFreqAlarmID(tempparam.DVBType,tempparam.DeviceID,tempparam.Freq,AlarmType,tempparam.AlarmID);
				DBMANAGER::instance()->UpdateAlarmInfo(tempparam.DVBType,tempparam.AlarmID,"1");
				DBMANAGER::instance()->UpdateAlarmRecordExpireTime(tempparam.DVBType,tempparam.AlarmID);
				CreateAlarmXML(tempparam,"1",tempparam.AlarmID,alarmxml);

				if(alarmxml.length()>0)
				{
					ACE_Message_Block *MBALARM = new ACE_Message_Block(alarmxml.length());
					memcpy(MBALARM->wr_ptr(),alarmxml.c_str(),alarmxml.length());
					MBALARM->wr_ptr(alarmxml.length());
					pAlarmSender->putq(MBALARM);//将报警xml添加到上报队列中
				}
				AddAlarm(typeID);
				return 0;
			}
		}
		AddAlarm(typeID);
		return 0;
	}
	else if((strmode == "1") && (3<=(timeNow-lastalarmtime)) && IsAlarmed(typeID))
	{//解报
		ReleaseAlarm(typeID);
		return 1;
	}
	return -1;
}
bool AlarmMgr::IsAlarmed( std::string typeId )
{
	bool ret=false;
	ACE_Guard<ACE_Thread_Mutex> guard(MutexTypeID);
	if(vecTypeID.size()>0)
	{
		std::vector<string>::iterator ptr=vecTypeID.begin();
		for (;ptr!=vecTypeID.end();ptr++)//查找typeid是否存在内存中
		{
			if(typeId==(*ptr))
			{
				ret=true;
				break;
			}
		}
	}
	return ret;
}

bool AlarmMgr::ReleaseAlarm( std::string typeId )
{
	ACE_Guard<ACE_Thread_Mutex> guard(MutexTypeID);
	if(vecTypeID.size())
	{
		std::vector<string>::iterator ptr=vecTypeID.begin();
		for (;ptr!=vecTypeID.end();ptr++)//找到值为typeid项，删除报警
		{
			if(typeId==(*ptr))
			{
				vecTypeID.erase(ptr);
				break;
			}
		}
	}
	if(vecSeperateTypeID.size())
	{
		std::vector<SeperateTypeID>::iterator itr= vecSeperateTypeID.begin();
		for(;itr!=vecSeperateTypeID.end();itr++)
		{
			if((*itr).head+(*itr).type==typeId)
			{
				vecSeperateTypeID.erase(itr);
				break;
			}
		}
	}
	return true;
}

bool AlarmMgr::AddAlarm( std::string typeId )
{
	ACE_Guard<ACE_Thread_Mutex> guard(MutexTypeID);
	vecTypeID.push_back(typeId);//添加报警
	size_t pos= typeId.find_last_of("_");
	SeperateTypeID temp;
	temp.type = typeId.substr(pos+1);
	temp.head = typeId.substr(0,pos+1);
	vecSeperateTypeID.push_back(temp);
	return true;
}
bool AlarmMgr::AddAlarmID( std::string type,int alarmid )
{
	MutexAlarmID.acquire();
	map<string,int>::iterator ptr=mapAlarmID.find(type);
	if (ptr!=mapAlarmID.end())
	{
		ptr->second=alarmid;
	}
	else
	{
		mapAlarmID.insert(make_pair(type,alarmid));
	}
	MutexAlarmID.release();
	return true;
}

bool AlarmMgr::GetAlarmID( std::string type,int& alarmid )
{
	MutexAlarmID.acquire();
	map<string,int>::iterator ptr=mapAlarmID.find(type);
	if (ptr!=mapAlarmID.end())
	{
		alarmid=ptr->second;
	}
	MutexAlarmID.release();
	return true;

}

bool AlarmMgr::CreateAlarmXML( const sCheckParam& checkparam,std::string mode,std::string alarmid,std::string& alarmxml )
{
	switch(checkparam.AlarmType)//根据监测类型生成不同的节点，并添加节点属性
	{
	case ALARM_ENVIRONMENT://环境报警
		{
			alarmxml = CreateEnvironmentAlarmXML(checkparam,mode);
			break;
		}
	case ALARM_EQUIPMENT://环境报警
		{
			alarmxml = CreateEquipmentAlarmXML(checkparam,mode);
			break;
		}
	case ALARM_PROGRAM://节目报警
		{
			alarmxml=CreateProgramAlarmXML(checkparam,mode,alarmid);
			break;
		}
	case ALARM_FREQ:
	case ALARM_TR101_290://290
		{
			alarmxml=CreateFreqAlarmXML(checkparam,mode,alarmid);
			break;
		}
	default:
		break;
	}
	return true;
}

std::string AlarmMgr::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	switch(checkparam.DVBType)
	{
	case ATV:
	case RADIO:
	case AM:
	case CTTB:
	case DVBC:
	case DVBS:
	case CTV:
		return CreateAlarmXMLForDVBC::CreateFreqAlarmXML(checkparam,alarmvalue,alarmid);
	default:
		return "";
	}
	return "";
}
std::string AlarmMgr::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	switch(checkparam.DVBType)
	{
	case ATV:
	case RADIO:
	case AM:
	case CTTB:
	case DVBC:
	case DVBS:
	case CTV:
		return CreateAlarmXMLForDVBC::CreateProgramAlarmXML(checkparam,alarmvalue,alarmid);
	default:
		return "";
	}
	return "";
}
std::string AlarmMgr::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	switch(checkparam.DVBType)
	{
	case ATV:
		return CreateAlarmXMLForATV::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case RADIO:
		return CreateAlarmXMLForRADIO::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case AM:
		return CreateAlarmXMLForAM::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case CTTB:
		return CreateAlarmXMLForCTTB::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case DVBC:
		return CreateAlarmXMLForDVBC::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case DVBS:
		return CreateAlarmXMLForDVBS::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	case CTV:
		return CreateAlarmXMLForCTV::CreateEnvironmentAlarmXML(checkparam,alarmvalue);
	default:
		return "";
	}
	return "";
}
std::string AlarmMgr::CreateEquipmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	switch(checkparam.DVBType)
	{
	case DVBC:
		return CreateAlarmXMLForDVBC::CreateEquipmentAlarmXML(checkparam,alarmvalue);
	default:
		return "";
	}
	return "";
}
bool AlarmMgr::ClearAlarm(string Freq)
{
	if(Freq=="ALL")
	{
		ETRMutex.acquire();
		ETRAlarm.clear();
		ETRMutex.release();
		ProgramMutex.acquire();
		ProgramAlarm.clear();
		ProgramMutex.release();
	}
	else
	{
		string key="_"+Freq+"_";
		ETRMutex.acquire();
		if(ETRAlarm.size()>0)
		{
			std::map<std::string,std::vector<sCheckParam> >::iterator itorE=ETRAlarm.begin();
			for(;itorE!=ETRAlarm.end();itorE++)
			{
				if(itorE->first.find(key.c_str())!=string::npos)
				{
					ETRAlarm.erase(itorE);
					break;
				}
			}
		}
		ETRMutex.release();
		ProgramMutex.acquire();
		if(ProgramAlarm.size()>0)
		{
			std::map<std::string,std::vector<sCheckParam> >::iterator itorP=ProgramAlarm.begin();
			for(;itorP!=ProgramAlarm.end();itorP++)
			{
				if(itorP->first.find(key.c_str())!=string::npos)
				{
					ProgramAlarm.erase(itorP);
					break;
				}
			}
		}
		ProgramMutex.release();
	}
	return true;
}
/**********************************************/
// 如果报警开关关闭，则放弃报警检查（当前以及通用参数），返回失败
// 如果未设置报警门限，则放弃报警检查（当前以及通用参数），返回失败
// 否则返回成功并将报警参数返回
/**********************************************/
bool AlarmMgr::GetAlarmParm(const sCheckParam& checkparam,sAlarmParam& param)
{
	sAlarmParam tempalarmparam;
	bool ret = false;

	//获取报警开关
	ret = ALARMPARAMINFOMGR::instance()->GetAlarmParam(checkparam,tempalarmparam);			//根据测量的报警值获得相应的报警参数设置
	if(tempalarmparam.Switch=="0")
	{
		param=tempalarmparam;
		return true;
	}
	if (ret==false)											//如果当前频点没有找到参数或者开关参数为关闭，则找通用参数
	{
		sCheckParam temp=checkparam;
		temp.Freq="All";								
		ret=ALARMPARAMINFOMGR::instance()->GetAlarmParam(temp,tempalarmparam);
	}
	if (ret==false)											//当前类型以及通用参数都没有找到或者开关都为关闭，则放弃当前报警检测
		return false;

	//获取报警门限
	switch(checkparam.AlarmType)//根据报警类型检测是否报警，并生成上报的xml
	{
	case ALARM_ENVIRONMENT:
		{
			ret = ALARMPARAMINFOMGR::instance()->GetAlarmParam(checkparam,tempalarmparam);			//根据测量的报警值获得相应的报警参数设置
			if (ret==false || tempalarmparam.DownThreshold.length()<1 || tempalarmparam.UpThreshold.length()<1)	//如果当前频点没有找到参数，则找通用参数
			{
				sCheckParam temp=checkparam;
				temp.Freq="All";															
				ret=ALARMPARAMINFOMGR::instance()->GetAlarmParam(temp,tempalarmparam);
			}
			if (ret==false || tempalarmparam.DownThreshold.length()<1 || tempalarmparam.UpThreshold.length()<1)	//当前类型以及通用参数都没有找到，则放弃当前报警检测
				return false;

			break;
		}
	case ALARM_FREQ:
		{
			ret = ALARMPARAMINFOMGR::instance()->GetAlarmParam(checkparam,tempalarmparam);			//根据测量的报警值获得相应的报警参数设置
			if (ret==false || tempalarmparam.DownThreshold.length()<1 || tempalarmparam.UpThreshold.length()<1)	//如果当前频点没有找到参数，则找通用参数
			{
				sCheckParam temp=checkparam;
				temp.Freq="All";															
				ret=ALARMPARAMINFOMGR::instance()->GetAlarmParam(temp,tempalarmparam);
			}
			if (ret==false || tempalarmparam.DownThreshold.length()<1 || tempalarmparam.UpThreshold.length()<1)	//当前类型以及通用参数都没有找到，则放弃当前报警检测
				return false;

			break;
		}
	case ALARM_PROGRAM:
		{
			ret = ALARMPARAMINFOMGR::instance()->GetAlarmParam(checkparam,tempalarmparam);			//根据测量的报警值获得相应的报警参数设置
			if (ret==false || tempalarmparam.Duration.length()<1 || tempalarmparam.Duration=="0")			//如果当前频点没有找到参数，则找通用参数
			{
				sCheckParam temp=checkparam;
				temp.Freq="All";															
				ret=ALARMPARAMINFOMGR::instance()->GetAlarmParam(temp,tempalarmparam);
			}
			if (ret==false || tempalarmparam.Duration.length()<1 || tempalarmparam.Duration=="0")			//当前类型以及通用参数都没有找到，则放弃当前报警检测
				return false;

			break;
		}
	case ALARM_TR101_290:
		{
			ret = ALARMPARAMINFOMGR::instance()->GetAlarmParam(checkparam,tempalarmparam);							//根据测量的报警值获得相应的报警参数设置
			if (ret==false || tempalarmparam.TimeInterval.length()<1 || tempalarmparam.Num.length()<1 || tempalarmparam.Num=="0")	//如果当前频点没有找到参数，则找通用参数
			{
				sCheckParam temp=checkparam;
				temp.Freq="All";															
				ret=ALARMPARAMINFOMGR::instance()->GetAlarmParam(temp,tempalarmparam);
			}
			if (ret==false || tempalarmparam.TimeInterval.length()<1 || tempalarmparam.Num.length()<1 || tempalarmparam.Num=="0")	//当前类型以及通用参数都没有找到，则放弃当前报警检测
				return false;
			break;
		}
	default:
		break;
	}
	//将报警参数返回
	param=tempalarmparam;
	param.Switch="1";
	return true;
}

string AlarmMgr::GetAlarmPriority(eDVBType dvbtype,string type)
{
	if(vecAlarmPriority.size()>0)
	{
		std::vector<sAlarmPriority>::iterator itr=vecAlarmPriority.begin();
		for(;itr!=vecAlarmPriority.end();itr++)
		{
			if((*itr).dvbtype==dvbtype&&(*itr).type==type)
				return ((*itr).priority);
		}
	}
	return string("1");
}

string AlarmMgr::GetAlarmTypeID(string headstr)
{
	if(vecSeperateTypeID.size()>0)
	{
		std::vector<SeperateTypeID>::iterator itr = vecSeperateTypeID.begin();
		for(;itr!=vecSeperateTypeID.end();itr++)
		{
			if((*itr).head==headstr)
			{
				return ((*itr).type);	
			}
		}
	}
	return string("");
}
void AlarmMgr::SendAlarm(std::string alarmxml)
{
	if(alarmxml.length()>0)
	{
		ACE_Message_Block *MBALARM = new ACE_Message_Block(alarmxml.length());
		memcpy(MBALARM->wr_ptr(),alarmxml.c_str(),alarmxml.length());
		MBALARM->wr_ptr(alarmxml.length());
		pAlarmSender->putq(MBALARM);//将报警xml添加到上报队列中
	}
}
