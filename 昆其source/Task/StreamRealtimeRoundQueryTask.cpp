
#include "StreamRealtimeRoundQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "TranslateXMLForDevice.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "ace/Synch.h"
#include "Scheduler.h"
#include "../Communications/TsSenderMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include <vector>
#include <iostream>
extern void ConvertGBKToUtf8(string& strGBK) ;
using namespace std;
ACE_Thread_Mutex g_RealtimeRoundMutex;
int g_RealtimeRoundTaskNum = 0;
extern bool g_getrounddata;

StreamRealtimeRoundQueryTask::StreamRealtimeRoundQueryTask() : DeviceRelatedTask()
{

}

StreamRealtimeRoundQueryTask::StreamRealtimeRoundQueryTask(std::string strXML) : DeviceRelatedTask(strXML)
{	
	StopSignal = false;
	RelatedTask = NULL;

	bOneDevice=false;
	bSendToIdleDev=false;

	TaskID = "MainTask";//设置为主任务

	DecivcXMLVec=TranslateXMLForDevice::TranslateRoundStreamXML(strXML);//发送给硬件的命令队列
	ChannelNum=DecivcXMLVec.size();	//轮播的频道数
	ChannelIndex=0;				//计数器


	//判断轮播任务是否含有非法频道
	XmlParser psr;
	psr.Set_xml(strXML);
	pXMLNODE rootNode = psr.GetNodeFromPath("Msg");
	string version;
	psr.GetAttrNode(rootNode,"Version",version);            //获得标准xml中RealtimeQueryTask节点的Freq属性值
	//
	pXMLNODE node=psr.GetNodeFromPath("Msg/StreamRoundQuery");
	std::string roundtime;

	psr.GetAttrNode(node,"RoundTime",roundtime);
	psr.GetAttrNode(node,"WindowNumber",WindowNumber);//*
	float runtime=0.5;
	psr.GetAttrNode(node,"RunTime",runtime);				//*
	if(runtime<0.0002)
	{
		runtime=0.5;
	}
	RunTime = runtime*3600;//*任务结束时间
	RoundTime=TimeUtil::StrToSecondTime(roundtime);//轮播时间
	if(RoundTime<8)
	{
		RoundTime = 8;
	}
	m_bmulti = false;

	if(WindowNumber >= 3)//if(RoundTime == 0)
	{
		SetMultiWindowNumber(WindowNumber);
		m_bmulti = true;
	}
	//
	CoderDev cdev;
	//
	pXMLNODELIST nodelist = psr.GetNodeList(node);
	string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Bps,sWidth,sHeight,sOsd;
	for (int i = 0;i < nodelist -> Size(); i++)
	{
		node=psr.GetNextNode(nodelist);
		string NodeName = psr.GetNodeName(node);
		if(NodeName == "RoundChannel")
		{
			psr.GetAttrNode(node,"Freq",Freq);
			psr.GetAttrNode(node,"OrgNetID",OrgNetID);
			psr.GetAttrNode(node,"TsID",TsID);
			psr.GetAttrNode(node,"ServiceID",ServiceID);
			psr.GetAttrNode(node,"VideoPID",VideoPID);
			psr.GetAttrNode(node,"AudioPID",AudioPID);
			psr.GetAttrNode(node,"Bps",Bps);
			psr.GetAttrNode(node,"Width",sWidth);  
			psr.GetAttrNode(node,"Height",sHeight);  
			psr.GetAttrNode(node,"OSDEnable",m_sOsd);  
			Device2ChanelID dev2chanel;
			dev2chanel.serviceid = ServiceID; 
			dev2chanel.videoid = VideoPID;
			dev2chanel.audioid = AudioPID;
			dev2chanel.freq = Freq;
			dev2chanel.programid = ServiceID;
			dev2chanel.bps = Bps;
			mIsHD = false;
			if (CHANNELINFOMGR::instance()->IsHDTV(DVBType,OrgNetID,TsID,ServiceID))
			{
				mIsHD = true;
			}
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				if (StrUtil::Str2Int(sWidth)>720||StrUtil::Str2Int(sHeight)>576)
				{
					mIsHD = true;
				}
			}
			else
			{
				if (StrUtil::Str2Int(sWidth)>640||StrUtil::Str2Int(sHeight)>480)
				{
					mIsHD = true;
				}
			}
			if(sOsd == "0")
			{
				mIsOSD = true;
			}
			else
			{
				mIsOSD = false;
			}
			if (DVBType==ATV||RADIO==DVBType||AM==DVBType||CTV==DVBType)
			{
				ChannelID=Freq;
			}
			else
			{
				CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,"",ChannelID);
				if(ChannelID.size()<=0)
				{
					CHANNELINFOMGR::instance()->GetChannelID(DVBType,Freq,ServiceID,ChannelID);
				}
				dev2chanel.chanelid = ChannelID;
			}

			int deviceid=-1;
			//BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByFreqEx(DVBType,Freq,deviceid,mIsHD,ServiceID,VideoPID,AudioPID);

			dev2chanel.deviceid =deviceid;
			dev2chanel.bhd=mIsHD;
			CHANNELINFOMGR::instance()->GetChannelInfo(DVBType,ChannelID,VideoPID,AudioPID,ServiceID,Program);
			dev2chanel.program = Program;
			string streamtype;
			CHANNELINFOMGR::instance()->GetVideoStreamType(DVBType,Freq,ServiceID,VideoPID,AudioPID,streamtype);
			MultiIndex.push_back(dev2chanel);
			if (ChannelID =="")
			{
				bRun = false;
				break;
			}
		}
		else if(NodeName == "RoundTuner")
		{
			string tuner;
			psr.GetAttrNode(node,"Info",tuner);
			if((tuner.size()>0)&&(tuner != "-1"))
			{
				m_tundevlist.push_back(StrUtil::Str2Int(tuner));
			}
		}
		else if(NodeName == "RoundCoder")
		{
			string coder;
			psr.GetAttrNode(node,"Info",coder);
			if((coder.size()>0)&&(coder != "-1"))
			{
				m_coddevlist.push_back(StrUtil::Str2Int(coder));
			}
		}
	}
	//设置调度
	int iBps = StrUtil::Str2Int(Bps)/1024;
	size_t sz = 0; 
	string strtime = "time";
	string utf8_time;
	string utf8_proname;
	string utf8_osdinfo;
	string striosdfontsize;
	string osdinfo,strosdinfoX,strosdinfoY,strosdtimeX,strosdtimeY,proname,strosdprogramX,strosdprogramY,isosd;
	//
	if(strtime.size()>0)
	{
		utf8_time = strtime;
		ConvertGBKToUtf8(utf8_time);
	}
	//
	OSDInfo osdin;
	if(mIsHD)
	{
		PROPMANAGER::instance()->GetOSDInfo(DVBC,"1",osdin);
	}
	else
	{
		PROPMANAGER::instance()->GetOSDInfo(DVBC,"0",osdin);
	}
	int iosdfontsize = StrUtil::Str2Int(osdin.FontSize);
	if(iosdfontsize < 8)
	{
		iosdfontsize = 8;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
	}
	osdinfo = osdin.Info;
	striosdfontsize = osdin.FontSize;
	//
	if(version == "3.0")
	{
		if(m_tundevlist.size()>m_coddevlist.size())
		{
			m_roundnum = m_coddevlist.size();
		}
		else
		{
			m_roundnum = m_tundevlist.size();
		}
	}
	else
	{
		std::list<int> tmptundevlist;
		std::list<int> tmpcoddevlist;
		PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeRoundQueryTask",DVBC,tmptundevlist);
		std::list<int>::iterator tptr=tmptundevlist.begin();
		for(;tptr!=tmptundevlist.end();tptr++)
		{
			m_tundevlist.push_back(*tptr);
		}
		PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeRoundQueryTask",DVBC,tmpcoddevlist);
		std::list<int>::iterator cptr=tmpcoddevlist.begin();
		for(;cptr!=tmpcoddevlist.end();cptr++)
		{
			m_coddevlist.push_back(*cptr);
		}
		if(m_tundevlist.size()>m_coddevlist.size())
		{
			m_roundnum = m_coddevlist.size();
		}
		else
		{
			m_roundnum = m_tundevlist.size();
		}
	}
	//
	int coderdevid = -1;
	std::vector<int>::iterator ptr=m_coddevlist.begin();
	for(int i=0;ptr!=m_coddevlist.end();ptr++,i++)
	{		
		if(i==m_roundnum-1)
		{
			coderdevid = (*ptr);
		}
	}
	if(coderdevid != -1)
	{
		PROPMANAGER::instance()->GetCoderInfo(coderdevid,cdev);
		DeviceID = coderdevid;
	}
	//
	m_startxml ="POST /device/streamrdset HTTP/1.1 <?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?> ";//返回xml头
	m_startxml+="<Msg Version=\"1.0\" Type=\"down\" DateTime=\"";
	m_startxml+=TimeUtil::GetCurDateTime();
	m_startxml+="\" Priority=\"1\">";
	m_startxml+="<StreamRoundConfig Idx=\"0";
	//m_startxml+=StrUtil::Int2Str(coderdevid);
	m_startxml+="\" Enable=\"1\" RoundTime=\"";
	m_startxml+=StrUtil::Int2Str(RoundTime);
	m_startxml+="\" ChannelNum=\"";
	m_startxml+=StrUtil::Int2Str(m_roundnum);
	m_startxml+="\" StreamNum=\"";
	m_startxml+=StrUtil::Int2Str(MultiIndex.size());
	m_startxml+="\" VideoCode=\"h264\" AudioCode=\"mpeg\" Quality=\"0\" Width=\"";
	m_startxml+="352";//sWidth;//轮播分辨率统一
	m_startxml+="\" Height=\"";
	m_startxml+="288";//sHeight;//轮播分辨率统一
	m_startxml+="\" Fps=\"25\" VideoRate=\"";
	m_startxml+=StrUtil::Int2Str(iBps-32);
	m_startxml+="\" AudioRate=";
	m_startxml+="\"32\"";
	m_startxml+=" SysRate=\"";
	m_startxml+=StrUtil::Int2Str(iBps);
	m_startxml+="\" DstIp=\"";
	m_startxml+=cdev.s_dip;
	m_startxml+="\" DstPort=\"";
	m_startxml+=cdev.s_dtsport;
	m_startxml+="\" >";
	std::vector<int>::iterator tpt=m_tundevlist.begin();
	std::vector<int>::iterator cpt=m_coddevlist.begin();
	for(int i=0;;tpt++,cpt++,i++)
	{		
		if(i>=m_roundnum)
		{
			break;
		}
		TunerDev tdev;
		PROPMANAGER::instance()->GetTunerInfo((*tpt),DVBC,tdev);
		m_startxml+="<Channel Idx=\"";
		m_startxml+=StrUtil::Int2Str(i);
		m_startxml+="\" TCoderIdx=\"";
		m_startxml+=StrUtil::Int2Str(*cpt);
		m_startxml+="\" CardIdx=\"";
		m_startxml+=tdev.s_cardindex;
		m_startxml+="_";
		m_startxml+=tdev.s_chanindex;
		m_startxml+="\"/>";
		//
		if((version == "3.0")&&(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9"))
		{
			if(!(PROPMANAGER::instance()->SetCoderLinkTunerinfo(*cpt,*tpt,DVBC,MultiIndex[i%MultiIndex.size()].freq,MultiIndex[i%MultiIndex.size()].serviceid,MultiIndex[i%MultiIndex.size()].bhd,Bps,"实时轮播进行中...")))
			{
				DeviceID = -1;
				break;
			}
		}
	}
	sFreqScanInfo freqscaninfo;
	PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
	for(int k=0;k<MultiIndex.size();k++)
	{
		//
		proname = MultiIndex[k].program;
		if(proname.size()>0)
		{
			utf8_proname = proname;
			ConvertGBKToUtf8(utf8_proname);
		}
		//
		if(osdinfo.size()>0)
		{
			utf8_osdinfo = osdinfo;
			ConvertGBKToUtf8(utf8_osdinfo);
		}
		string scurTime = TimeUtil::GetCurDateTime();
		int iosdTimeLen = scurTime.length();
		int iosdProLen = proname.length();
		int iosdInfoLen = proname.length() + osdinfo.length();
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdTimeLen+2)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdProLen+2)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdInfoLen+3)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		//
		m_startxml+="<Stream  Idx=\"";
		m_startxml+=StrUtil::Int2Str(k);
		m_startxml+="\" Type=\"DVBC\" Freq=\"";
		m_startxml+=MultiIndex[k].freq;
		m_startxml+="\" Symbol=\"";
		m_startxml+=freqscaninfo.SymbolRate;
		m_startxml+="\" Mode=\"QAM64\" ServiceID=\"";
		m_startxml+=MultiIndex[k].serviceid;
		m_startxml+="\" VideoPid=\"";
		m_startxml+=MultiIndex[k].videoid;
		m_startxml+="\" AudioPid=\"";
		m_startxml+=MultiIndex[k].audioid;
		m_startxml+="\" OSD0=\"";
		m_startxml+=strosdtimeX;
		m_startxml+="_";
		m_startxml+=strosdtimeY;
		m_startxml+="_";
		m_startxml+=striosdfontsize;
		m_startxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_startxml+=utf8_time;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_startxml+=utf8_time;
			}
		}
		m_startxml+="\" OSD1=\"";
		m_startxml+=strosdprogramX;
		m_startxml+="_";
		m_startxml+=strosdprogramY;
		m_startxml+="_";
		m_startxml+=striosdfontsize;
		m_startxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_startxml+=utf8_proname;//proname;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_startxml+=utf8_proname;//proname;
			}
		}
		m_startxml+="\" OSD2=\"";
		m_startxml+=strosdinfoX;
		m_startxml+="_";
		m_startxml+=strosdinfoY;
		m_startxml+="_";
		m_startxml+=striosdfontsize;
		m_startxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_startxml+=utf8_osdinfo;//osdinfo;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_startxml+=utf8_osdinfo;//osdinfo;
			}
		}
		m_startxml+="\"/>";
	}
	m_startxml+="</StreamRoundConfig>";
	m_startxml+="</Msg>";
	//
	m_stopxml ="POST /device/streamrdset HTTP/1.1 <?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?> ";//返回xml头
	m_stopxml+="<Msg Version=\"1.0\" Type=\"down\" DateTime=\"";
	m_stopxml+=TimeUtil::GetCurDateTime();
	m_stopxml+="\" Priority=\"1\">";
	m_stopxml+="<StreamRoundConfig Idx=\"0";
	m_stopxml+="\" Enable=\"0\" RoundTime=\"";
	m_stopxml+=StrUtil::Int2Str(RoundTime);
	m_stopxml+="\" ChannelNum=\"";
	m_stopxml+=StrUtil::Int2Str(m_roundnum);
	m_stopxml+="\" StreamNum=\"";
	m_stopxml+=StrUtil::Int2Str(MultiIndex.size());
	m_stopxml+="\" VideoCode=\"h264\" AudioCode=\"mpeg\" Quality=\"0\" Width=\"";
	m_stopxml+="352";//sWidth;//轮播分辨率统一
	m_stopxml+="\" Height=\"";
	m_stopxml+="288";//sHeight;//轮播分辨率统一
	m_stopxml+="\" Fps=\"25\" VideoRate=\"";
	m_stopxml+=StrUtil::Int2Str(iBps-32);
	m_stopxml+="\" AudioRate=";
	m_stopxml+="\"32\"";
	m_stopxml+=" SysRate=\"";
	m_stopxml+=StrUtil::Int2Str(iBps);
	m_stopxml+="\" DstIp=\"";
	m_stopxml+=cdev.s_dip;
	m_stopxml+="\" DstPort=\"";
	m_stopxml+=cdev.s_dtsport;
	m_stopxml+="\" >";
	tpt=m_tundevlist.begin();
	cpt=m_coddevlist.begin();
	for(int i=0;;tpt++,cpt++,i++)
	{		
		if(i>=m_roundnum)
		{
			break;
		}
		TunerDev tdev;
		PROPMANAGER::instance()->GetTunerInfo((*tpt),DVBC,tdev);
		m_stopxml+="<Channel Idx=\"";
		m_stopxml+=StrUtil::Int2Str(i);
		m_stopxml+="\" TCoderIdx=\"";
		m_stopxml+=StrUtil::Int2Str(*cpt);
		m_stopxml+="\" CardIdx=\"";
		m_stopxml+=tdev.s_cardindex;
		m_stopxml+="_";
		m_stopxml+=tdev.s_chanindex;
		m_stopxml+="\"/>";
	}
	for(int k=0;k<MultiIndex.size();k++)
	{
		//
		proname = MultiIndex[k].program;
		if(proname.size()>0)
		{
			utf8_proname = proname;
			ConvertGBKToUtf8(utf8_proname);
		}
		//
		if(osdinfo.size()>0)
		{
			utf8_osdinfo = osdinfo;
			ConvertGBKToUtf8(utf8_osdinfo);
		}
		string scurTime = TimeUtil::GetCurDateTime();
		int iosdTimeLen = scurTime.length();
		int iosdProLen = proname.length();
		int iosdInfoLen = proname.length() + osdinfo.length();
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdTimeLen+2)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdProLen+2)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(sWidth)-(iosdInfoLen+3)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		//
		m_stopxml+="<Stream  Idx=\"";
		m_stopxml+=StrUtil::Int2Str(k);
		m_stopxml+="\" Type=\"DVBC\" Freq=\"";
		m_stopxml+=MultiIndex[k].freq;
		m_stopxml+="\" Symbol=\"";
		m_stopxml+=freqscaninfo.SymbolRate;
		m_stopxml+="\" Mode=\"QAM64\" ServiceID=\"";
		m_stopxml+=MultiIndex[k].serviceid;
		m_stopxml+="\" VideoPid=\"";
		m_stopxml+=MultiIndex[k].videoid;
		m_stopxml+="\" AudioPid=\"";
		m_stopxml+=MultiIndex[k].audioid;
		m_stopxml+="\" OSD0=\"";
		m_stopxml+=strosdtimeX;
		m_stopxml+="_";
		m_stopxml+=strosdtimeY;
		m_stopxml+="_";
		m_stopxml+=striosdfontsize;
		m_stopxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_stopxml+=utf8_time;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_stopxml+=utf8_time;
			}
		}
		m_stopxml+="\" OSD1=\"";
		m_stopxml+=strosdprogramX;
		m_stopxml+="_";
		m_stopxml+=strosdprogramY;
		m_stopxml+="_";
		m_stopxml+=striosdfontsize;
		m_stopxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_stopxml+=utf8_proname;//proname;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_stopxml+=utf8_proname;//proname;
			}
		}
		m_stopxml+="\" OSD2=\"";
		m_stopxml+=strosdinfoX;
		m_stopxml+="_";
		m_stopxml+=strosdinfoY;
		m_stopxml+="_";
		m_stopxml+=striosdfontsize;
		m_stopxml+="_";
		if(version == "3.0")
		{
			if(m_sOsd != "0")
			{
				m_stopxml+=utf8_osdinfo;//osdinfo;
			}
		}
		else
		{
			if(m_sOsd != "1")
			{
				m_stopxml+=utf8_osdinfo;//osdinfo;
			}
		}
		m_stopxml+="\"/>";
	}
	m_stopxml+="</StreamRoundConfig>";
	m_stopxml+="</Msg>";
	//
	TaskScheduler = new Scheduler();
	if (TaskScheduler != NULL)
	{
		TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());//,taskendtime);
	}
}

