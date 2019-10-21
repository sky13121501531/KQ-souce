
#include "QualityRealtimeQueryTask.h"
#include "Scheduler.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "TranslateUpXMLForDVBC.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/XmlParser.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DeviceAccess/TCPDeviceAccess.h"
#include "./TranslateXMLForDevice.h"
#include "../Alarm/AlarmMgr.h"
#include "../BusinessProcess/QualityCompensationMgr.h"
#include "ace/Synch.h"
#include "ace/OS.h"
#include "../DeviceAccess/FetchQualityLevel.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/BaseAnalyser.h"
#include "../DeviceAccess/QuaDevInfoTCPDeviceAccess.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/AppLog.h"
#include "../Communications/DeviceServer.h"
#include <vector>
#include <iostream>
using namespace std;
extern standardRFResult g_standardRFResult[100];


QualityRealtimeQueryTask::QualityRealtimeQueryTask() : DeviceRelatedTask()
{

}

QualityRealtimeQueryTask::QualityRealtimeQueryTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	//获取执行周期和频点信息
	XmlParser parser( strXML.c_str());
	pXMLNODE rootNode ;
	pXMLNODE childNode ;
	rootNode=parser.GetRootNode();
	childNode=parser.GetNodeFirstChild(rootNode);
	parser.GetAttrNode( childNode,"Periodicity",Periodicity );

	pXMLNODE tempChildNode = parser.GetNodeFirstChild( childNode );
	parser.GetAttrNode(tempChildNode,"Freq",Freq);
	if(this->GetVersion() == "3.0")
	{
		string strDevid;
		parser.GetAttrNode(tempChildNode,"DeviceID",strDevid);
		m_tunDeviceID = StrUtil::Str2Int(strDevid);
		DeviceID = m_tunDeviceID;
	}

	TaskScheduler = new Scheduler();
	TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());

	m_lastLevel = "0.00";
}

QualityRealtimeQueryTask::~QualityRealtimeQueryTask()
{

}

