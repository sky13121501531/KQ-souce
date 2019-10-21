
#include "StreamRealtimeQueryTask.h"
#include "Scheduler.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Communications/TsSenderMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/AppLog.h"
#include "ace/Synch.h"
#include "./TranslateXMLForDevice.h"
#include "../FileSysAccess/TSRecorder.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <iostream>

using namespace std;
ACE_Thread_Mutex g_RealtimeMutex;
int g_realstreamusedev = -1;//实时视频不报警
const std::string MANURECORD_EXPIREDAYS = "7";

class ManuRecord_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ManuRecord_Task();
	~ManuRecord_Task(){};

};

ManuRecord_Task::ManuRecord_Task()
{
	msg_queue()->high_water_mark(188*8192);
	msg_queue()->low_water_mark(188*8192);
}

StreamRealtimeQueryTask::StreamRealtimeQueryTask() : DeviceRelatedTask()
{

}

StreamRealtimeQueryTask::StreamRealtimeQueryTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	bManualRecord=false;
	std::string freq,version,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,Bps,CodingFormat,sWidth,sHeight,sOsd;
	XmlParser parser(strXML.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",version);            //获得标准xml中RealtimeQueryTask节点的Freq属性值
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/StreamRealtimeQuery/RealtimeQueryTask");
	parser.GetAttrNode(queryNode,"Freq",freq);            //获得标准xml中RealtimeQueryTask节点的Freq属性值
	parser.GetAttrNode(queryNode,"OrgNetID",OrgNetID);    //获得标准xml中RealtimeQueryTask节点的OrgNetID属性值
	parser.GetAttrNode(queryNode,"TsID",TsID);            //获得标准xml中RealtimeQueryTask节点的TsID属性值
	parser.GetAttrNode(queryNode,"ServiceID",ServiceID);  //获得标准xml中RealtimeQueryTask节点的ServiceID属性值
	parser.GetAttrNode(queryNode,"VideoPID",VideoPID);  //获得标准xml中RealtimeQueryTask节点的VideoPID属性值
	parser.GetAttrNode(queryNode,"AudioPID",AudioPID);  //获得标准xml中RealtimeQueryTask节点的AudioPID属性值
	parser.GetAttrNode(queryNode,"Bps",Bps);  //获得标准xml中RealtimeQueryTask节点的AudioPID属性值
	parser.GetAttrNode(queryNode,"CodingFormat",CodingFormat);  //获得标准xml中RealtimeQueryTask节点的AudioPID属性值
	parser.GetAttrNode(queryNode,"Width",sWidth);  
	parser.GetAttrNode(queryNode,"Height",sHeight);  
	parser.GetAttrNode(queryNode,"CDeviceID",m_cdevid);  
	parser.GetAttrNode(queryNode,"TDeviceID",m_tdevid);  
	parser.GetAttrNode(queryNode,"OSDEnable",sOsd); 
	//
	strServiceID = ServiceID;
	strVideoPID = VideoPID;
	strAudioPID = AudioPID;
	mBps = Bps;
	//

	float nTempTime = 1;
	parser.GetAttrNode(queryNode, "RunTime", nTempTime);	//单位为：小时
	if (nTempTime == 0)
	{
		m_nRunTime = 60*60*24;	//一天
	}
	else
	{
		m_nRunTime = nTempTime*60*60 + 3;	//加大3s的调频时间
	}
	Freq = freq;                                          
	strDeviceXML = TranslateXMLForDevice::TranslateStreamRealTimeXML(strXML);//自定义标准XML转化为与硬件通信的xml
	if (ATV==DVBType || RADIO==DVBType || DVBType==AM || DVBType==CTV)
	{
		ChannelID=freq;
	}
	else
	{
		if (Freq == strHDMIFreq)
		{
			ChannelID = Freq;
		}
		else
		{
			CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,ChannelID);
			if(ChannelID.size()<=0)
			{
				CHANNELINFOMGR::instance()->GetChannelID(DVBType,freq,ServiceID,ChannelID);
			}
		}

	}
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
	//
	if(version == "3.0")
	{
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				if(PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(m_tdevid),DVBType,Freq,ServiceID,mIsHD,Bps,"实时视频进行中..."))
				{
					DeviceID = codlis[i];
				}
				else
				{
					break;
				}
			}
			else
			{
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,ServiceID,DVBType,mIsHD,"实时视频进行中...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
	//
	if(sOsd == "0")
	{
		mIsOSD = true;
	}
	else
	{
		mIsOSD = false;
	}
	mRecordFileName = "";	//手动录像改为存到tempfile表中  jidushan 110406

    TaskScheduler = new Scheduler();
	if (TaskScheduler != NULL)
	{
		TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
	}
}