StreamRealtimeRoundQueryTask::~StreamRealtimeRoundQueryTask()
{

}

void StreamRealtimeRoundQueryTask::Run(void)
{
	
	ACE_DEBUG ((LM_DEBUG,"(%D | %t) 实时视频轮播任务准备 !\n"));
	ACE_Guard<ACE_Thread_Mutex> guard(g_RealtimeRoundMutex);
	ACE_DEBUG ((LM_DEBUG,"(%D | %t) 实时视频轮播任务执行 !\n"));
	//
	std::vector<int>::iterator cpt=m_coddevlist.begin();
	for(int i=0;i<m_coddevlist.size();cpt++,i++)
	{		
		TSFETCHERMGR::instance()->StopHistoryTask(*cpt);
		TSFETCHERMGR::instance()->SetSendSwitch(*cpt,false);
	}
	g_getrounddata = false;
	//
	g_RealtimeRoundTaskNum++;
	//
	bRun = true;
	SetRunning();

	if(ChannelNum==0)
	{
		RetValue=RUN_FAILED;
	}
	//判断视频轮播任务设置是否成功,设置失败，直接给用户发送失败信息
	if(RetValue != SET_SUCCESS)
	{
		SendXML(TranslateUpXML::TranslateStreamRoundQuery(this));

		if (TaskScheduler != NULL)
		{
			TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));
		}

		SetFinised();//任务停止

		ACE_DEBUG((LM_DEBUG,"(%D | %t) 实时视频轮播主任务设置失败!\n"));
		//
		if(GetVersion()=="3.0")
		{
			std::vector<int>::iterator tpt=m_tundevlist.begin();
			std::vector<int>::iterator cpt=m_coddevlist.begin();
			for(int i=0;;tpt++,cpt++,i++)
			{		
				if(i>=m_roundnum)
				{
					break;
				}
				PROPMANAGER::instance()->FreeSetTunerDev(*tpt,DVBC,"All","All");
				PROPMANAGER::instance()->FreeSetCoderDev(*cpt);
			}
		}
		return;
	}
	RetValue=RUN_SUCCESS;
	int curdeviceid=0;
	int lastusedevid=-1;
	time_t lastTime=0;
	ChannelIndex = 0;//已准备好流的频道标记
	int playchan = 0;//当前播放的频道标记
	bool bchangepro = true;//标志现有资源够不够,用不用换台 false不用
	int num = 0;
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")//1U设备内部轮播
	{
		//给设备发转码命令m_startxml
		string codemonitorip,codemonitorport;
		PROPMANAGER::instance()->GetDevMonitorInfo(1,DVBC,DeviceID,codemonitorip,codemonitorport);//coder
		CoderDev cdev;
		PROPMANAGER::instance()->GetCoderInfo(DeviceID,cdev);
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(cdev.s_monitorindex),codemonitorip,StrUtil::Str2Int(codemonitorport));
		string strRet;
		pDeviceAccess->SendTaskMsg(m_stopxml,strRet);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,m_stopxml,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		Sleep(1000);
		pDeviceAccess->SendTaskMsg(m_startxml,strRet);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,m_startxml,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		//
		string sxml=TranslateUpXML::TranslateStreamRoundQuery(this,StrUtil::Int2Str(DeviceID),true);
		SendXML(sxml);	
		TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
		TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,"StreamRealtimeRoundQueryTask");
		time_t curtime = time(0);
		int waittime = 10;
		while ( bRun&&IsRunning()) 
		{
			if (TSSENDERMGR::instance()->HasClient(DeviceID) == false)//没有连接用户，任务退出
			{
				if(time(0)-curtime > waittime)
				{
					break;
				}
				else
				{
					OSFunction::Sleep(0,500);
					continue;
				}
			}
			waittime = 0;
			curtime = time(0);
			OSFunction::Sleep(0,100);
		}
		TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
		//
		//给设备发转码命令m_stopxml
		pDeviceAccess->SendTaskMsg(m_stopxml,strRet);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,m_stopxml,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		delete pDeviceAccess;
		//
	}
	else//非1U设备内部轮播
	{
		if(m_coddevlist.size()==1)
		{
			TSFETCHERMGR::instance()->SetSendSwitch(m_coddevlist[0],true);
		}
		for(int k=0;k<MultiIndex.size();k++)
		{
			if(MultiIndex[k].deviceid >= 0)
			{
				ChannelIndex++;
				if(ChannelIndex>=ChannelNum)
				{
					bchangepro = false;
				}
			}
			else
			{
				TSFETCHERMGR::instance()->IncreaseTaskNum(m_coddevlist[num]);
				string devcmd = DecivcXMLVec[ChannelIndex];
				//
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
				{
					ChangeDevCommand(DecivcXMLVec[ChannelIndex],StrUtil::Int2Str(m_tundevlist[num]),StrUtil::Int2Str(m_coddevlist[num]),devcmd);
					PROPMANAGER::instance()->FreeSetTunerDev(m_tundevlist[num],DVBC,"All","All");
					PROPMANAGER::instance()->FreeSetCoderDev(m_coddevlist[num]);
					PROPMANAGER::instance()->SetCoderLinkTunerinfo(m_coddevlist[num],m_tundevlist[num],DVBC,MultiIndex[ChannelIndex].freq,MultiIndex[ChannelIndex].serviceid,MultiIndex[ChannelIndex].bhd,MultiIndex[ChannelIndex].bps,"实时轮播进行中...");
				}
				//
				TSFETCHERMGR::instance()->SetTsDeviceXml(m_coddevlist[num],devcmd);
				num++;
				ChannelIndex++;
				if(ChannelIndex>=ChannelNum)
				{
					bchangepro = false;
				}
				if(num>=m_tundevlist.size()||num>=m_coddevlist.size())
				{
					break;
				}
			}
			if(!bchangepro)
			{
				break;
			}
		}
		//wz_0217
		int roundchannel = 0;
		if(false == PROPMANAGER::instance()->GetVirDeviceId(DVBType, roundchannel))
		{
			ACE_DEBUG ((LM_DEBUG,"(%D | %t) 轮播任务获取轮播通道失败 !\n"));
		}
		//
		SendXML(TranslateUpXML::TranslateStreamRoundQuery(this));
		//
		time_t curtime = time(0);
		int waittime = 10;
		bool bfirst = true;
		while ( bRun&&IsRunning()) 
		{
			//wz_0217
			if (TSSENDERMGR::instance()->HasClient(roundchannel) == false)//没有连接用户，任务退出
			{
				bfirst = false;
				if(time(0)-curtime > waittime)
				{
					break;
				}
				else
				{
					OSFunction::Sleep(0,10);
					continue;
				}
			}
			if(!bfirst)
			{
				waittime = 0;
				curtime = time(0);
			}
			if((time(0)-lastTime>=RoundTime)&&(DecivcXMLVec.size() > 1))
			{
				if(playchan>=ChannelNum)
				{
					playchan = 0;
				}
				if(ChannelIndex>=ChannelNum)
				{
					ChannelIndex = 0;
				}
				if(curdeviceid>=m_roundnum)
				{
					curdeviceid = 0;
				}
				if(lastusedevid != -1)
				{
					if(m_coddevlist.size()!=1)
					{
						TSFETCHERMGR::instance()->SetSendSwitch(m_coddevlist[lastusedevid],false);
					}
					if(m_coddevlist.size()!=1)
					{
						TSFETCHERMGR::instance()->SetSendSwitch(m_coddevlist[curdeviceid],true);
					}
					if(bchangepro)
					{
						for(int k=0;k<MultiIndex.size();k++)
						{
							string devcmd = DecivcXMLVec[ChannelIndex];
							//
							if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
							{
								ChangeDevCommand(DecivcXMLVec[ChannelIndex],StrUtil::Int2Str(m_tundevlist[lastusedevid]),StrUtil::Int2Str(m_coddevlist[lastusedevid]),devcmd);
								PROPMANAGER::instance()->FreeSetTunerDev(m_tundevlist[lastusedevid],DVBC,"All","All");
								PROPMANAGER::instance()->FreeSetCoderDev(m_coddevlist[lastusedevid]);
								PROPMANAGER::instance()->SetCoderLinkTunerinfo(m_coddevlist[lastusedevid],m_tundevlist[lastusedevid],DVBC,MultiIndex[ChannelIndex].freq,MultiIndex[ChannelIndex].serviceid,MultiIndex[ChannelIndex].bhd,MultiIndex[ChannelIndex].bps,"实时轮播进行中...");
							}
							//
							bool bSetTS = false;
							//for(int i=0;i<200;i++)
							//{
							//	if(!g_getrounddata)
							//	{
							//		TSFETCHERMGR::instance()->SetTsDeviceXml(m_coddevlist[lastusedevid],devcmd);
							//		bSetTS = true;
							//	}
							//	Sleep(10);
							//}
							if(!bSetTS)
							{
								TSFETCHERMGR::instance()->SetTsDeviceXml(m_coddevlist[lastusedevid],devcmd);
							}
							ChannelIndex++;
							if(ChannelIndex>=ChannelNum)
							{
								ChannelIndex = 0;
							}
							break;
						}
					}
				}
				else
				{
					if(m_coddevlist.size()!=1)
					{
						TSFETCHERMGR::instance()->SetSendSwitch(m_coddevlist[curdeviceid],true);
						Sleep(4000);
					}
				}
				lastusedevid = curdeviceid;
				curdeviceid++;
				playchan++;
				//
				lastTime=time(0);
				//
			}
			else
			{
				OSFunction::Sleep(0,10);
			}
		}
		if(m_coddevlist.size()==1)
		{
			TSFETCHERMGR::instance()->SetSendSwitch(m_coddevlist[0],false);
		}
		for(int id=0;id<m_tundevlist.size();id++)
		{
			TSFETCHERMGR::instance()->SetSendSwitch(id,false);
		}
		for(int k=0;k<MultiIndex.size();k++)
		{
			if(MultiIndex[k].deviceid >= 0)
			{
				TSFETCHERMGR::instance()->SetSendSwitch(MultiIndex[k].deviceid,false);
			}
		}
		for(int h = 0;h<m_coddevlist.size();h++)
		{
			TSFETCHERMGR::instance()->DecreaseTaskNum(m_coddevlist[h]);
		}
	}
	if(GetVersion()=="3.0")
	{
		std::vector<int>::iterator tpt=m_tundevlist.begin();
		std::vector<int>::iterator cpt=m_coddevlist.begin();
		for(int i=0;;tpt++,cpt++,i++)
		{		
			if(i>=m_roundnum)
			{
				break;
			}
			PROPMANAGER::instance()->FreeSetTunerDev(*tpt,DVBC,"All","All");
			PROPMANAGER::instance()->FreeSetCoderDev(*cpt);
		}
	}
	bRun = false;
	if (TaskScheduler != NULL)
	{
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));
	}

	SetFinised();
	//
	g_RealtimeRoundTaskNum--;
	//
	ACE_DEBUG ((LM_DEBUG,"(%D | %t) 实时视频轮播任务停止 !\n"));
}
string StreamRealtimeRoundQueryTask::GetTaskName()
{
	return std::string("实时视频轮播任务");
}