void QualityRealtimeQueryTask::Run(void)
{
	if(this->GetVersion() == "3.0")
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标任务执行 !\n",m_tunDeviceID));
	}
	else
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标任务执行 !\n",DeviceID));
	}
	bRun = true;
	SetRunning();

	bool pbRun = true;
	strDeviceXML=TranslateXMLForDevice::TranslateQualityRealTimeQuery(strStandardXML);

	string rtnxml;
	XmlParser parse(strDeviceXML.c_str());
	string strfreq;
	pXMLNODE frNode = parse.GetNodeFromPath("Msg/QualityQuery");
	parse.GetAttrNode(frNode,"Freq",strfreq);
	if(RetValue != SET_SUCCESS)
	{			
		SetFinised();//任务停止

		string msg = string("通道[") + StrUtil::Int2Str(DeviceID) + string("]实时指标任务执行失败，没有通道资源");
		//SYSMSGSENDER::instance()->SendMsg(msg,DVBType,VS_MSG_SYSALARM);
		if(this->GetVersion() == "3.0")
		{
			SendXML(TranslateUpXMLForDVBC::GetXmlHeader(this,"IndexQuery"));
		}
		else
		{
			SendXML(TranslateUpXMLForDVBC::GetXmlHeader(this,"GetIndexSet"));
		}
		return;
	}

	int idev = -1;
	TunerDev tdev;
	HTTPDeviceAccess* pDeviceAccess = NULL;
	string tunermonitorip,tunermonitorport;
	string strUsedType = "1";
	if (DVBType == DVBC||DVBType == CTTB||DVBType == DVBS||DVBType == RADIO||DVBType == AM)
	{
		//
		eDVBType edvtype;
		int itendev = -1;
		bool b30VUseThesameFreq = false;
		if(this->GetVersion() == "3.0")
		{
			strUsedType = "1";//3.0版也从已有设备中获取数据
			itendev = m_tunDeviceID;
			idev = itendev;
			//
			PROPMANAGER::instance()->GetTunerInfo(idev,DVBType,tdev);
			if(tdev.b_user&&tdev.s_freq == strfreq)
			{
				b30VUseThesameFreq = true;
			}
			edvtype = DVBType;
			//
			PROPMANAGER::instance()->SetTunerinfo(itendev,DVBC,strfreq,"","信道指标测量中...");
		}
		if((b30VUseThesameFreq)||((strUsedType == "0")&&(PROPMANAGER::instance()->GetTunerDevFromFreq(itendev,edvtype,"quality",strfreq,""))))
		{
			idev = itendev;
			if(edvtype != DVBType)
			{
				cout<<"实时指标任务奇怪-频点一样类型不一样:"<<strfreq<<"-"<<OSFunction::GetStrDVBType(DVBType)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
			}
			PROPMANAGER::instance()->GetTunerInfo(idev,edvtype,tdev);
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edvtype,idev,tunermonitorip,tunermonitorport);//tuner
			pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		}
		else if(idev != -1)
		{
			if(PROPMANAGER::instance()->GetTunerInfo(idev,DVBType,tdev))
			{
				PROPMANAGER::instance()->GetDevMonitorInfo(0,DVBType,idev,tunermonitorip,tunermonitorport);//tuner
				pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
				//
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
				{
					string  MsgID ;
					DBMANAGER::instance()->GetMsgID(MsgID);
					int msgid=StrUtil::Str2Int(MsgID);
					DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
					//
					std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
					std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
					string Surl = DeviceServerIP+":"+DeviceServerPort;
					//
					sFreqScanInfo freqscaninfo;
					PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
					//
					string strRet;
					string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
					cmd += "<Msg Version=\"1.0\" MsgID=\"";
					cmd += MsgID;
					cmd += "\" SrcUrl=\"http://";
					cmd += Surl;
					cmd += "\">";
					cmd += "<Type>SetDemodulateInfo</Type>";
					cmd += "<Data>";
					cmd += "<Channel>";
					cmd += 	tdev.s_chanindex;
					cmd += "</Channel>";
					cmd += "<Frequency>";
					cmd += 	strfreq;
					cmd += "</Frequency>";
					cmd += "<SymbolRate>";
					cmd += 	freqscaninfo.SymbolRate;
					cmd += "</SymbolRate>";
					cmd += "<Modulation>";
					cmd += 	"QAM64";
					cmd += "</Modulation>";
					cmd += "</Data>";
					cmd += "</Msg>";
					int pos = cmd.find("<Data>");
					string sendcmd = cmd.substr(pos,cmd.size()-pos);
					//判断是否是扰流节目，如果不是则不下发解扰指令
					if(sendcmd != tdev.s_ExistCmd)
					{
						PROPMANAGER::instance()->SetExistCmd(idev,DVBC,sendcmd);
						cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
						cout<<"发送命令:"<<cmd<<endl;
						pDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<"收到结果:"<<strRet<<endl;
						string sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						//
						if(strRet.find("200") != -1)
						{
							time_t curt = time(0);
							string DevRetval;
							while(((time(0)-curt)<30))
							{
								DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
								if(DevRetval.size()>0)
								{
									break;
								}
								else
								{
									Sleep(200);
								}
							}
						}
					}
				}
				else
				{
					string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
					string strRet;
					cmd+="chl=";
					cmd+=tdev.s_cardindex;
					cmd+="_";
					cmd+=tdev.s_chanindex;
					cmd+="&fre=";
					cmd+=strfreq;
					if(DVBType == DVBC || DVBType == CTTB)
					{
						if (DVBType == DVBC)
						{
							sFreqScanInfo freqscaninfo;
							PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
							//
							cmd+="&sym=";
							cmd+=freqscaninfo.SymbolRate;
							cmd+="&qam=2&ip=";
						}
						else if(DVBType == CTTB)
						{
							cmd+="&bandwith=8&spectrum=0&ip=";
						}
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
					}
					else if(DVBType == RADIO || DVBType == AM)
					{
						cmd+="&volume=50";
					}
					if(cmd != tdev.s_ExistCmd)
					{
						PROPMANAGER::instance()->SetExistCmd(idev,DVBC,cmd);
						if(pDeviceAccess->SendTaskMsg(cmd,strRet))
						{
							printf("000000\n");
							cout<<cmd<<"****ret:*****"<<strRet<<endl;
						}
						printf("11111\n");
						Sleep(100);
					}
				}
			}
		}
	}

	//
	string xml;
	xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>				\
		   <Msg DVBType=\"DVBC\">														\
		   <Return Type=\"QualityQuery\" Value=\"0\" Desc=\"成功\" Comment=\"\"/>		\
		   <QualityQueryReport  STD=\"ADTB-T\" Freq=\"500\" SymbolRate=\"6875\">		\
		   <QualityParam>																\
		   <QualityIndex Type=\"1\" Desc=\"Level\" Value=\"111\"/>";
	if(DVBType == DVBC || DVBType == CTTB ||DVBType == DVBS)
	{
		xml += "<QualityIndex Type=\"2\" Desc=\"BER\" Value=\"111\"/>					\
			   <QualityIndex Type=\"3\" Desc=\"MER\" Value=\"111\"/>";
	}
	if(DVBType == RADIO || DVBType == AM)
	{
		xml += "<QualityIndex Type=\"4\" Desc=\"CNR\" Value=\"111\"/>";
	}
		   //<QualityIndex Type=\"5\" Desc=\"Freq Offset\" Value=\"111\"/>
	if(DVBType == AM)
	{
		xml += "<QualityIndex Type=\"8\" Desc=\"AM-Modulation\" Value=\"0.00\"/>";	
	}
	if(DVBType == RADIO)
	{
		xml += "<QualityIndex Type=\"9\" Desc=\"FM-Modulation\" Value=\"31.00\"/>";
	}
	xml += "</QualityParam>																\
		    </QualityQueryReport>														\
		    </Msg>";
	//
	Sleep(3000);
	while (pbRun == true && IsRunning())
	{
		rtnxml="";
		//if(Periodicity=="Cycle" && REALTIMEQUALITYSENDER::instance()->HasClient(strMsgID)==false)//没有连接用户，任务退出
		//{
		//	break;
		//}
		string strStr = "30";
		string strNse = "2";
		string strMer = "17";
		string strBer = "0.0000002";
		string strEVM = "0";
		string strCNR = "0";
		string strAMFMModulation = "0";
		if ((DVBType == DVBC||DVBType == CTTB||DVBType == DVBS||DVBType == RADIO||DVBType == AM)&&(pDeviceAccess != NULL))
		{
			//
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				string  MsgID ;
				DBMANAGER::instance()->GetMsgID(MsgID);
				int msgid=StrUtil::Str2Int(MsgID);
				DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
				//
				std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
				std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
				string Surl = DeviceServerIP+":"+DeviceServerPort;
				//
				sFreqScanInfo freqscaninfo;
				PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
				//
				string strRet;
				string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
				cmd += "<Msg Version=\"1.0\" MsgID=\"";
				cmd += MsgID;
				cmd += "\" SrcUrl=\"http://";
				cmd += Surl;
				cmd += "\">";
				cmd += "<Type>GetDemodulateInfo</Type>";
				cmd += "<Data>";
				cmd += "<Channel>";
				cmd += 	tdev.s_chanindex;
				cmd += "</Channel>";
				cmd += "</Data>";
				cmd += "</Msg>";
				//
				cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
				cout<<"发送命令:"<<cmd<<endl;
				pDeviceAccess->SendTaskMsg(cmd,strRet);
				cout<<"收到结果:"<<strRet<<endl;
				string sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
				APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
				APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
				APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
				//
				if(strRet.find("200") != -1)
				{
					time_t curt = time(0);
					string DevRetval;
					while(((time(0)-curt)<30))
					{
						DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
						if(DevRetval.size()>0)
						{
							break;
						}
						else
						{
							Sleep(200);
						}
					}
					if(DevRetval.find(strfreq) == -1)
					{
						Sleep(200);
						continue;
					}
					if(idev != -1)
					{
						for(int ig = 0;ig<20;ig++)//确保获取到真实数据
						{
							strStr = StrUtil::Int2Str(g_standardRFResult[idev].ucharLevel);
							strMer = StrUtil::Float2Str(float(g_standardRFResult[idev].ushortMer)/10.0f);
							strBer = StrUtil::Int2Str(g_standardRFResult[idev].uintBer);
							strEVM = StrUtil::Float2Str(float(g_standardRFResult[idev].ushortEVM)/10.0f);
							strCNR = StrUtil::Float2Str(float(g_standardRFResult[idev].ushortCNR)/10.0f);
							if(g_standardRFResult[idev].ucharLevel > 5)
							{
								break;
							}
							else
							{
								Sleep(500);
							}
						}
					}
				}
			}
			else
			{
				string cmd = "POST /io/info  HTTP/1.1  \r\n\r\n";
				string strRet;
				cmd+="chl=";
				cmd+=tdev.s_cardindex;
				cmd+="_";
				cmd+=tdev.s_chanindex;
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标命令: %s\n",m_tunDeviceID,cmd.c_str()));
				pDeviceAccess->SendTaskMsg(cmd,strRet);
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标返回: %s\n",m_tunDeviceID,strRet.c_str()));
				cout<<cmd<<"****ret:*****"<<strRet<<endl;
				//
				if(strRet.size() <= 0)
				{
					cout<<"QualityRealtimeQueryTask命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
					continue;
				}
				int pos = strRet.find("<html>");
				string strtem = strRet;
				if(pos >= 0)
				{
					strtem = strRet.substr(pos,strRet.size()-pos);
				}
				else
				{
					cout<<"QualityRealtimeQueryTask没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
					continue;
				}
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
							cout<<"QualityRealtimeQueryTask err code:"<<er<<endl;
							break;
						}
					}
				}
			}
		}
		string freq = strfreq;
		XmlParser rtnparser(xml.c_str());
		pXMLNODE returtnNode = rtnparser.GetNodeFromPath("Msg/Return");	
		pXMLNODE ReportNode = rtnparser.GetNodeFromPath("Msg/QualityQueryReport");
		rtnparser.SetAttrNode("Freq",strfreq,ReportNode);
		pXMLNODE paramNode = rtnparser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");
		pXMLNODELIST paramList = rtnparser.GetNodeList(paramNode);
		int count=rtnparser.GetChildCount(paramNode);
		for(int k=0;k<count;k++)
		{
			string type,desc,val;
			pXMLNODE paramnode = rtnparser.GetNextNode(paramList);	
			rtnparser.GetAttrNode(paramnode,"Type",type);
			rtnparser.GetAttrNode(paramnode,"Desc",desc);
			if(type=="1")
			{
				val = strStr;
				if(StrUtil::Str2Int(val) < 0 && (DVBType == CTTB))
				{
					int temval = StrUtil::Str2Int(val) + 107;
					val = StrUtil::Int2Str(temval);
				}
				rtnparser.SetAttrNode("Value",val,paramnode);
			}
			else if(type=="2")
			{
				rtnparser.SetAttrNode("Value",strBer,paramnode);
				val = strBer;
			}
			else if(type=="3")
			{
				if(GetVersion() == "3.0")
				{
					if (PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")
					{
						int tem = rand();
						int vl = tem%10;
						float fv = vl/10.0f;
						val = StrUtil::Float2Str(StrUtil::Str2Float(strNse)-6.8-fv);
						rtnparser.SetAttrNode("Value",val,paramnode);
					}
					else
					{
						rtnparser.SetAttrNode("Value",strMer,paramnode);
						val = strMer;
					}
				}
				else
				{
					val = StrUtil::Float2Str(StrUtil::Str2Float(strNse)-6.4);//C/N = S/N-6.4
					rtnparser.SetAttrNode("Value",val,paramnode);
				}
			}
			else if(type=="4")
			{
				rtnparser.SetAttrNode("Value",strNse,paramnode);
				val = strNse;
			}
			else if(type=="5")
			{
			}
			else if(type=="8"||type=="9")
			{
				rtnparser.SetAttrNode("Value",strAMFMModulation,paramnode);
				val = strAMFMModulation;
			}

			if(this->GetVersion() == "3.0")
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标:%s\t%s\t%s\t\n",m_tunDeviceID,type.c_str(),desc.c_str(),val.c_str()));
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]指标:%s\t%s\t%s\t\n",DeviceID,type.c_str(),desc.c_str(),val.c_str()));
			}
		}
		rtnparser.SaveToString(rtnxml);
		//
		std::vector<sCheckParam> alarmVec;//报警信息处理
		std::vector<sCheckParam>::iterator itr;
		ReadyForAlarm(rtnxml,alarmVec);
		for(itr=alarmVec.begin();itr!=alarmVec.end();itr++)
		{
			ALARMMGR::instance()->CheckAlarm(*itr,false);
		}
		if(strProtocol=="HTTP")
		{
			RetValue = RUN_SUCCESS;
			SendXML(TranslateUpXML::TranslateQualityRealtimeQuery(this,rtnxml));
		}
		else if(strProtocol=="TCP")
		{
			RetValue = RUN_SUCCESS;
			
			unsigned char TcpData[1024];
			memset(TcpData,0,1024);

			int len = 0;
			switch(DVBType)
			{
			case CTTB:
				{
					len=CreateTcpDataForCTTB(rtnxml,&TcpData[10],1024-10);
					break;
				}
			case ATV:
			case CTV:
				{
					len=CreateTcpDataForATV(rtnxml,&TcpData[10],1014-10 );
					break;
				}
			case RADIO:
			case AM:
				{
					len=CreateTcpDataForRadio(rtnxml,&TcpData[10],1024-10 );
					break;
				}
			}
			
			int msgid=StrUtil::Str2Int(GetMsgID());

			memcpy(&TcpData[0],&msgid,sizeof(msgid));
			TcpData[4] = 0;
			TcpData[5] = 4;
			

			memcpy(&TcpData[6],&len,sizeof(len));
			len+=10;
			ACE_Message_Block *MBTcpData = new ACE_Message_Block(len);

			memcpy(MBTcpData->wr_ptr(),TcpData,len);
			MBTcpData->wr_ptr(len);

			//指标数据放入到指标发送线程的队列
			//ACE_Time_Value OutTime(ACE_OS::time(0)+1);
			//if (-1 == REALTIMEQUALITYSENDER::instance()->putq(MBTcpData,&OutTime))
			//{
			//	MBTcpData->release();
			//}
		}
		
		if(Periodicity=="Once")
		{
			break;
		}
		OSFunction::Sleep(3);
	}

	pbRun = false;
	if(pDeviceAccess != NULL)
	{
		delete pDeviceAccess;
	}
	//
	if(this->GetVersion() == "3.0")
	{
		PROPMANAGER::instance()->FreeSetTunerDev(m_tunDeviceID,DVBC,strfreq,"");
	}
	//
	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

	SetFinised();
	if(this->GetVersion() == "3.0")
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标任务停止 !\n",m_tunDeviceID));
	}
	else
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时指标任务停止 !\n",DeviceID));
	}
}
string QualityRealtimeQueryTask::GetTaskName()
{
	return std::string("实时指标任务");
}
std::string QualityRealtimeQueryTask::GetObjectName()
{
	return std::string("QualityRealtimeQueryTask");
}

