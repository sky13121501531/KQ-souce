///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RoundRecordTask.h
// 创建者：gaoxd
// 创建时间：2011-04-16
// 内容描述：偱切录像任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "RoundRecordTask.h"
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

class Round_Record_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Round_Record_Task();
	~Round_Record_Task(){};

};

Round_Record_Task::Round_Record_Task()
{
	msg_queue()->high_water_mark(188*8192);
	msg_queue()->low_water_mark(188*8192);
}

RoundRecordTask::RoundRecordTask() : DeviceRelatedTask()
{

}

RoundRecordTask::RoundRecordTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	XmlParser parser;
	parser.Set_xml(strStandardXML);

	DecivcXMLVec=TranslateXMLForDevice::TranslateRoundRecordXML(strXML);//发送给硬件的命令队列
	ChannelNum=DecivcXMLVec.size();	//轮播的频道数
	ChannelIndex=0;				//计数器
	pXMLNODE recordNode=parser.GetNodeFromPath("Msg/RoundRecord");
	string OrgNetID, TsID, ServiceID,VideoPID,AudioPID,Code;
	string dayofweek,starttime,endtime,startdatetime,enddatetime,roundtime;
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"DeviceID",DeviceID);
		parser.GetAttrNode(recordNode,"StartDateTime",startdatetime);
		parser.GetAttrNode(recordNode,"EndDateTime",enddatetime);
		parser.GetAttrNode(recordNode,"RoundTime",roundtime);

		RoundTime=TimeUtil::StrToSecondTime(roundtime);//轮播时间
	}
	pXMLNODELIST nodelist=parser.GetNodeList(recordNode);

	for (int i=0;i<nodelist->Size();i++)
	{
		
		pXMLNODE node=parser.GetNextNode(nodelist);
		string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID;
		parser.GetAttrNode(node,"Freq",Freq);
		parser.GetAttrNode(node,"OrgNetID",OrgNetID);
		parser.GetAttrNode(node,"TsID",TsID);
		parser.GetAttrNode(node,"ServiceID",ServiceID);
		parser.GetAttrNode(node,"VideoPID",VideoPID);
		parser.GetAttrNode(node,"AudioPID",AudioPID);
		if (DVBType==ATV||RADIO==DVBType||AM==DVBType||CTV==DVBType)
		{
			ChannelID=Freq;
		}
		else
		{
			CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,"",ChannelID);
		}
		if (ChannelID .length()<1)
		{
			bRun = false;
			break;
		}
	}
	TaskScheduler=new Scheduler();//任务的执行时间对象
	TaskScheduler->SetRunTime(startdatetime,enddatetime,enddatetime);
}
RoundRecordTask::~RoundRecordTask()
{
}
void RoundRecordTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]偱切录像任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	ACE_Task<ACE_MT_SYNCH>* Task = new Round_Record_Task;


	int BufLen = 188*4096;
	int BufSize = 188*1024;

	if (DVBType == RADIO  || DVBType==AM)
	{
		BufSize	 /= 16;
		BufLen   /= 16;
	}

	unsigned char* TsBuf = new unsigned char[BufLen];//缓冲区
	memset(TsBuf,0,BufLen);//初始化缓冲区

	size_t PacketLen = 0;
	TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,DecivcXMLVec[0]);
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);
	TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	TSRecorder * precord = new TSRecorder(DeviceID,DecivcXMLVec[0],TaskID);
	time_t lastTime=time(0);
	while(bRun == true && IsRunning())
	{
		try
		{
			if(time(0)-lastTime>RoundTime)
			{
				lastTime=time(0);
				ChannelIndex++;
				if(ChannelIndex>=ChannelNum)
					ChannelIndex=0;
				string currentxml=DecivcXMLVec[ChannelIndex];
				
				TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,currentxml);
				if(precord!=NULL)
				{
					delete precord;
					precord = NULL;
				}
				precord = new TSRecorder(DeviceID,currentxml,TaskID);
			}
			ACE_Message_Block *mb = NULL;

			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//从数据接收线程取数据
			{
				if (mb->length() > 0)
				{
					memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//拷贝至内存
					PacketLen += mb->length();
				}

				if (PacketLen>=BufLen-BufSize && precord != NULL)
				{
					precord->SaveFile(TsBuf,PacketLen);//数据写入文件
					memset(TsBuf,0,BufLen);
					PacketLen = 0;
				}
				mb->release();
			}
		}
		catch(...)
		{
			string error = "循切录像任务异常";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );			
			OSFunction::Sleep(0,100);
		}
	}
	if(precord!=NULL)
	{
		delete precord;
		precord = NULL;
	}

	delete[] TsBuf;//释放内存
	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	delete Task;
	if (TaskScheduler != NULL)
		TaskScheduler->SetNextRunTime();//设置任务的下次执行时间
	SetFinised();
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]偱切录像任务停止 !\n",DeviceID));
}

std::string RoundRecordTask::GetObjectName()
{
	return std::string("RoundRecordTask");
}
string RoundRecordTask::GetTaskName()
{
	return std::string("偱切录像");
}

void RoundRecordTask::SetDeciveID( int deviceid )
{
	DeviceID=deviceid;
}

bool RoundRecordTask::AddTaskXml( void )
{
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE node=psr.GetNodeFromPath("Msg/RoundRecord");
	if (node)
	{
		psr.SetAttrNode("DeviceID",DeviceID,node); //分配任务的deviceid
		psr.Get_xml(strStandardXML);
	}
	//return DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
	
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

bool RoundRecordTask::DelTaskXml( void )
{
	return DBMANAGER::instance()->DeleteTask(DVBType,strStandardXML);
}