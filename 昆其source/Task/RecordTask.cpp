///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-01
// 内容描述：录像任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "RecordTask.h"
#include "TranslateDownXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "ace/OS.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../FileSysAccess/TSRecorder.h"
#include "Scheduler.h"
#include "../Foundation/XmlParser.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/AppLog.h"
#include "../Task/TranslateXMLForDevice.h"
#include "../DBAccess/DBManager.h"
using namespace std;
extern int g_realstreamusedev;
class Record_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Record_Task();
	~Record_Task(){};

};

Record_Task::Record_Task()
{
	msg_queue()->high_water_mark(188*10000);
	msg_queue()->low_water_mark(188*1000);
}

RecordTask::RecordTask() : DeviceRelatedTask()
{

}

RecordTask::RecordTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	m_bIsHDProgram = false;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	//
	std::string version;
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",version);            //获得标准xml中RealtimeQueryTask节点的Freq属性值
	//
	pXMLNODE recordNode;
	if(strStandardXML.find("AutoRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/AutoRecord/Record");
	}
	else if(strStandardXML.find("TaskRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TaskRecord/Record");
	}
	string OrgNetID, TsID, ServiceID,VideoPID,AudioPID,Code,Bps;
	string dayofweek,starttime,endtime,startdatetime,enddatetime,sWidth,sHeight,cdevid,tdevid;
	string strTChassisID="",strTPosition="",strTChanId="",strCChassisID="",strCPosition="",strCModuleID="",strCChanId="";
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"DeviceID",DeviceID);
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode(recordNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(recordNode,"TsID",TsID);
		parser.GetAttrNode(recordNode,"ServiceID",ServiceID);
		parser.GetAttrNode(recordNode,"VideoPID",VideoPID);
		parser.GetAttrNode(recordNode,"AudioPID",AudioPID);
		parser.GetAttrNode(recordNode,"Code",Code);
		parser.GetAttrNode(recordNode,"DayofWeek",dayofweek);
		parser.GetAttrNode(recordNode,"StartTime",starttime);
		parser.GetAttrNode(recordNode,"EndTime",endtime);
		parser.GetAttrNode(recordNode,"StartDateTime",startdatetime);
		parser.GetAttrNode(recordNode,"EndDateTime",enddatetime);
		parser.GetAttrNode(recordNode,"Width",sWidth);
		parser.GetAttrNode(recordNode,"Height",sHeight);
		parser.GetAttrNode(recordNode,"CDeviceID",cdevid);  
		parser.GetAttrNode(recordNode,"TDeviceID",tdevid);  
		parser.GetAttrNode(recordNode,"CChassisID",strCChassisID);
		parser.GetAttrNode(recordNode,"CPosition",strCPosition);
		parser.GetAttrNode(recordNode,"CModuleID",strCModuleID);
		parser.GetAttrNode(recordNode,"CTranscode",strCChanId);
		parser.GetAttrNode(recordNode,"TChassisID",strTChassisID);
		parser.GetAttrNode(recordNode,"TPosition",strTPosition);
		parser.GetAttrNode(recordNode,"TDemodulate",strTChanId);
		parser.GetAttrNode(recordNode,"Bps",Bps);
		//
		strServiceID = ServiceID;
		strVideoPID = VideoPID;
		strAudioPID = AudioPID;
		//
	}
	CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,ChannelID);

//#ifdef ZONG_JU_ZHAO_BIAO
	m_bIsHDProgram =  CHANNELINFOMGR::instance()->IsHDTV(DVBType, OrgNetID, TsID, ServiceID);