int QualityRealtimeQueryTask::CreateTcpDataForCTTB(std::string  strRtnXML,unsigned char* TcpData,int size_n)
{
	char buffer[54] = {};
	memset(buffer,0,54*sizeof(char));
	int index = 0;

	memcpy(&buffer[0],"TRQU",4*sizeof(char));
	index = index + 4*sizeof(char);

	time_t t64 = time(0);
	memcpy(&buffer[index],&t64,8*sizeof(char));
	index = index + 8*sizeof(char);

	int intFreq = atoi(Freq.c_str());
	memcpy(&buffer[index],&intFreq,4*sizeof(char));
	index = index + 4*sizeof(char);

	string Type,Value;
	XmlParser rtnparser;
	rtnparser.Set_xml(strRtnXML);

	pXMLNODE queryParamNode = rtnparser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");
	pXMLNODELIST queryIndexList = rtnparser.GetNodeList( queryParamNode );
	int count = rtnparser.GetChildCount( queryParamNode );

	memcpy(&buffer[index],&count,2*sizeof(char));
	index = index + 2*sizeof(char);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE indexNode = rtnparser.GetNextNode( queryIndexList);
		
		//设置QualityIndex 属性
		rtnparser.GetAttrNode( indexNode,"Type",Type );
		rtnparser.GetAttrNode( indexNode,"Value",Value );
		float FValue = StrUtil::Str2Float(Value);
		if(Type == "1")
		{
			if(FValue < 0.1)
			{
				FValue = FValue + 107;
			}
		}
		int IntValue = 	int(FValue*1000);
		string add;
		if(index>size_n-5)
			break;
		switch(atoi(Type.c_str()))
		{
		case 1:
			QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(this->GetDVBType(),StrUtil::Int2Str(DeviceID),Type,"","",add);
			IntValue+=StrUtil::Str2Int(add);
			buffer[index] = 1;
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		case 2:
			buffer[index] = 3;		//特殊处理地面数字的指标类型
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		case 3:
			buffer[index] = 2;		//特殊处理地面数字的指标类型
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		case 4:
			buffer[index] = 4;
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		case 5:	
			buffer[index] = 5;
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		case 6:
			buffer[index] = 6;
			index = index + 1*sizeof(char);
			memcpy(&buffer[index],&IntValue,4*sizeof(char));
			index = index + 4*sizeof(char);
			break;
		}
	}
	int rtnlen  = sizeof(buffer);
	memcpy(TcpData,buffer,rtnlen);

	rtnlen -= 6;

    return rtnlen;
}

