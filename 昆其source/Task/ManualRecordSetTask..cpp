#include "ManualRecordSetTask.h"
#include "../Foundation/XmlParser.h"
#include "./TranslateUpXML.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"

ManualRecordSetTask::ManualRecordSetTask(void)
{
}

ManualRecordSetTask::ManualRecordSetTask( std::string strXML ):DeviceIndependentTask(strXML)
{

}
ManualRecordSetTask::~ManualRecordSetTask(void)
{
}

void ManualRecordSetTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]手动录像设置任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();

	int deviceid=0;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE manualRecordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(manualRecordNode,"DeviceID",deviceid);
	
	std::list<int> devicedlist;
	PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeQueryTask",DVBType,devicedlist);
	if(!devicedlist.empty())
	deviceid=(*devicedlist.begin());
	bool ret = PROPMANAGER::instance()->IsDeviceAvaiable(deviceid,"StreamRealtimeQueryTask",DVBType);
	if (ret == false)
	{
		RetValue=DEVICEID_UNAVAIABLE;
		SendXML(TranslateUpXML::TranslateManualRecordSet(this));
		SetFinised();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像设置任务停止 !\n",DeviceID));
		return ;
	}
	RetValue=RUN_SUCCESS;
	BUSINESSLAYOUTMGR::instance()->SetManualRecord(deviceid,DVBType,strStandardXML);	
	SendXML(TranslateUpXML::TranslateManualRecordSet(this));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像设置任务停止 !\n",DeviceID));
}

std::string ManualRecordSetTask::GetObjectName()
{
	return std::string("ManualRecordSetTask");
}

std::string ManualRecordSetTask::GetTaskName()
{
	return std::string("手动录像设置任务");
}