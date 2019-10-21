///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordSetTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-01
// 内容描述：自动录像设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "RecordSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "RecordTask.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "./QualityTask.h"
#include "../Foundation/PropManager.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/StrUtil.h"
#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/AppLog.h"
#include "../Foundation/StrUtil.h"
#include "../DeviceMgr/DeviceManager.h"

#endif
//ACE_Thread_Mutex g_RecordSetTaskMutex;
using namespace std;

RecordSetTask::RecordSetTask() : DeviceIndependentTask()
{

}

RecordSetTask::RecordSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}

RecordSetTask::~RecordSetTask()
{

}
void RecordSetTask::Run(void)
{
	bRun = true;
	SetRunning();
	
	std::string Action;
	std::vector<std::string> TaskXMLVec;
	std::vector<XMLTask*> vecTask;
	int retSet = GetRetChannelSetSouce(strStandardXML);
	bool moreFlag = false;
	if(retSet==-1)
	{
		RetValue = RECODESOUCE_OUTOFRAND;
		SendXML(TranslateUpXML::TranslateRecord(this,vecTask));//发送到前端
		return;
	}
	else if(retSet==1)
	{
		moreFlag = true;
	}
#ifdef ZONG_JU_ZHAO_BIAO
	std::vector<sRecordSetInfo> vecRecSetInfo;	//存储任务信息以及任务是否正常执行起来
#endif

	RetValue = RUN_SUCCESS;
	TranslateDownXML::TranslateRecordTask(strStandardXML,TaskXMLVec);
	//BUSINESSLAYOUTMGR::instance()->DelRecInfo(GetDVBType(),"0");	//删除所有其他自动录像
	if(!moreFlag)
		PROPMANAGER::instance()->FreeRecTunerCoder(GetDVBType());
#ifdef ZONG_JU_ZHAO_BIAO
	std::string strFreq, strServiceId, strAudioPid, strVideoPid, strChanId;
	std::vector<std::string>  vctXML;
	bool rnt = DBMANAGER::instance()->QueryTask(GetDVBType(),vctXML);
#endif
	std::string strTdevid = "";
	//
	bool isMoreVideo = false,isLvideo = false;
	vector<std::string> vecServiceID;
	//
	bool bEffective = TestTaskIsEffective(TaskXMLVec);
	for (size_t i=0; i< TaskXMLVec.size();i++)
	{
		XmlParser psr;
		int deviceid = -1;
		psr.Set_xml(TaskXMLVec[i]);
		pXMLNODE recordNode;
		if(TaskXMLVec[i].find("AutoRecord")!=string::npos)
		{
			string MoID = "";
			recordNode=psr.GetNodeFromPath("Msg/AutoRecord/Record");
#ifdef ZONG_JU_ZHAO_BIAO
			//******************************
			//总局入网：自动录像删除时，添加获取频道信息代码
			psr.GetAttrNode(recordNode, "Freq", strFreq);
			psr.GetAttrNode(recordNode, "ServiceID", strServiceId);
			psr.GetAttrNode(recordNode, "VideoPID", strVideoPid);
			psr.GetAttrNode(recordNode, "AudioPID", strAudioPid);
			psr.GetAttrNode(recordNode, "CModuleID", MoID);
			//区分删除TASK任务
			if(MoID.size()>0)
			{
				if(MoID=="4")
				{
					isMoreVideo = true;
				}
				else
				{
					isLvideo = true;
				}
			}
			//******************************
#endif
		}
		else
		{
			recordNode=psr.GetNodeFromPath("Msg/TaskRecord/Record");
		}
		psr.GetAttrNode(recordNode,"Action",Action);
		psr.GetAttrNode(recordNode,"TaskID",TaskID);
		if((GetDVBType() == DVBC)&&(GetVersion() == "3.0"))
		{
			psr.GetAttrNode(recordNode,"CDeviceID",deviceid);
			psr.GetAttrNode(recordNode,"TDeviceID",strTdevid);  
			DeviceID = deviceid;
		}
		else
		{
			psr.GetAttrNode(recordNode,"DeviceID",deviceid);
			DeviceID = deviceid;
		}
		string IndexSet = "",Step = "";
		psr.GetAttrNode(recordNode,"IndexSet",IndexSet);//指标报警标志
		psr.GetAttrNode(recordNode,"Step",Step);
		
		if(Action=="Set"||Action=="set")
		{
			RecordTask* recordtask = new RecordTask(TaskXMLVec[i]);

			//如果向数据库中添加任务失败了，就认为没有设置失败，跳出循环
			bool ret = BUSINESSLAYOUTMGR::instance()->AddRecTask(recordtask);
			vecTask.push_back(recordtask);
#ifdef ZONG_JU_ZHAO_BIAO
			sRecordSetInfo recSetInfo;
			recSetInfo.strTaskXml = TaskXMLVec[i];
			recSetInfo.bSetAction = ret;
			recSetInfo.nDeviceId = recordtask->GetDeviceID();
			vecRecSetInfo.push_back( recSetInfo );
#endif
			OSFunction::Sleep(0,5);
		 }
//		else if(Action=="Del"||Action=="del")
//		{	
//#ifdef ZONG_JU_ZHAO_BIAO
//			deviceid = PROPMANAGER::instance()->FreeDev(strFreq,strServiceId);
//			//**********************************
//#endif
//			if(deviceid != -1 && !BUSINESSLAYOUTMGR::instance()->DelTask(deviceid,TaskID))
//			{
//				RetValue=DATABASEERROR;
//			}
//			else if(deviceid == 0 && !BUSINESSLAYOUTMGR::instance()->DelTask(TaskID))
//			{
//				RetValue=DATABASEERROR;
//			}
//			RetValue=RUN_SUCCESS;
//			if (strFreq.size() > 0)
//			{
//				if(!(BUSINESSLAYOUTMGR::instance()->IsExistRecTaskByFreq(GetDVBType(),strFreq)))
//				{
//					BUSINESSLAYOUTMGR::instance()->DelTask(strFreq);//以频点作为任务ID,时间是当前时间的一年
//				}
//			}
//			//********************删除录像指标**********************
//		}
	}
	for (int i=0;i<vctXML.size();i++)
	{
		XmlParser moreVideopsr;
		moreVideopsr.Set_xml(vctXML[i]);
		pXMLNODE MorerecordNode = moreVideopsr.GetNodeFromPath("Msg/AutoRecord/Record");
		string isMoreServiceid = "";
		moreVideopsr.GetAttrNode(MorerecordNode, "ServiceID", isMoreServiceid);//DBMANAGER::instance()->DeleteTask(GetDVBType(),vctXML[i]);
		if(isMoreVideo && isLvideo)
		{
			DBMANAGER::instance()->DeleteTask(GetDVBType(),vctXML[i]);
		}
		if(isMoreVideo && !isLvideo)//仅有多画
		{
			if(isMoreServiceid.find(";")!=-1)
			{
				DBMANAGER::instance()->DeleteTask(GetDVBType(),vctXML[i]);
			}
		}
		else if(!isMoreVideo && isLvideo)//仅有普通
		{
			if(isMoreServiceid.find(";")==-1)
			{
				DBMANAGER::instance()->DeleteTask(GetDVBType(),vctXML[i]);
			}	
		}
	}
#ifdef ZONG_JU_ZHAO_BIAO
	//如果所有录像任务都设置失败，那么认为这个设置任务失败.
	if(Action=="Set"||Action=="set")
	{
		bool bSetRetValue = false;
		for (int n=0; n<vecRecSetInfo.size(); n++)
		{
			if (vecRecSetInfo[n].bSetAction)
			{
				bSetRetValue = true;
				break;
			}
		}
		if (!bSetRetValue)
		{
			this->SetRetValue( RUN_FAILED );
		}
	}
	if (Action == "Set" || Action == "set")	
	{
		SendXML(TranslateUpXML::TranslateRecord(this,vecRecSetInfo));//发送到前端
	}
	else
	{
#endif
		SendXML(TranslateUpXML::TranslateRecord(this,vecTask));//发送到前端
	}
	
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像任务停止 !\n",DeviceID));
}