StreamRealtimeQueryTask::~StreamRealtimeQueryTask()
{

}
void StreamRealtimeQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时视频任务执行 !\n",DeviceID));
	//
	if(GetVersion() == "3.0")
	{
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(m_tdevid),DVBType,Freq,strServiceID,mIsHD,mBps,"实时视频进行中...");
			}
			else
			{
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,strServiceID,DVBType,mIsHD,"实时视频进行中...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
	//
	g_realstreamusedev = DeviceID;
	bRun = true;
	SetRunning();//任务状态更新为正在运行

	if(RetValue != SET_SUCCESS)	//实时视频任务设置失败
	{			
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));
		SetFinised();//任务停止
		SetExpired();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t)实时视频任务设置失败!\n"));
		return;
	}

	TSFETCHERMGR::instance()->StopHistoryTask(DeviceID);			//清除历史视频

	bool FindRecordDevice = false;
	int tempID = -1;
	if(ChannelID == strHDMIFreq)//机顶盒有录像就返回录像通道,不能一个HDMI数据同时两转码通道又录像又实时
	{
		if(BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByChannelID(DVBType,ChannelID,tempID))
		{
			FindRecordDevice = true;
			DeviceID = tempID;			
		}
	}
	else
	{
		if(PROPMANAGER::instance()->IsRealTimeFromRecord(DVBType)&&BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByChannelID(DVBType,ChannelID,tempID))
		{
			FindRecordDevice = true;
			DeviceID = tempID;			
		}
	}
	
	RetValue=RUN_SUCCESS;

	if (FindRecordDevice)	//如果回复的是自动录像的通道，任务直接返回退出
	{
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));	//发送转化后的xml

		if (TaskScheduler != NULL)
			TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

		SetFinised();																			//任务停止
		bRun = false;
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时视频任务（录像通道）停止 !\n",DeviceID));
		return;
	}
	else//如果没有找到做该频点任务的录像通道
	{
		int idevID = -1;
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));	//发送转化后的xml
		TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,strDeviceXML);
		TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	}

	//手动录像
	ACE_Task<ACE_MT_SYNCH>* Task = new ManuRecord_Task;
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);

	const int BufLen = 188*4096;
	const int BufSize = 188*1024;

	unsigned char* TsBuf = new unsigned char[BufLen];//接收数据缓冲区
	memset(TsBuf,0,BufLen);

	int PacketLen = 0;
	int FirstCheck = 10;

	std::string strBeginTime = TimeUtil::GetCurDateTime();

	time_t curtime = time(0);
	int waittime = 10;
	bool bfirst = true;
	while(bRun && IsRunning())
	{
		std::string strCurTime = TimeUtil::GetCurDateTime();
		if ( TimeUtil::DiffSecond(strCurTime,strBeginTime) >= m_nRunTime )
		{
			std::cout<<"实时视频任务时间已到，自动停止!"<<std::endl;
			break;
		}


		if (TSSENDERMGR::instance()->HasClient(DeviceID) == false)//没有连接用户，任务退出
		{
			bfirst = false;
			if(time(0)-curtime > waittime)
			{
				break;
			}
			else
			{
				OSFunction::Sleep(0,50);
				continue;
			}
		}
		if(!bfirst)
		{
			waittime = 0;
			curtime = time(0);
		}
		//有手动录像任务,且手动录像任务在执行时间以内
		if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)<=StrUtil::Str2Int(mRecordTime)*60/*TimeUtil::StrToSecondTime(mRecordTime)*/)
		{
			ACE_Message_Block *mb = NULL;

			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//从数据接收线程取数据
			{
				if (mb->length() > 0)
				{
					if (PacketLen>=BufLen-mb->length())
					{
						mRecordFile.WriteBuf(TsBuf,PacketLen);//数据写入文件
						memset(TsBuf,0,BufLen);
						PacketLen = 0;
					}
					memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//拷贝至内存
					PacketLen +=(int) mb->length();
				}

				
				mb->release();
			}
		}
		//有手动录像任务,但手动录像任务已经超时
		else if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)>StrUtil::Str2Int(mRecordTime)*60)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]手动录制任务停止 !\n",DeviceID));
			bManualRecord=false;
			mRecordFile.Close();
			//修改手动录像文件入库  jidushan 110406
			string temptime = TimeUtil::GetCurDateTime();
			DBMANAGER::instance()->AddTempFile(mRecordFileName, temptime, m_strExpiredTime);
			SendXML(TranslateUpXML::TranslateManualRecordReport(this,mManualRecordXML,mURL));
		}
		else 
		{
			Task->msg_queue()->flush();
			OSFunction::Sleep(0,10);
		}
		
	}
	//手动录像到时之前,实时视频停止了,就将手动录像也停止
	if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)<=StrUtil::Str2Int(mRecordTime)*60)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]手动录制任务停止 !\n",DeviceID));
		string temptime = TimeUtil::GetCurDateTime();
		DBMANAGER::instance()->AddTempFile(mRecordFileName, temptime, m_strExpiredTime);
		SendXML(TranslateUpXML::TranslateManualRecordReport(this,mManualRecordXML,mURL));
	}

	delete[] TsBuf;//释放内存
	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	delete Task;

	bRun = false;

	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

	SetFinised();		//任务停止
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	if(GetVersion() == "3.0")
	{
		PROPMANAGER::instance()->FreeSetTunerDev(StrUtil::Str2Int(m_tdevid),DVBC,Freq,strServiceID);
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			PROPMANAGER::instance()->FreeSetCoderDev(codlis[i]);
		}
	}
	//g_realstreamusedev = -1;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]实时视频任务停止 !\n",DeviceID));
}

