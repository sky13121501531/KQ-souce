
#include "ATVChannelScanQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "Scheduler.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DBAccess/DBManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../Foundation/XmlParser.h"
#include "./TranslateXMLForDevice.h"
#include "../Foundation/OSFunction.h"
using namespace std;

ATVChannelScanQueryTask::ATVChannelScanQueryTask() : DeviceRelatedTask()
{

}

ATVChannelScanQueryTask::ATVChannelScanQueryTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	TaskScheduler = new Scheduler();
	TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
}

ATVChannelScanQueryTask::~ATVChannelScanQueryTask()
{

}

void ATVChannelScanQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道扫描任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	string strRtnXml; //XML文件内容

	bool ManualChan = PROPMANAGER::instance()->IsScanFile(DVBType);

	if (DVBType == ATV && ManualChan)
	{
		OSFunction::Sleep(1);
		XmlParser ChannelFile;
		ChannelFile.LoadFromFile("c:/vscttb/ATVChanRet.xml");
		ChannelFile.SaveToString(strRtnXml);
		SetRetValue(RUN_SUCCESS);
	}
	else if (DVBType == RADIO && ManualChan)
	{
		OSFunction::Sleep(1);
		XmlParser ChannelFile;
		ChannelFile.LoadFromFile("c:/vscttb/RADIOChanRet.xml");
		ChannelFile.SaveToString(strRtnXml);
		SetRetValue(RUN_SUCCESS);
	}
	else if (DVBType == AM && ManualChan)
	{
		OSFunction::Sleep(1);
		XmlParser ChannelFile;
		ChannelFile.LoadFromFile("c:/vscttb/AMChanRet.xml");
		ChannelFile.SaveToString(strRtnXml);
		SetRetValue(RUN_SUCCESS);
	}
	else if (DVBType == CTV && ManualChan)
	{
		OSFunction::Sleep(1);
		XmlParser ChannelFile;
		ChannelFile.LoadFromFile("c:/vscttb/CTVChanRet.xml");
		ChannelFile.SaveToString(strRtnXml);
		SetRetValue(RUN_SUCCESS);
	}
	else
	{
		bool rtn=DEVICEACCESSMGR::instance()->SendTaskMsg(DeviceID,strStandardXML,strRtnXml);

		if (!rtn)
		{
			SetRetValue(RUN_FAILED);
		}
		else
		{
			SetRetValue(RUN_SUCCESS);
		}
	}
	
	string retXML=TranslateUpXML::TranslateChannelScan(this,strRtnXml);
	SendXML(retXML);//发送xml
	//更新频道扫描结果
    CHANNELINFOMGR::instance()->UpdateChannelInfo(GetDVBType(), strRtnXml);

	bRun = false;
	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

    SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道扫描任务停止 !\n",DeviceID));
}
string ATVChannelScanQueryTask::GetTaskName()
{
	return std::string("频道扫描任务");
}
std::string ATVChannelScanQueryTask::GetObjectName()
{
	return std::string("ATVChannelScanQueryTask");
}
