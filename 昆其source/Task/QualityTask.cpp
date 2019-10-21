
#pragma warning(disable:4996)

#include <vector>
#include <iostream>
#include "ace/Task.h"

#include "./QualityTask.h"
#include "./Scheduler.h"
#include "./TranslateDownXML.h"
#include "./TranslateXMLForDevice.h"
#include "../Alarm/AlarmMgr.h"
#include "../DBAccess/DBManager.h"
#include "../DBAccess/QualityDbManager.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DeviceAccess/FetchQualityLevel.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/BaseAnalyser.h"
#include "../DeviceAccess/QuaDevInfoTCPDeviceAccess.h"
#include "../BusinessProcess/QualityCompensationMgr.h"


using namespace std;


QualityTask::QualityTask() : DeviceRelatedTask()
{

}

QualityTask::QualityTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	TaskScheduler =new Scheduler();

	int mCheckInterVal = 0;
	int cycleInterval = 0;
	//设置任务的运行时间
	XmlParser parser;
	parser.Set_xml(strXML);
	pXMLNODE qualityTaskNode = parser.GetNodeFromPath("Msg/QualityTask");
	std::string taskid;
	parser.GetAttrNode(qualityTaskNode,"TaskID",taskid);
	//
	if(this->GetVersion() == "3.0")
	{
		string strDevid;
		parser.GetAttrNode(qualityTaskNode,"TunerIndex",strDevid);
		DeviceID = StrUtil::Str2Int(strDevid);
	}
	SetTaskID(taskid);

	pXMLNODE node=parser.GetNodeFromPath("Msg/QualityTask/TimeParam/TimeIndex");
	std::string DayOfWeek, StartTime, EndTime,StartDateTime, EndDateTime, CheckInterVal;

	parser.GetAttrNode(node,"DayOfWeek",DayOfWeek);
	parser.GetAttrNode(node,"StartTime",StartTime);
	parser.GetAttrNode(node,"EndTime",EndTime);
	parser.GetAttrNode(node,"StartDateTime",StartDateTime);
	parser.GetAttrNode(node,"EndDateTime",EndDateTime);
	parser.GetAttrNode(node,"CheckInterval",CheckInterVal);
	//
	m_alarmlimit = false;
	if(CheckInterVal == "33:33:33")
	{
		m_alarmlimit = true;//指标报警门限记录指标待完成
		CheckInterVal = "00:00:05";
	}
	mCheckInterVal=TimeUtil::StrToSecondTime(CheckInterVal);
	if(StartDateTime.length()<1 && EndDateTime.length()<1)
	{
		int differ = TimeUtil::StrToSecondTime(EndTime) - TimeUtil::StrToSecondTime(StartTime);
		if (stricmp(DayOfWeek.c_str(),"ALL"))  //每周的某一天
		{
			std::string dateTime,startDate;
			long today=TimeUtil::DateIsWeekDay(time(0));//今天星期几
			int diffDay=StrUtil::Str2Int(DayOfWeek)-today;//星期之间所差的天数
			if (diffDay<0)
			{
				diffDay+=7;
			}
			dateTime=TimeUtil::CalDay(time(0),diffDay);
			startDate=TimeUtil::GetDateFromDatetime(dateTime);
			std::string StartDateTime=startDate+" "+StartTime;
			std::string EndDateTime=startDate+" "+EndTime;
			cycleInterval = 7*ONEDAYLONG-differ;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleInterval,mCheckInterVal);
		}
		else //每天都做
		{
			cycleInterval = ONEDAYLONG - differ;
			std::string curDate = TimeUtil::GetCurDate();
			std::string StartDateTime=curDate+" "+StartTime;
			std::string EndDateTime=curDate+" "+EndTime;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleInterval,mCheckInterVal);
		}
	}
	else if (StartDateTime != "" && EndDateTime != "")
	{

		TaskScheduler->SetRunTime(StartDateTime,EndDateTime,EndDateTime,0,mCheckInterVal);
	}
}

QualityTask::~QualityTask()
{

}

void QualityTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标任务执行 !\n",DeviceID));

	bRun = true;
	SetRunning();
	RetValue = RUN_SUCCESS;
	XmlParser parser;
	int idev = -1;
	std::string TaskID;
	parser.Set_xml(strStandardXML);
	pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTask");
	parser.GetAttrNode(qualityNode,"TaskID",TaskID);
	parser.GetAttrNode(qualityNode,"TunerIndex",idev);

	mVecStdStr=TranslateXMLForDevice::TranslateQualityTask(strStandardXML);

	for (size_t i=0;i!=mVecStdStr.size();++i)
	{
	
		std::string retDeviceXml;
		std::string	Freq;
		std::string strTime = TimeUtil::GetCurDateTime();
		std::vector<eQualityInfo> vecqInfo;

		
		if (DVBType == DVBC||DVBType == CTTB||DVBType == DVBS||DVBType == RADIO||DVBType == AM)
		{
			string strStdXml = mVecStdStr[i];
			XmlParser stdParser(strStdXml.c_str());
			pXMLNODE qualityNode = stdParser.GetNodeFromPath("Msg/QualityQuery");
			stdParser.GetAttrNode(qualityNode,"Freq",Freq);

			string strtem;
			bool bRet = this->GetQualityInfoXmlFromDevice(idev, Freq, strtem);
			if (!bRet)
				continue;

			string strStr = "30";
			string strNse = "2";
			string strMer = "17";
			string strBer = "0.0000002";
			string strAMFMModulation = "0";
			XmlParser psr;
			psr.Set_xml(strtem);
			pXMLNODE node=psr.GetNodeFromPath("html/body");
			pXMLNODELIST nodeList = psr.GetNodeList(node);
			int cnt = nodeList->Size();
			pXMLNODE scanNode = NULL;
			for(int k=0;k<cnt;k++)
			{
				pXMLNODE childNode = psr.GetNextNode(nodeList);
				string name = psr.GetNodeName(childNode);
				if(name == "RFInfo")
				{
					psr.GetAttrNode(childNode,"str",strStr);
					if(DVBType == CTTB)
					{
						psr.GetAttrNode(childNode,"noise",strNse);
					}
					else
					{
						if (PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")
							psr.GetAttrNode(childNode,"nse",strNse);
						else
							psr.GetAttrNode(childNode,"snr",strNse);
					}
					if(DVBType == AM|| DVBType == RADIO)
					{
						psr.GetAttrNode(childNode,"modulation",strAMFMModulation);
					}
					else
					{
						psr.GetAttrNode(childNode,"mer",strMer);
						psr.GetAttrNode(childNode,"ber",strBer);
					}
				}
				else if(name == "err")
				{
					string er;
					psr.GetAttrNode(childNode,"code",er);
					if(er != "0")
					{
						cout<<"QualityTask err code:"<<er<<endl;
						break;
					}
				}
			}
			string sOutBer;
			bool bret = this->GetBerInfo2ScientificNotation(strBer,Freq, strStr, sOutBer);
			if (bret)
			{
				strBer = sOutBer;
			}
			
			pXMLNODE paramNode = stdParser.GetNodeFromPath("Msg/QualityParam");
			pXMLNODELIST paramList = stdParser.GetNodeList(paramNode);
			int icount=stdParser.GetChildCount(paramNode);
			for (int i = 0; i < icount; i++)
			{
				eQualityInfo QuaInfo;
				QuaInfo.freq = Freq;
				string type,desc;
				pXMLNODE paramnode = stdParser.GetNextNode(paramList);	
				stdParser.GetAttrNode(paramnode,"Type",type);
				stdParser.GetAttrNode(paramnode,"Desc",desc);
				if (type == "0")//ALL
				{
					QuaInfo.type = "1";
					QuaInfo.desc = "Level";
					QuaInfo.valu = strStr;
					vecqInfo.push_back(QuaInfo);
					if(DVBType == AM|| DVBType == RADIO)
					{
						eQualityInfo qi1;
						qi1.freq = Freq;
						qi1.type = "4";
						qi1.desc = "CNR";
						qi1.valu = strNse;
						vecqInfo.push_back(qi1);

						eQualityInfo qi2;
						qi2.freq = Freq;
						if(DVBType == AM)
						{
							qi2.type = "8";
							qi2.desc = "AM-Modulation";
						}
						else
						{
							qi2.type = "9";
							qi2.desc = "FM-Modulation";
						}
						qi2.valu = strAMFMModulation;
						vecqInfo.push_back(qi2);
					}
					else
					{
						eQualityInfo qi1;
						qi1.freq = Freq;
						qi1.type = "2";
						qi1.desc = "BER";
						qi1.valu = strBer;
						vecqInfo.push_back(qi1);

						eQualityInfo qi2;
						qi2.freq = Freq;
						qi2.type = "3";
						qi2.desc = "MER";
						qi2.valu = strMer;
						vecqInfo.push_back(qi2);
					}
					break;
				}
				else if(type=="1")//Level
				{
					QuaInfo.type = type;
					QuaInfo.desc = desc;
					QuaInfo.valu = StrUtil::Int2Str(StrUtil::Str2Int(strStr) + 107);
					vecqInfo.push_back(QuaInfo);
				}
				else if(type=="2")//BER
				{
					QuaInfo.type = type;
					QuaInfo.desc = desc;
					QuaInfo.valu = strBer;
					vecqInfo.push_back(QuaInfo);
				}
				else if(type=="3")//MER
				{
					if(GetVersion() == "3.0")
					{
						QuaInfo.type = type;
						QuaInfo.desc = desc;
						QuaInfo.valu = strMer;
						vecqInfo.push_back(QuaInfo);
					}
				}
				else if(type=="4")
				{
					if(GetVersion() == "3.0")
					{
						QuaInfo.type = type;
						QuaInfo.desc = desc;
						QuaInfo.valu = strNse;
						vecqInfo.push_back(QuaInfo);
					}
				}
				else if(type=="5")
				{
				}
				else if(type=="8"||type=="9")
				{
					if(GetVersion() == "3.0")
					{
						QuaInfo.type = type;
						QuaInfo.desc = desc;
						QuaInfo.valu = strAMFMModulation;
						vecqInfo.push_back(QuaInfo);
					}
				}
			}
		}
		//指标信息入库
		for (size_t j =0;j!=vecqInfo.size();++j)
		{
			vecqInfo[j].checktime = strTime;
			//QUALITYDBMANAGER::instance()->AddQualityInfo(DVBType,Freq,TaskID,vecqInfo[j]);暂时不存库，还涉及指标删除
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标:%s\t%s\t%s\t\n",DeviceID,vecqInfo[j].type.c_str(),
				vecqInfo[j].desc.c_str(),vecqInfo[j].valu.c_str()));
			//
			sCheckParam sChP;
			ReadyForAlarm(vecqInfo[j],sChP);
			ALARMMGR::instance()->CheckAlarm(sChP,false);
		}
	}
	
	if (TaskScheduler != NULL)
	{
		TaskScheduler->SetNextRunTime();
	}
	SetFinised();

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标任务停止 !\n",DeviceID));
}
string QualityTask::GetTaskName()
{
	return std::string("指标任务");
}
std::string QualityTask::GetObjectName()
{
	return std::string("QualityTask");
}
bool  QualityTask::GetQualityInfo(std::string orgXML,std::string rtnXML, std::string &Freq,std::string &taskID,std::vector<eQualityInfo> & vecQualityInfo)
{
	XmlParser parser(orgXML.c_str());
	XmlParser rtnParser(rtnXML.c_str());
	pXMLNODE  querySetNode= parser.GetNodeFromPath("Msg/QualityQuery");
	parser.GetAttrNode( querySetNode,"Freq",Freq );

	pXMLNODE paramNode = rtnParser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");
	pXMLNODELIST indexList = rtnParser.GetNodeList( paramNode );
	int count = rtnParser.GetChildCount( paramNode );
	std::string Type ="",Desc = "", Value = "";
	for ( int i = 0;i<count;i++ )
	{
		pXMLNODE indexNode = rtnParser.GetNextNode( indexList );
		//获取eQualityInfo结构体的值
		rtnParser.GetAttrNode( indexNode,"Type",Type );
		rtnParser.GetAttrNode( indexNode,"Desc",Desc );
		rtnParser.GetAttrNode( indexNode,"Value",Value );
		eQualityInfo  mqInfo;
		if (DVBType == CTTB && Type=="2")
		{
			mqInfo.type = "3";
		}
		else if (DVBType == CTTB && Type=="3")
		{
			mqInfo.type = "2";
		}
		else
		{
			mqInfo.type = Type;
		}
		
		mqInfo.desc = Desc;
		mqInfo.valu = Value;
		vecQualityInfo.push_back(mqInfo);
	}
	return true;
}