/**
StreamRoundChangeInfoR.xml（主动上报）
<?xml version="1.0" encoding="GB2312" standalone="yes" ?> 
<Msg  Version=“2.5” MsgID="2" Type="MonUp" DateTime="2002-08-17 15:30:00" SrcCode="110000M01" DstCode="110000G01"ReplyID="1000_ID">
<Return Type="StreamRoundInfo" Value="0" Desc="执行成功"/>
<ReturnInfo>
< StreamRoundInfo  WindowNumber = "4" Num= "0">
<Channel Position=”1”  Program="CCTV-1" ProgramID="454" Freq=“” ServiceID="454" />
<Channel Position=”2”  Program="CCTV-2 " ProgramID="456" Freq =“”ServiceID="456" />
<Channel Position=”3”  Program="CCTV-3 " ProgramID="457" Freq =“” ServiceID="457" />
<Channel Position=”4”  Program="无 " ProgramID="0"  Freq =”0” ServiceID="0" />   
</ StreamRoundInfo >
</ReturnInfo>
</Msg>
**/
std::string StreamRealtimeRoundQueryTask::ReportCreateXML()   
{
	std::string roundxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	roundxml+="<Msg><Return Type=""StreamRoundInfo"" Value=""0"" Desc=""执行成功""/><ReturnInfo></ReturnInfo></Msg>";
	XmlParser roundparser(roundxml.c_str());

	pXMLNODE roundroot=roundparser.GetRootNode();
	roundparser.SetAttrNode( "Version",string("1.0"),roundroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	roundparser.SetAttrNode( "MsgID",MsgID,roundroot );//消息id属性
	roundparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,roundroot );//当前时间


	//添加<Msg  Version=“2.5” MsgID="2" Type="MonUp" DateTime="2002-08-17 15:30:00" SrcCode="110000M01" DstCode="110000G01"ReplyID="1000_ID">
	roundparser.SetAttrNode( "Type",string("MonUp"),roundroot );//消息类型
	roundparser.SetAttrNode( "SrcCode",GetSrcCode(),roundroot );//本机标识，可通过接口获得
	roundparser.SetAttrNode( "DstCode",GetDstCode(),roundroot );//目标机器标识
	roundparser.SetAttrNode( "ReplyID",GetMsgID(),roundroot );//回复的消息id



	// 添加< StreamRoundInfo  WindowNumber = "4" Num= "0">
	pXMLNODE returninfonode=roundparser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE roundinfonode=roundparser.CreateNodePtr(returninfonode,"StreamRoundInfo");
	roundparser.SetAttrNode("WindowNumber",WindowNumber,roundinfonode);
	roundparser.SetAttrNode("Num",MultiNum,roundinfonode);

	//添加<Channel Position=”1”  Program="CCTV-1" ProgramID="454" Freq=“” ServiceID="454" />
	//	pXMLNODE roundreportclnode=roundparser.GetNodeFromPath("Msg/ReturnInfo/StreamRoundInfo");
	for(int i=0;i<MultiCurIndex.size();i++)
	{
		pXMLNODE roundreportcldnode=roundparser.CreateNodePtr(roundinfonode,"Channel");
		roundparser.SetAttrNode("Position",i+1,roundreportcldnode);
		roundparser.SetAttrNode("Program",MultiCurIndex[i].program,roundreportcldnode);
		roundparser.SetAttrNode("ProgramID",MultiCurIndex[i].programid,roundreportcldnode);
		roundparser.SetAttrNode("Freq",MultiCurIndex[i].freq,roundreportcldnode);
		roundparser.SetAttrNode("ServiceID",MultiCurIndex[i].serviceid,roundreportcldnode);
	}
	std::string retXML;
	roundparser.SaveToString(retXML);
	cout << retXML<<endl;
	return retXML;
}
std::string StreamRealtimeRoundQueryTask::GetObjectName()
{
	return std::string("StreamRealtimeRoundQueryTask");
}
bool StreamRealtimeRoundQueryTask::ChangeDevCommand(string incmd,string turnid,string coderid,string& outcmd)
{
	XmlParser tmpparser(incmd.c_str());
	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.SetAttrNode("TDeviceID",turnid,TsNode);
	tmpparser.SetAttrNode("CDeviceID",coderid,TsNode);
	tmpparser.SetAttrNode("ComamdType",string("1"),TsNode);
	tmpparser.SaveToString(outcmd);
	return true;
}