//#endif
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		if (StrUtil::Str2Int(sWidth)>720||StrUtil::Str2Int(sHeight)>576)
		{
			m_bIsHDProgram = true;
		}
	}
	else
	{
		if (StrUtil::Str2Int(sWidth)>640||StrUtil::Str2Int(sHeight)>480)
		{
			m_bIsHDProgram = true;
		}
	}
	if(ChannelID.length()<1)
	{
		ChannelID=Freq;
	}
	CHANNELINFOMGR::instance()->GetChanCodeByChanId(DVBType, ChannelID, m_strChannelName);

	TaskScheduler=new Scheduler();//任务的执行时间对象
	//设置任务执行日期时间
	if (dayofweek.length()==0)
	{
		if (TaskID=="0")//自动录像
		{
			TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
		}
		else//单次运行的任务录像
		{
			TaskScheduler->SetRunTime(startdatetime,enddatetime,enddatetime);
		}
	}
	else
	{
		long diffSec=TimeUtil::StrToSecondTime(endtime)-TimeUtil::StrToSecondTime(starttime);
		//多次运行
		
		if(dayofweek=="ALL"||dayofweek=="All")//每天运行
		{
			std::string StartDateTime=TimeUtil::GetCurDate()+" "+starttime;
			std::string EndDateTime=TimeUtil::GetCurDate()+" "+endtime;
			long cycleSec=ONEDAYLONG-diffSec;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleSec);//设置第一次运行时间
		}
		else//一周的某天运行
		{
			std::string dateTime,startDate;
			long today=TimeUtil::DateIsWeekDay(time(0));//今天星期几
			int diffDay=StrUtil::Str2Int(dayofweek)-today;//星期之间所差的天数
			if (diffDay<0)
			{
				diffDay+=7;
			}
			dateTime=TimeUtil::CalDay(time(0),diffDay);
			startDate=TimeUtil::GetDateFromDatetime(dateTime);
			std::string StartDateTime=startDate+" "+starttime;
			std::string EndDateTime=startDate+" "+endtime;
			long cycleSec=ONEDAYLONG*7-diffSec;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleSec);//设置第一次运行时间
		}
		
	}
	//if (DeviceID == 0)		//初始无通道
	//{
	//	DeviceID = -1;// 设置为-1，调度会分配一个通道
	//}
	//
	if(version == "3.0")
	{
		//
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")//重新获取逻辑通道
		{
			if(cdevid.size()<=0)
			{
				tdevid = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strTChassisID),StrUtil::Str2Int(strTPosition),StrUtil::Str2Int(strTChanId)));
				vector<int> coddevlis;
				PROPMANAGER::instance()->SeparateStrVec(strCChanId,coddevlis);
				cdevid = "";
				for(int i=0;i<coddevlis.size();i++)
				{
					if(cdevid.size() == 0)
					{
						cdevid = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),coddevlis[i]));
						parser.SetAttrNode( "TDeviceID",tdevid,recordNode );	    //设置TsQuery节点的OrgNetID属性
						parser.SetAttrNode( "CDeviceID",cdevid,recordNode);			    //设置TsQuery节点的TsID属性
						parser.SaveToString(strStandardXML);
					}
					else
					{
						cdevid += ";";
						cdevid += StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),coddevlis[i]));
					}
				}
			}
		}
		//
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				if(PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(tdevid),DVBType,Freq,ServiceID,m_bIsHDProgram,Bps,"录像进行中..."))
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
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,ServiceID,DVBType,m_bIsHDProgram,"录像进行中...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
}
RecordTask::~RecordTask()
{
}
void RecordTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]录像任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	if(g_realstreamusedev == DeviceID)
	{
		g_realstreamusedev = -1;
	}
	strDeviceXML = TranslateXMLForDevice::TranslateRecordTask(strStandardXML );//自定义标准XML到硬件接口XML转换
	
	ACE_Task<ACE_MT_SYNCH>* Task = new Record_Task;
	TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,strDeviceXML);
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);
	TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	//设置模拟无载波报警模块的录像信息
	sSignalCheck param;
	if(DVBType==ATV||DVBType==RADIO||DVBType==AM||DVBType==CTV)
	{	
		param.DeviceID	= StrUtil::Int2Str(DeviceID);
		param.dvbtype	= DVBType;
		param.Freq		= Freq;
		param.ChannelID	= ChannelID;
		//CHECKLEVELFORUNLOCK::instance()->AddRecordInfo(param);
	}

	TSRecorder record(DeviceID,strStandardXML);

	//int BufLen = 188*4096;

	//if (DVBType == RADIO  || DVBType==AM)
	//{
	//	BufLen   /= 16;
	//}

	//unsigned char* TsBuf = new unsigned char[BufLen];//缓冲区,UDPTsFetcher里已经有188*800的缓冲此处不再需要缓冲,否则录像文件在数据库的记录时间和OSD时间偏差更大,现在是2秒
	//memset(TsBuf,0,BufLen);//初始化缓冲区
	//
	//size_t PacketLen = 0;

	int nNoDataCount = 0;
	bool bHasData = true;
	while(bRun == true && IsRunning())
	{
		try
		{
			ACE_Message_Block *mb = NULL;
			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(5));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//从数据接收线程取数据
			{
				if (!bHasData)
				{
					bHasData = true;
					std::string msg = "通道：";
					msg += StrUtil::Int2Str(DeviceID);
					msg += " 接收数据成功!!";
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
				}

				if (mb->length() > 0)
				{
					record.SaveFile((unsigned char*)mb->rd_ptr(),mb->length());//数据写入文件
					//if (PacketLen>=BufLen-mb->length())
					//{
					//	record.SaveFile(TsBuf,PacketLen);//数据写入文件
					//	memset(TsBuf,0,BufLen);
					//	PacketLen = 0;
					//}
				
					//memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//拷贝至内存
					//PacketLen += mb->length();
				}
				
				mb->release();
			}
			else
			{
				if (bHasData)	//如果之前是有数据的
				{
					nNoDataCount ++;
					if (nNoDataCount >= 10)
					{
						nNoDataCount = 0;
						bHasData = false;
						std::string msg = "通道：";
						msg += StrUtil::Int2Str(DeviceID);
						msg += " 接收数据失败!!";
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
					}
				}
			}
		}
		catch(...)
		{
			string error = "录像任务异常";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );			
			OSFunction::Sleep(0,100);
		}
	}

	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	
	//读取队列中剩下的内容
	//std::cout<<"msg queue length : "<<Task->msg_queue()->message_length()<<std::endl;
	ACE_Message_Block *mb = NULL;
	ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(1));
	while (Task->getq(mb,&OutTime) != -1 && mb != NULL)//从数据接收线程取数据
	{
		if (mb->length() > 0)
		{
			record.SaveFile((unsigned char*)mb->rd_ptr(),mb->length());//数据写入文件
			//if (PacketLen>=BufLen-mb->length())
			//{
			//	bool ret = record.SaveFile(TsBuf,PacketLen);//数据写入文件
			//	memset(TsBuf,0,BufLen);
			//	PacketLen = 0;
			//}
			////std::cout<<"data in msg queue : "<<mb->length()<<std::endl;
			//memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//拷贝至内存
			//PacketLen += mb->length();
		}
		mb->release();
	}

	//if (PacketLen > 0)
	//{
	//	//std::cout<<"packet len : "<<PacketLen<<std::endl;
	//	record.SaveFile(TsBuf,PacketLen);	//数据写入文件
	//}
	
	delete Task;
	//delete[] TsBuf;//释放内存

	if (TaskScheduler != NULL)
		TaskScheduler->SetNextRunTime();//设置任务的下次执行时间
	
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	if(DVBType==ATV||DVBType==RADIO||DVBType==AM)
	{
		//CHECKLEVELFORUNLOCK::instance()->RemoveRecordInfo(param);
	}

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]录像任务停止 !\n",DeviceID));
}