void QualityTask::SetDeciveID( int deviceid )
{
	DeviceID=deviceid;
}

bool QualityTask::AddTaskXml()
{
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE node=psr.GetNodeFromPath("Msg/QualityTask");
	if (node)
	{
		psr.SetAttrNode("DeviceID",DeviceID,node); //分配任务的deviceid
		psr.Get_xml(strStandardXML);
	}

	//插入任务命令执行5次，如果全部失败，就返回false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
		if (ret)
		{
			break;
		}
	}
	 
	return ret;
}

bool QualityTask::DelTaskXml()
{
	return DBMANAGER::instance()->DeleteTask(DVBType,strStandardXML);
}
bool QualityTask::ReadyForAlarm(eQualityInfo eQinfo,sCheckParam& alarmVec)
{
	string freq,STD,SymbolRate,type,desc,val;
	type = eQinfo.type;
	desc = eQinfo.desc;
	val = eQinfo.valu;
	int val_int = StrUtil::Str2Int(val);
	alarmVec.DVBType = this->GetDVBType();
	alarmVec.AlarmType = ALARM_FREQ;
	alarmVec.TypeID = type;
	alarmVec.TypeDesc = desc;
	alarmVec.STD = STD;
	alarmVec.Freq = eQinfo.freq;
	alarmVec.SymbolRate = SymbolRate;
	alarmVec.ChannelID = this->GetChannelID();
	if(DVBType==ATV||DVBType==RADIO||DVBType==AM||DVBType==CTV)
	{
		alarmVec.TypedValue = StrUtil::Float2Str((float)val_int/1000);
	}
	else if(DVBType==DVBC||DVBType==CTTB||DVBType==DVBS)
	{
		alarmVec.TypedValue = StrUtil::Float2Str((float)val_int);
	}
	alarmVec.DeviceID = StrUtil::Int2Str(DeviceID);
	alarmVec.CheckTime=time(0);
	return true;
}
bool QualityTask::GetQualityInfoXmlFromDevice(int iDevID, std::string strFreq, std::string& strRetXml)
{
	if(iDevID == -1)
	{
		return false;
	}
	int idevID = iDevID;
	HTTPDeviceAccess* pDeviceAccess = NULL;
	TunerDev tdev;
	string tunermonitorip,tunermonitorport;
	eDVBType edvtype;
	int iDeviceID = GetDeviceID(); //DeviceID 不要直接使用
	if(PROPMANAGER::instance()->GetTunerDevFromFreq(idevID,edvtype,"quality",strFreq,""))
	{
		if(edvtype != DVBType)
		{
			cout<<"指标任务奇怪-频点一样类型不一样:"<<strFreq<<"-"<<OSFunction::GetStrDVBType(DVBType)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
		}
		PROPMANAGER::instance()->GetTunerInfo(idevID,DVBType,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,DVBType,idevID,tunermonitorip,tunermonitorport);//tuner
		pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
	}
	else
	{
		sFreqScanInfo freqscaninfo;
		PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
		//
		PROPMANAGER::instance()->GetTunerInfo(idevID,DVBType,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,DVBType,idevID,tunermonitorip,tunermonitorport);//tuner
		pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		//
		string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
		string strRet;
		cmd+="chl=";
		cmd+=tdev.s_cardindex;
		cmd+="_";
		cmd+=tdev.s_chanindex;
		cmd+="&fre=";
		cmd+=strFreq;
		cmd+="&sym=";
		cmd+=freqscaninfo.SymbolRate;
		cmd+="&qam=2&ip=";
		cmd+=tdev.s_sip;
		cmd+="&tspt=";
		cmd+=tdev.s_stsport;
		cmd+="&trpt=";
		cmd+=tdev.s_trport;
		cmd+="&desc=";
		cmd+="0";
		cmd+="&evip=";
		cmd+=tdev.s_290ip;
		cmd+="&evpt=";
		cmd+=tdev.s_290port;
		pDeviceAccess->SendTaskMsg(cmd,strRet);
		cout<<cmd<<"****ret:*****"<<strRet<<endl;
		Sleep(2000);
	}
	//
	string cmd = "POST /io/info  HTTP/1.1  \r\n\r\n";
	string strRet;
	cmd+="chl=";
	cmd+=tdev.s_cardindex;
	cmd+="_";
	cmd+=tdev.s_chanindex;
	pDeviceAccess->SendTaskMsg(cmd,strRet);
	delete pDeviceAccess;
	pDeviceAccess = NULL;

	cout<<cmd<<"****ret:*****"<<strRet<<endl;
	//
	if(strRet.size() <= 0)
	{
		cout<<"QualityTask命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
		return false;
	}
	int pos = strRet.find("<html>");
	string strtem = strRet;
	if(pos >= 0)
	{
		strtem = strRet.substr(pos,strRet.size()-pos);
		strRetXml = strtem;
	}
	else
	{
		cout<<"QualityTask没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
		return false;
	}

	return true;
}
//用其它设备(场强卡,频谱仪) 获取场强信息
bool QualityTask::GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel)
{
	if (pQua->s_devType == "QuaCard")
	{
		
		DEVICEFREQLEVEL::instance()->InitDevice();
		std::string tmpfreq=strFreq;
		if (tmpfreq.length()>3)
		{
			tmpfreq=tmpfreq.substr(0,3);
		}
		float level;
		DEVICEFREQLEVEL::instance()->GetFreqLevel(StrUtil::Str2Float(tmpfreq),level);
		DEVICEFREQLEVEL::instance()->CloseDevice();
		sLevel = StrUtil::Float2Str(level);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]场强卡 场强指标:%s\n", DeviceID, sLevel.c_str()));
	}
	else if (pQua->s_devType == "Spectrum")
	{
		if ((pQua->s_cmdProtocol == "tcp"))
		{
			CQuaDevInfoTCPDeviceAccess *pDeviceAccess = NULL;
			pDeviceAccess = new CQuaDevInfoTCPDeviceAccess(pQua->i_index, pQua->s_qualityIP, pQua->i_port);
			if (pDeviceAccess != NULL)
			{
				string s_level;
				bool bret = false;
				//bret = pDeviceAccess->SendQuaDevInfoCmd(strFreq, s_level);
				/////////////////////////////////频谱中取 Level/////////////////////////////////////////
				std::string tmpfreq=strFreq;

				long l_freq = StrUtil::Str2Long(tmpfreq);
				string startFreq = StrUtil::Long2Str(l_freq-1000);
				string endFreq = StrUtil::Long2Str(l_freq+1000);
				bret = pDeviceAccess->SendSpecDevInfoCmd(startFreq, endFreq, 1, s_level);
				string strRet = s_level;
				strRet = strRet.substr(5, strRet.length());//aaa2:
				string str1 = "";
				int ipos1 = -1;
				int ipos2 = -1;
				vector<string> vLever;
				while(1)
				{
					ipos1 = -1;
					ipos1 = strRet.find_first_of(",");
					str1 = "";
					ipos2 = -1;
					if (ipos1 > 0)
					{
						str1 = strRet.substr(0, ipos1);
						ipos2 = str1.find("=");
						if (ipos2 <= 0)
							continue;
						string sFreq = str1.substr(0, ipos2);
						string slever = str1.substr(ipos2+1, str1.length());
						int ipos3 = sFreq.find(".");
						sFreq = sFreq.substr(0, ipos3);
						if (StrUtil::Str2Long(sFreq)*1000 == l_freq)
						{
							vLever.push_back(slever);
						}
						else if (StrUtil::Str2Long(sFreq)*1000 > l_freq)
						{
							break;
						}

						strRet = strRet.substr(ipos1+1, strRet.length());
					}
					else
					{
						if (strRet.length() > 0)
						{
							str1 = strRet;
							ipos2 = str1.find("=");
							if (ipos2 <= 0)
								break;
							string sFreq = str1.substr(0, ipos2);
							string slever = str1.substr(ipos2+1, strRet.length());
							int ipos3 = sFreq.find(".");
							sFreq = sFreq.substr(0, ipos3);
							if (StrUtil::Str2Long(sFreq)*1000 == l_freq)
							{
								vLever.push_back(slever);
							}

						}

						break;
					}
				}

				float isize = vLever.size();
				s_level = "";
				if (isize > 0)
				{
					vector<string>::iterator ite_map = vLever.begin();
					float flevel = 0.0;
					for(ite_map; ite_map!= vLever.end(); ite_map++)
					{
						flevel += StrUtil::Str2Float((*ite_map).c_str());
					}
					s_level = StrUtil::Float2Str(flevel/isize);
					PROPMANAGER::instance()->AdjustingFreqLevel(strFreq, s_level);
					
				}
				//////////////////////////////////////////////////////////////////////////
				if (bret)
				{
					sLevel = s_level;
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频谱仪 场强指标:%s\n", DeviceID, sLevel.c_str()));
				}
				delete pDeviceAccess;
				pDeviceAccess = NULL;
			}
		}
	}

	return true;
}
//用其它设备(场强卡,频谱仪) 获取场强信息
bool QualityTask::GetLevelFromOtherDevice(Quality_DevInfo* pQua, vector<std::string> vecFreqs, map<std::string, std::string>& mapFreqLevels)
{
	if ((pQua->s_cmdProtocol == "tcp"))
	{
		CQuaDevInfoTCPDeviceAccess *pDeviceAccess = NULL;
		pDeviceAccess = new CQuaDevInfoTCPDeviceAccess(pQua->i_index, pQua->s_qualityIP, pQua->i_port);
		if (pDeviceAccess != NULL)
		{
			string s_level;
			bool bret = false;
			/////////////////////////////////频谱中取 Level/////////////////////////////////////////
			int ivecFreqSize = vecFreqs.size();
			vector<std::string> vfreqs;
			vfreqs = vecFreqs;
			std::string tmpfreq = "";
			std::string startFreq = "";
			std::string endFreq = "";
			if (ivecFreqSize > 1)
			{
				for(int i = 0; i < vfreqs.size(); i++)
				{
					for (int j = i; j < vfreqs.size()-1; j++)
					{
						long l_freq_1 = StrUtil::Str2Long(vfreqs[j]);
						long l_freq_2 = StrUtil::Str2Long(vfreqs[j+1]);
						if (l_freq_1 > l_freq_2)//排序 大在后，小在前
						{
							std::string sFreq = vfreqs[j];
							vfreqs[j] = vfreqs[j+1];
							vfreqs[j+1] = sFreq;
						}
					}
				}
				std::string sStart = vfreqs[0];
				std::string sEnd = vfreqs[ivecFreqSize-1];
				long l_freq = StrUtil::Str2Long(sStart);
				startFreq = StrUtil::Long2Str(l_freq - 1000);
				l_freq = StrUtil::Str2Long(sEnd);
				endFreq = StrUtil::Long2Str(l_freq + 1000);
			}
			else if (ivecFreqSize == 1)
			{
				tmpfreq = vfreqs[0];
				long l_freq = StrUtil::Str2Long(tmpfreq);
				//startFreq = StrUtil::Long2Str(l_freq - 1000);
				//endFreq = StrUtil::Long2Str(l_freq + 1000);
				startFreq = StrUtil::Long2Str(l_freq - 4000);
				endFreq = StrUtil::Long2Str(l_freq + 4000);
			}
			else 
			{
				return false;
			}

			bret = pDeviceAccess->SendSpecDevInfoCmd(startFreq, endFreq, 0.1, s_level);
			string strRet = s_level;
			strRet = strRet.substr(5, strRet.length());//aaa2:
			string str1 = "";
			int ipos1 = -1;
			int ipos2 = -1;
			vector<SpectrumInfo> vSpectrumInfo;
			vector<std::string> vLevel;
			string sOldFreq = "";
			vector<string> vOldFreqLever;
			while(1)
			{
				ipos1 = -1;
				ipos1 = strRet.find_first_of(",");
				str1 = "";
				ipos2 = -1;
				if (ipos1 > 0)
				{
					str1 = strRet.substr(0, ipos1);
					ipos2 = str1.find("=");
					if (ipos2 <= 0)
						continue;
					long lFreq = 0;
					if (ivecFreqSize == 1)
					{
						string sFreq = str1.substr(0, 5);//698.0
						string slever = str1.substr(ipos2+1, str1.length());
						lFreq = StrUtil::Str2Long(sFreq)*1000;
						//if (lFreq == StrUtil::Str2Long(tmpfreq))
						//{
						//	vLevel.push_back(slever);
						//}
						if (sFreq != sOldFreq)
						{
							sOldFreq = sFreq;
							if (vOldFreqLever.size() > 0)
							{
								float flever = 0.0;
								for(int i = 0; i < vOldFreqLever.size(); i++)
								{
									string s = vOldFreqLever[i];
									flever += StrUtil::Str2Float(s.c_str());
								}
								slever = StrUtil::Float2Str(flever / (float) vOldFreqLever.size());
								vLevel.push_back(slever);
								vOldFreqLever.clear();
							}
						}
						vOldFreqLever.push_back(slever);
					}
					else if (ivecFreqSize > 1)
					{
						string sFreq = str1.substr(0, ipos2);
						string slever = str1.substr(ipos2+1, str1.length());
						int ipos3 = sFreq.find(".");
						sFreq = sFreq.substr(0, ipos3);
						lFreq = StrUtil::Str2Long(sFreq)*1000;
						for (int i = 0; i < vfreqs.size(); i++)
						{
							string str = vfreqs[i];
							if (StrUtil::Str2Long(str) == lFreq)
							{
								SpectrumInfo si;
								si.freq = lFreq;
								si.level = StrUtil::Str2Float(slever);
								vSpectrumInfo.push_back(si);
							}
						}
					}
					if (lFreq > StrUtil::Str2Long(endFreq))
					{
						break;
					}

					strRet = strRet.substr(ipos1+1, strRet.length());
				}
				else
				{
					if (strRet.length() > 0)
					{
						str1 = strRet;
						ipos2 = str1.find("=");
						if (ipos2 <= 0)
							break;
						long lFreq = 0;
						if (ivecFreqSize == 1)
						{
							string sFreq = str1.substr(0, 5);//698.0
							string slever = str1.substr(ipos2+1, str1.length());
							lFreq = StrUtil::Str2Long(sFreq)*1000;
							//if (lFreq == StrUtil::Str2Long(tmpfreq))
							//{
							//	vLevel.push_back(slever);
							//}
							if (sFreq != sOldFreq)
							{
								sOldFreq = sFreq;
								if (vOldFreqLever.size() > 0)
								{
									float flever = 0.0;
									for(int i = 0; i < vOldFreqLever.size(); i++)
									{
										string s = vOldFreqLever[i];
										flever += StrUtil::Str2Float(s.c_str());
									}
									slever = StrUtil::Float2Str(flever / (float) vOldFreqLever.size());
									vLevel.push_back(slever);
									vOldFreqLever.clear();
								}
							}
							vOldFreqLever.push_back(slever);
						}
						else if (ivecFreqSize > 1)
						{
							string sFreq = str1.substr(0, ipos2);
							string slever = str1.substr(ipos2+1, strRet.length());
							int ipos3 = sFreq.find(".");
							sFreq = sFreq.substr(0, ipos3);
							for (int i = 0; i < vfreqs.size(); i++)
							{
								string str = vfreqs[i];
								if (StrUtil::Str2Long(str) == lFreq)
								{
									SpectrumInfo si;
									si.freq = lFreq;
									si.level = StrUtil::Str2Float(slever);
									vSpectrumInfo.push_back(si);
								}
							}
						}
					}

					break;
				}
			}
			if (ivecFreqSize == 1)
			{
				float isize = vLevel.size();
				s_level = "";
				if (isize > 0)
				{
					vector<string>::iterator ite_map = vLevel.begin();
					double tempval=0;
					for(ite_map; ite_map!= vLevel.end(); ite_map++)
					{
						tempval+=pow(10.0f,StrUtil::Str2Float((*ite_map).c_str())/10.0f);
					}
					float flevel = 10.0f*log10(tempval);
					s_level = StrUtil::Float2Str(flevel);
					PROPMANAGER::instance()->AdjustingFreqLevel(tmpfreq, s_level);
				}
				if (bret)
				{
					mapFreqLevels.insert(make_pair(tmpfreq, s_level));
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频谱仪 [%s]场强指标:%s\n", DeviceID, tmpfreq.c_str(), s_level.c_str()));
				}
			}
			else if (ivecFreqSize > 1)
			{
				if (vSpectrumInfo.size() > 0)
				{
					for(int i = 0; i < vfreqs.size(); i++)
					{
						string sfreq = vfreqs[i];
						float isize = 0.0;
						float flevel = 0.0;

						for (int j = 0; j < vSpectrumInfo.size(); j++)
						{
							SpectrumInfo* p = &vSpectrumInfo[j];
							long freq = (long)p->freq;
							if (StrUtil::Str2Long(sfreq) == freq)
							{
								flevel += p->level;
								isize += 1;
							}
						}
						string level = "";
						level = StrUtil::Float2Str(flevel/isize);
						PROPMANAGER::instance()->AdjustingFreqLevel(sfreq, level);
						ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频谱仪 [%s]场强指标:%s\n", DeviceID, sfreq.c_str(), level.c_str()));
						mapFreqLevels.insert(make_pair(sfreq, level));
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			delete pDeviceAccess;
			pDeviceAccess = NULL;
		}
	}

	return true;
}

//将 Ber 信息 转换成 科学计数法
bool QualityTask::GetBerInfo2ScientificNotation(string sInBer, string sFreq, string sLevel, string& sOutBer)
{
	if (sInBer == "")
		return false;
	string val = sInBer;
	int ps = val.find("_");
	string num;
	string sum;
	if(ps != -1)
	{
		num = val.substr(0,ps);
		sum = val.substr(ps+1,val.size()-ps-1);
	}
	if(num=="0")
	{
		val = "0.0E-8";
	}
	else
	{
		string type = "2";//BER
		string add;
		QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(GetDVBType(), StrUtil::Int2Str(DeviceID), type, sFreq, sLevel, add);//新版数字指标通道使用0,数据库校准配置也是0
		float ftemnumValue = StrUtil::Str2Float(num);
		float fValue = 0.00000001;
		if(add.size()>0)
		{
			ftemnumValue = ftemnumValue+StrUtil::Str2Int(add);
		}
		if(ftemnumValue>0&&sum.size()>0)
		{
			fValue = ftemnumValue/StrUtil::Str2Float(sum);
		}
		else if(num.size()>0&&sum.size()>0)//防止纠正过界成负数的处理
		{
			fValue = (StrUtil::Str2Float(num)/StrUtil::Str2Float(sum));
		}
		int per = 0;
		string tval = "1.0";
		for(int i=0;i<8;i++)
		{
			float temp = fValue*10;
			per = per-1;
			fValue = temp;
			if(temp>1)
			{
				tval = StrUtil::Float2Str1(fValue);
				if(tval.size()<=0)
				{
					tval = "1.0";	
				}
				break;
			}
		}
		val = tval+string("E")+StrUtil::Int2Str(per);
	}
	sOutBer = val;

	return true;
}