std::string RecordSetTask::GetObjectName()
{
	return std::string("RecordSetTask");
}
string RecordSetTask::GetTaskName()
{
	return "设置录像任务";
}
bool RecordSetTask::CreateQualityTask(string freq,string CheckInterval,string TunerIndex)
{
	//***************************************
	/*<?xml version="1.0" encoding="GB2312" standalone="yes" ?> 
	<Msg Version="1" MsgID="2" DVBType="DVBC" TaskType="QualityTaskSet" Protocol="" DateTime="2006-08-17 15:30:00" SrcCode="110000X01" DstCode="110000N01" SrcURL="" Priority="1">
	<QualityTaskSet TaskID="100" Action="Set">
	<TaskParm>
	<QualityQueryTask STD="" Freq="698000" SymbolRate="" /> 
	</TaskParm>
	<QualityParam>
	<QualityIndex Type="0" Desc="ALL" /> 
	<QualityIndex Type="1" Desc="Level" /> 
	<QualityIndex Type="2" Desc="BER" /> 
	<QualityIndex Type="4" Desc="MER" /> 
	</QualityParam>
	<TimeParam>
	<TimeIndex DayOfWeek="" StartTime="" EndTime="" StartDateTime="2006-09-03 14:08:33" EndDateTime="2006-09-04 16:08:33" CheckInterval="00:00:10" /> 
	</TimeParam>
	</QualityTaskSet>
	</Msg>*/
	//***************************************
	bool ret = true;
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	xml+="<Msg><QualityTaskSet TaskID=\"";
	xml+=freq;
	xml+="\" TunerIndex=\"";
	xml+=TunerIndex;
	xml+="\" Action=\"Set\"><TaskParm><QualityQueryTask Freq=\"";
	xml+=freq;
	xml+="\" STD=\"\" SymbolRate=\"\"/></TaskParm><QualityParam><QualityIndex Type=\"0\" Desc=\"ALL\"/></QualityParam><TimeParam><TimeIndex/></TimeParam></QualityTaskSet></Msg>";
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("3.0"),root );//版本
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	string TypeDvb;
	if(GetDVBType()==CTTB)
	{
		TypeDvb="CTTB";
	}
	else if(GetDVBType()==DVBC)
	{
		TypeDvb="DVBC";
	}
	else if(GetDVBType()==RADIO)
	{
		TypeDvb="RADIO";
	}
	else if(GetDVBType()==AM)
	{
		TypeDvb="AM";
	}
	else if(GetDVBType()==DVBS)
	{
		TypeDvb="DVBS";
	}
	else
	{
		TypeDvb="";
	}
	parser.SetAttrNode("DVBType",TypeDvb,root);               //设置Msg节点的DVBType属性值
	parser.SetAttrNode("TaskType",string("QualityTask"),root);    //设置Msg节点的TaskType属性值
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(GetDVBType()),root );//本机标识，可通过接口获得
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(GetDVBType()),root );//目标机器标识
	parser.SetAttrNode( "Priority",string("1"),root );//回复的消息id
	//
	pXMLNODE SReportTimeNode = parser.GetNodeFromPath("Msg/QualityTaskSet/TimeParam/TimeIndex");
	parser.SetAttrNode( "StartDateTime",strDateTime,SReportTimeNode );
	parser.SetAttrNode( "EndDateTime",TimeUtil::DateTimeToStr(time(0)+365*24*3600),SReportTimeNode );
	parser.SetAttrNode( "CheckInterval",CheckInterval,SReportTimeNode );
	string xmltask;
	parser.SaveToString(xmltask);
	//
	std::vector<std::string> strVecTask;
	TranslateDownXML::TranslateQualityTask(xmltask,strVecTask);
	for (size_t i=0; i!= strVecTask.size();++i)
	{
		QualityTask* qualitytask = new QualityTask(strVecTask[i]);//新的指标任务
		//添加新任务
		/*if( BUSINESSLAYOUTMGR::instance()->AddTask(qualitytask) == true )
		{

		}*/
	}
	return ret;
}