int QualityRealtimeQueryTask::CreateTcpDataForATV(std::string strRtnXML,unsigned char* TcpData,int size_n )
{
	vector<QualityDesc> QualityParamVec;
	float mFreq=StrUtil::Str2Float(Freq);
	
	bool retVal = false;
	int TcpDataLen = 0;
	int len = 0,templen = 0;
	templen = len;
	unsigned char ch[128];
	memset(ch,0,128*sizeof(char));

	XmlParser parser;
	parser.Set_xml(strRtnXML);

	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE SrcparamNode = parser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");
	pXMLNODELIST paramList = parser.GetNodeList(SrcparamNode);
	int count = paramList->Size();
	for(int k=0;k<count;k++)
	{
		std::string Type,Desc,Value;
		QualityDesc TempDesc;
		pXMLNODE childNode = parser.GetNextNode(paramList);
		parser.GetAttrNode(childNode,"Type",Type);
		parser.GetAttrNode(childNode,"Desc",Desc);
		parser.GetAttrNode(childNode,"Value",Value);
		if (Type == "1")
		{
			if (Value != "0.00")
			{
				m_lastLevel = Value;
			}
			else
			{
				Value = m_lastLevel;
			}
		}

		TempDesc.Desc=Desc;
		TempDesc.Type=Type;
		TempDesc.Value=Value;
		QualityParamVec.push_back(TempDesc);
	}
	try
	{
		//保存data_type_tag	接口类型。电视实时指标测量上报是”TRQU”。
		memcpy(&ch[templen],"TRQU",4*sizeof(char));
		templen = templen + 4*sizeof(char);

		//保存report_time	上报时间, 从1970年1月1日0时0分0秒到该时间点所经过的秒
		time_t t64 = time(0);
		memcpy(&ch[templen],&t64,8*sizeof(char));
		templen = templen + 8*sizeof(char);
		float ff = 1000*mFreq;

		//保存freq	测量频率。单位为kHz。
		int ff1 = (int)ff;
		memcpy(&ch[templen],&ff1,4*sizeof(char));
		templen = templen + 4*sizeof(char);

		//保存data_length	数据长度
		short datalen = (short)QualityParamVec.size();
		memcpy(&ch[templen],&datalen,2*sizeof(char));
		templen = templen + 2*sizeof(char);

		BYTE nCount =(BYTE)QualityParamVec.size();

		int	ff2[7] = {0,0,0,0,0,0,0};
		for(BYTE i = 0; i < nCount; i++)
		{
			if(templen>size_n-5)
				break;
			switch(StrUtil::Str2Int(QualityParamVec[i].Type))
			{
			case 1:
				ff = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[0] =(int)ff;
				//cout<<"图像电平:"<<ff2[0]<<endl;
				ch[templen] = 1;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[0],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 2:
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[1] =(int)ff;
				//cout<<"伴音电平:"<<ff2[1]<<endl;
				ch[templen] = 2;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[1],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 3:

				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[2] =(int)ff;
				//cout<<"图像伴音电平差:"<<ff2[2]<<endl;
				ch[templen] = 3;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[2],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 4:
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[3] =(int)ff;
				//cout<<"载噪比:"<<ff2[3]<<endl;
				ch[templen] = 4;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[3],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 5:
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[4] =(int)ff;
				//cout<<"载频频偏:"<<ff2[4]<<endl;
				ch[templen] = 5;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[4],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 6:
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[5] =(int)ff;
				//cout<<"斜率:"<<ff1<<endl;
				ch[templen] = 6;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[5],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 7:
				//mQualityParamVec[6].fReturnVal = 0.00;    //误码率
				break;
			default:
				break;
			}
		}

		memcpy(TcpData,ch,templen);
		TcpDataLen = templen;
		retVal = true;

	}
	catch(...)
	{
		string msg = string("通道[") + StrUtil::Int2Str(DeviceID) + string("]实时指标测量出错");
		//SYSMSGSENDER::instance()->SendMsg(msg,DVBType,VS_MSG_SYSALARM);
		return -1;
	}
	return TcpDataLen;
}