std::string RecordTask::GetObjectName()
{
	if (TaskID == "0")
		return std::string("AutoRecord");
	else
		return std::string("TaskRecord");
}
string RecordTask::GetTaskName()
{
	if (TaskID == "0")
		return std::string("自动录像");
	else
		return std::string("任务录像");

}

void RecordTask::SetDeciveID( int deviceid )
{
	DeviceID=deviceid;
}

bool RecordTask::AddTaskXml( void )
{
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE node=NULL;
	if(TaskID == "0")
		node=psr.GetNodeFromPath("Msg/AutoRecord/Record");
	else
		node=psr.GetNodeFromPath("Msg/TaskRecord/Record");
	if (node)
	{
		psr.SetAttrNode("DeviceID",DeviceID,node); //分配任务的deviceid
		psr.Get_xml(strStandardXML);
	}
	//return DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
	string taskxmltodb = strStandardXML;
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		XmlParser tmpparser(taskxmltodb.c_str());
		pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/AutoRecord/Record" );
		tmpparser.SetAttrNode("TDeviceID",string(""),TsNode);
		tmpparser.SetAttrNode("CDeviceID",string(""),TsNode);
		tmpparser.SetAttrNode("DeviceID",string(""),TsNode);
		tmpparser.SaveToString(taskxmltodb);
	}
	//插入任务命令执行5次，如果全部失败，就返回false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,taskxmltodb);
		if (ret)
		{
			break;
		}
	}

	return ret;
}