bool StreamRealtimeQueryTask::StartManualRecord(std::string ManualRecordXml)
{
	bool ret=false;
	
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]手动录制任务开始 !\n",DeviceID));

	mManualRecordXML=ManualRecordXml;

	XmlParser parser;
	parser.Set_xml(ManualRecordXml);
	pXMLNODE manualRecordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(manualRecordNode,"Time",mRecordTime);
#ifdef ZONG_JU_ZHAO_BIAO
	parser.GetAttrNode(manualRecordNode,"Lifecycle",m_strExpiredTime);

	if(m_strExpiredTime.length()<1)
	{
		m_strExpiredTime=MANURECORD_EXPIREDAYS;
	}
#endif
	
	//从配置文件中获得录像文件的实际保存位置，放在临时文件夹
	std::string realpath=PROPMANAGER::instance()->GetTempFileLoac();

	//获得日期
	std::string datetime=TimeUtil::DateTimeToString(time(0));
	datetime=datetime.substr(4, datetime.size()- 4);

	//拼接文件名：路径+通道号+任务id+日期
	std::string fileName = StrUtil::Int2Str(DeviceID) + string("_")  + TaskID + string("_") + datetime;
	fileName+=".ts";
	
	std::string fullname = realpath + fileName;		//包含实际路径的文件名
	mRecordFileName = fullname;
	//创建文件
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 创建[%s]!\n",fullname.c_str()));
	ret=mRecordFile.CreateNewFile(fullname.c_str());

	//获取共享路径
	std::string sharepath = PROPMANAGER::instance()->GetTempSharePath();
	
	//获取文件传送协议类型	wz_110310
	string fileprotocol = "";
	ret = PROPMANAGER::instance()->GetFileProtocol(GetDVBType(), fileprotocol);
	if (!ret)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取文件传送协议失败!\n"));
		return false;
	}
	mURL=fileprotocol+"://" +PROPMANAGER::instance()->GetHttpServerIP() +":"+PROPMANAGER::instance()->GetHttpServerPort() + sharepath + fileName;//录像文件存放的url
	mStartTime=TimeUtil::GetCurDateTime();
	bManualRecord=true;
	return ret;
}