int QualityRealtimeQueryTask::CreateTcpDataForRadio(std::string strRtnXML,unsigned char* TcpData,int size_n )
{
	vector<QualityDesc> QualityParamVec;
	float mFreq=StrUtil::Str2Float(Freq);

	bool retVal = false;
	int TcpDataLen = 0;
	int len = 0,templen = 0;
	templen = len;
	unsigned char ch[128];
	memset(ch,0,128*sizeof(char));

	XmlParser parser;
	parser.Set_xml(strRtnXML);

	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE SrcparamNode = parser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");
	pXMLNODELIST paramList = parser.GetNodeList(SrcparamNode);
	int count = paramList->Size();
	for(int k=0;k<count;k++)
	{
		std::string Type,Desc,Value;
		QualityDesc TempDesc;
		pXMLNODE childNode = parser.GetNextNode(paramList);
		parser.GetAttrNode(childNode,"Type",Type);
		parser.GetAttrNode(childNode,"Desc",Desc);
		parser.GetAttrNode(childNode,"Value",Value);
		if (Type == "1")
		{
			if (Value != "0.00")
			{
				m_lastLevel = Value;
			}
			else
			{
				Value = m_lastLevel;
			}
		}
		TempDesc.Desc=Desc;
		TempDesc.Type=Type;
		TempDesc.Value=Value;
		QualityParamVec.push_back(TempDesc);
	}
	try
	{
		//保存data_type_tag	接口类型。电视实时指标测量上报是”TRQU”。
		memcpy(&ch[templen],"TRQU",4*sizeof(char));
		templen = templen + 4*sizeof(char);

		//保存report_time	上报时间, 从1970年1月1日0时0分0秒到该时间点所经过的秒
		time_t t64 = time(0);
		memcpy(&ch[templen],&t64,8*sizeof(char));
		templen = templen + 8*sizeof(char);
		float ff = 1000*mFreq;

		//保存freq	测量频率。单位为kHz。
		int ff1 = (int)ff;
		memcpy(&ch[templen],&ff1,4*sizeof(char));
		templen = templen + 4*sizeof(char);

		//保存data_length	数据长度
		short datalen = (short)QualityParamVec.size();
		memcpy(&ch[templen],&datalen,2*sizeof(char));
		templen = templen + 2*sizeof(char);

		BYTE nCount =(BYTE)QualityParamVec.size();

		int	ff2[7] = {0,0,0,0,0,0,0};
		for(BYTE i = 0; i < nCount; i++)
		{
			if(templen>size_n-5)
				break;
			switch(StrUtil::Str2Int(QualityParamVec[i].Type))
			{
			case 1:                                     //音频信号场强
				{
					ff = StrUtil::Str2Float(QualityParamVec[i].Value);
					string add;
					QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(this->GetDVBType(),StrUtil::Int2Str(DeviceID),string("1"),"","",add);
					ff+=StrUtil::Str2Int(add);
					ff2[0] =(int)ff;
					ch[templen] = 1;
					templen = templen + 1*sizeof(char);
					memcpy(&ch[templen],&ff2[0],4*sizeof(char));
					templen = templen + 4*sizeof(char);
					break;
				}
			case 2:                                       //调制度
				{
					ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
					string add;
					QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(this->GetDVBType(),StrUtil::Int2Str(DeviceID),string("2"),"","",add);
					ff+=StrUtil::Str2Int(add);
					ff2[1] =(int)ff;
					//cout<<"伴音电平:"<<ff2[1]<<endl;
					ch[templen] = 2;
					templen = templen + 1*sizeof(char);
					memcpy(&ch[templen],&ff2[1],4*sizeof(char));
					templen = templen + 4*sizeof(char);
					break;
				}
			case 3:                                       //频偏

				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[2] =(int)ff;
				//cout<<"图像伴音电平差:"<<ff2[2]<<endl;
				ch[templen] = 3;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[2],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 4:                                        //音频信号主谐波频率 信号载波频率 (音频信号主谐波频率?)
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[3] =(int)ff;
				//cout<<"载噪比:"<<ff2[3]<<endl;
				ch[templen] = 4;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[3],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 5:                                       //谐波失真
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[4] =(int)ff;
				//cout<<"载频频偏:"<<ff2[4]<<endl;
				ch[templen] = 5;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[4],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 6:                                        //信噪比
				ff  = StrUtil::Str2Float(QualityParamVec[i].Value);
				ff2[5] =(int)ff;
				ch[templen] = 6;
				templen = templen + 1*sizeof(char);
				memcpy(&ch[templen],&ff2[5],4*sizeof(char));
				templen = templen + 4*sizeof(char);
				break;
			case 7:
				//mQualityParamVec[6].fReturnVal = 0.00;    //误码率
				break;
			default:
				break;
			}
		}

		memcpy(TcpData,ch,templen);
		TcpDataLen = templen;
		retVal = true;

	}
	catch(...)
	{
		string msg = string("通道[") + StrUtil::Int2Str(DeviceID) + string("]实时指标测量出错");
		//SYSMSGSENDER::instance()->SendMsg(msg,DVBType,VS_MSG_SYSALARM);
		return -1;
	}
	return TcpDataLen;
}