bool RecordTask::DelTaskXml( void )
{
	return DBMANAGER::instance()->DeleteTask(DVBType,strStandardXML);
}

std::string RecordTask::CreateSchedulerAlarm()
{
	std::string amxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	amxml+="<Msg><RecordTaskAlarmReport></RecordTaskAlarmReport></Msg>";
	XmlParser amparser(amxml.c_str());

	std::string Uptype;
	if(DVBType==RADIO)
	{
		Uptype="RadioUp";
	}
	else if(DVBType == ATV)
	{
		Uptype="TVMonUp";
	}
	long alarmid=0;
	pXMLNODE amroot=amparser.GetRootNode();
	amparser.SetAttrNode( "Version",string("2"),amroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	amparser.SetAttrNode( "MsgID",MsgID,amroot );//消息id属性
	amparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,amroot );//当前时间
	amparser.SetAttrNode( "Type",Uptype,amroot );//消息类型
	amparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBType),amroot );//本机标识，可通过接口获得
	amparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBType),amroot );//目标机器标识
	amparser.SetAttrNode( "ReplyID",string("-1"),amroot );//回复的消息id

	string code=PROPMANAGER::instance()->GetDefSrcCode(DVBType);
	pXMLNODE amreportnode=amparser.GetNodeFromPath("Msg/RecordTaskAlarmReport");
	pXMLNODE amalarmnode=amparser.CreateNodePtr(amreportnode,"RecordTaskAlarm ");
	amparser.SetAttrNode("EquCode",string(""),amalarmnode);
	PROPMANAGER::instance()->GetAlarmID(alarmid);
	amparser.SetAttrNode("AlarmID",alarmid,amalarmnode);	
	amparser.SetAttrNode("ChCode",GetChannelID(),amalarmnode);
	amparser.SetAttrNode("TaskID",GetTaskID(),amalarmnode);
	amparser.SetAttrNode("Desc",string("录像资源冲突报警"),amalarmnode);
	amparser.SetAttrNode("Reason",string(""),amalarmnode);
	amparser.SetAttrNode("CheckDateTime",TimeUtil::DateTimeToStr(time(0)),amalarmnode);
	amparser.SetAttrNode("StartDateTime",TimeUtil::DateTimeToStr(GetStartDateTime()),amalarmnode);
	amparser.SetAttrNode("EndDateTime",TimeUtil::DateTimeToStr(GetEndDateTime()),amalarmnode);

	string TypeID="";
	if(TaskID=="0")
		TypeID = strMsgID;
	else
		TypeID = TaskID;

	std::string retXML;
	amparser.SaveToString(retXML);
	sCheckParam tmpParam;
	tmpParam.DVBType=DVBType;
	tmpParam.AlarmType = ALARM_RECORDSCHEDULER;
	tmpParam.Freq = Freq;
	tmpParam.DeviceID = StrUtil::Int2Str(DeviceID);
	tmpParam.ChannelID = ChannelID;
	tmpParam.TypeID = TypeID;
	tmpParam.TypeDesc =" 录像冲突报警";
	tmpParam.CheckTime = time(0);
	tmpParam.AlarmID = StrUtil::Long2Str(alarmid);
	DBMANAGER::instance()->AddAlarmInfo(tmpParam,"1");
	return retXML;
}


#ifdef ZONG_JU_ZHAO_BIAO
bool RecordTask::IsHDProgram()
{
	return m_bIsHDProgram;
}
#endif