bool RecordSetTask::TestTaskIsEffective(std::vector<std::string> vecTaskXml)
{
	map<std::string, vector<std::string>> mapFreqServiceID;
	for (size_t i=0; i< vecTaskXml.size();++i)
	{
		XmlParser psr;
		psr.Set_xml(vecTaskXml[i]);
		pXMLNODE recordNode;
		if(vecTaskXml[i].find("AutoRecord")!=string::npos)
		{
			recordNode=psr.GetNodeFromPath("Msg/AutoRecord/Record");
			std::string strFreq, strServiceId, Action;
			psr.GetAttrNode(recordNode, "Freq", strFreq);
			psr.GetAttrNode(recordNode, "ServiceID", strServiceId);
			psr.GetAttrNode(recordNode,"Action",Action);
			if (Action == "Del" || Action == "del")
				break;
			vector<std::string> vecServiceID;
			if (mapFreqServiceID.size() > 0)
			{
				map<std::string, vector<std::string>>::iterator ite = mapFreqServiceID.find(strFreq);
				if (ite != mapFreqServiceID.end())
				{
					vecServiceID = (*ite).second;
					mapFreqServiceID.erase(ite);
				}
			}
			vecServiceID.push_back(strServiceId);
			mapFreqServiceID.insert(make_pair(strFreq, vecServiceID));
		}
	}
	if (mapFreqServiceID.size() == 0)
		return false;
	int iUnUsedCoderNum = PROPMANAGER::instance()->GetUnUsedCoderDevNum();
	if (iUnUsedCoderNum < vecTaskXml.size())
		return false;
	bool bRet = PROPMANAGER::instance()->IsHadUnUsedTunerOrCoder(this->GetDVBType(), mapFreqServiceID);

	return bRet;
}
int RecordSetTask::GetRetChannelSetSouce(std::string strXML)
{
	sChannelSetSouceInfo sChannelInfo;
	vector<sChannelSetSouceInfo> vChannelInfo;
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strXML.c_str());
	XmlParser tempParser( source );
	pXMLNODE MyRecordNode;
	pXMLNODE MyRecordSetNode;
	if(strXML.find("AutoRecordSet")!=string::npos)
	{
		MyRecordSetNode=parser.GetNodeFromPath("Msg/AutoRecordSet");
	}
	else if(strXML.find("TaskRecordSet")!=string::npos)
	{
		MyRecordSetNode=parser.GetNodeFromPath("Msg/TaskRecordSet");
	}
	else 
	{
		return false;
	}
	pXMLNODELIST recordSetList=parser.GetNodeList(MyRecordSetNode);
	if(recordSetList->Size()==0)
	{
		return -1;
	}
	for(int i=0;i<recordSetList->Size();++i)
	{
		string DeviceID,ServiceID,Width,Height,Bps;
		string CoderDeviceID="",CChanIdList="",strCChassisID="",strCPosition="",strCModuleID="",strCChanId="";
		pXMLNODE childNode=parser.GetNextNode(recordSetList);//s_Freq
		parser.GetAttrNode(childNode,"Freq",sChannelInfo.c_Freq);
		parser.GetAttrNode(childNode,"DeviceID",sChannelInfo.c_DeviceID);
		parser.GetAttrNode(childNode,"ServiceID",sChannelInfo.c_ServiceID);
		parser.GetAttrNode(childNode,"Width",sChannelInfo.c_Width);
		parser.GetAttrNode(childNode,"Height",sChannelInfo.c_Height);
		parser.GetAttrNode(childNode,"Bps",sChannelInfo.c_Bps);
		parser.GetAttrNode(childNode,"CChassisID",sChannelInfo.c_coderIndex);
		parser.GetAttrNode(childNode,"CPosition",strCPosition);
		parser.GetAttrNode(childNode,"CModuleID",sChannelInfo.c_CoderUnit);
		parser.GetAttrNode(childNode,"CTranscode",sChannelInfo.c_UnitCodeNum);
		vChannelInfo.push_back(sChannelInfo);
	}
	if(sChannelInfo.c_ServiceID.find(";")!=-1 || sChannelInfo.c_CoderUnit=="4")
	{
		return 1;
	}
	if(vChannelInfo.size()>0)
	{
		int IndexNum = PROPMANAGER::instance()->GetMonitorDevNum();
		for(int i =0;i<IndexNum;i++)
		{
			sDeviceInfo sDevInfo;
			bool ret = PROPMANAGER::instance()->GetDevMonitorInfo(i,sDevInfo);
			if(ret)
			{
				int SD_count = 0,HD2_count = 0,HD4_count = 0,FK_count = 0;
				for(int j =0;j<vChannelInfo.size();j++)
				{
					int i_Width = StrUtil::Str2Int(vChannelInfo[j].c_Width);
					int i_Height = StrUtil::Str2Int(vChannelInfo[j].c_Height);
					if(vChannelInfo[j].c_coderIndex==sDevInfo.s_deviceindex)
					{
						int RetChannelTVHD = CHANNELINFOMGR::instance()->HDTVType(DVBC,vChannelInfo[j].c_Freq,vChannelInfo[j].c_ServiceID);
						if(RetChannelTVHD==1)
						{
							SD_count++;
						}
						else if(RetChannelTVHD==4)
						{
							if(i_Width<=720 && i_Height<=576)
							{
								HD2_count++;
							}
							else if((i_Width>=1280 &&  i_Width<=1920) && (i_Height>=720 && i_Height<=1080))
							{
								HD4_count++;
							}
						}
						else if(RetChannelTVHD==8)
						{
							FK_count++;
						}
					}
				}
				int RetCount = SD_count*1 + HD2_count*2 + HD4_count*4 + FK_count*8;
				if(RetCount>32)
				{
					return -1;
				}
				else
				{
					HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
					for(int j=0;j<32;j++)
					{
						//关闭转码资源
						string cmd;
						cmd = "POST /tcoder/config  HTTP/1.1  \r\n\r\n";
						string strRet = "";
						cmd+="chl=";
						cmd+=StrUtil::Int2Str(j);
						cmd+="&ena=";
						cmd+="0";
						cmd+="&chk=";
						cmd+="0";
						cmd+="&sip=";
						cmd+="0.0.0.0";
						cmd+="&spt=";
						cmd+="0";
						cmd+="&spro=";
						cmd+="0";
						cmd+="&svid=";
						cmd+="0";
						cmd+="&saud0=";
						cmd+="0";
						cmd+="&saud1=";
						cmd+="0";
						cmd+="&saud2=";
						cmd+="0";
						cmd+="&vcode=";
						cmd+="h264";
						cmd+="&acode=";
						cmd+="mpeg";
						cmd+="&vfmt=";
						cmd+="2";//1 D1 2 CIF 0 自动
						cmd+="&vfre=";
						cmd+="0";
						cmd+="&v1v1=";
						cmd+="2";
						cmd+="&mutehold=";
						cmd+="-70";
						cmd+="&bfhold=";
						cmd+="20";
						cmd+="&sthold=";
						cmd+="1.5";
						cmd+="&vrate=";
						cmd+="1350";
						cmd+="&arate=";
						cmd+="32";
						cmd+="&srate=";
						cmd+="1500";
						cmd+="&osd0=";
						cmd+="150_40_16_time";
						cmd+="&osd1=";
						cmd+="50_50_16_test";
						cmd+="&osd2=";
						cmd+="150_200_16_test";
						cmd+="&dip=";
						cmd+="0.0.0.0";
						cmd+="&dpt=";
						cmd+="0";
						cmd+="&evip=";
						cmd+="0.0.0.0";
						cmd+="&evpt=";
						cmd+="0";
						cout<<"发送命令:"<<cmd<<endl;
						pDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<"收到结果:"<<strRet<<endl;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
					}
					delete pDeviceAccess;
				}
			}
		}
	}
	return 0;
}