bool QualityRealtimeQueryTask::ReadyForAlarm(std::string strXML,std::vector<sCheckParam>& alarmVec)
{
	//声明解析类的对象
	XmlParser devParser;
	devParser.Set_xml(strXML);
	string freq,STD,SymbolRate,type,desc,val;
	pXMLNODE devRootNode= devParser.GetRootNode();

	pXMLNODE ReportNode = devParser.GetNodeFromPath("Msg/QualityQueryReport");
	devParser.GetAttrNode(ReportNode,"Freq",freq);
	devParser.GetAttrNode(ReportNode,"STD",STD);
	devParser.GetAttrNode(ReportNode,"SymbolRate",SymbolRate);

	pXMLNODE paramNode = devParser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");

	pXMLNODELIST paramList = devParser.GetNodeList(paramNode);

	int count=devParser.GetChildCount(paramNode);

	for(int k=0;k<count;k++)
	{
		
		string add;
		pXMLNODE paramnode = devParser.GetNextNode(paramList);
	
		devParser.GetAttrNode(paramnode,"Type",type);
		devParser.GetAttrNode(paramnode,"Desc",desc);
		devParser.GetAttrNode(paramnode,"Value",val);
		int logindex=0;
		PROPMANAGER::instance()->GetDeviceLogIndex(this->GetDeviceID(),logindex);
		QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(this->GetDVBType(),"0",type,"","",add);
		int val_int = StrUtil::Str2Int(val)+StrUtil::Str2Int(add);
		sCheckParam param;
		param.DVBType = this->GetDVBType();
		param.AlarmType = ALARM_FREQ;
		param.TypeID = type;
		param.TypeDesc = desc;
		param.STD = STD;
		param.Freq = freq;
		param.SymbolRate = SymbolRate;
		param.ChannelID = this->GetChannelID();
		if(DVBType==ATV||DVBType==RADIO||DVBType==AM ||DVBType==CTV)
		{
			param.TypedValue = StrUtil::Float2Str((float)val_int/1000);
		}
		else if(DVBType==DVBC||DVBType==CTTB||DVBType==DVBS)
		{
			param.TypedValue = StrUtil::Float2Str((float)val_int);
		}
		param.DeviceID = StrUtil::Int2Str(DeviceID);
		param.CheckTime=time(0);

		alarmVec.push_back(param);
	}
	return true;
}

//用其它设备(场强卡,频谱仪) 获取场强信息
bool QualityRealtimeQueryTask::GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel)
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
				string startFreq = StrUtil::Long2Str(l_freq-4000);
				string endFreq = StrUtil::Long2Str(l_freq+4000);
				bret = pDeviceAccess->SendSpecDevInfoCmd(startFreq, endFreq, 1, s_level);
				if(bret)
				{
					string strRet = s_level;
					strRet = strRet.substr(5, strRet.length());//aaa2:
					string str1 = "";
					int ipos1 = -1;
					int ipos2 = -1;
					vector<string> vLever;
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
							//string sFreq = str1.substr(0, ipos2);
							string sFreq = str1.substr(0, 5);//698.0
							string slever = str1.substr(ipos2+1, str1.length());
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
									vLever.push_back(slever);
									vOldFreqLever.clear();
								}
							}
							vOldFreqLever.push_back(slever);
							//	
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
										vLever.push_back(slever);
										vOldFreqLever.clear();
									}
								}
								vOldFreqLever.push_back(slever);
							}

							break;
						}
					}

					float isize = vLever.size();
					s_level = "";
					if (isize > 0)
					{
						vector<string>::iterator ite_map = vLever.begin();
						double tempval=0;
						for(ite_map; ite_map!= vLever.end(); ite_map++)
						{
							tempval+=pow(10.0f,StrUtil::Str2Float((*ite_map).c_str())/10.0f);
						}
						float flevel = 10.0f*log10(tempval);
						s_level = StrUtil::Float2Str(flevel);
						PROPMANAGER::instance()->AdjustingFreqLevel(strFreq, s_level);
					}
					//////////////////////////////////////////////////////////////////////////
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
