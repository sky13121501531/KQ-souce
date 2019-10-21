#include "EquipmentHeartBeatQueryTask.h"
#include "./TranslateUpXML.h"

EquipmentHeartBeatQueryTask::EquipmentHeartBeatQueryTask(void) : DeviceIndependentTask()
{
}

EquipmentHeartBeatQueryTask::~EquipmentHeartBeatQueryTask(void)
{
}

EquipmentHeartBeatQueryTask::EquipmentHeartBeatQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void EquipmentHeartBeatQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备连通状态查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateEquipmentHeartBeatQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设备连通状态查询任务停止 !\n",DeviceID));

}
std::string EquipmentHeartBeatQueryTask::GetObjectName()
{
	return string("EquipmentHeartBeatQueryTask");
}
std::string EquipmentHeartBeatQueryTask::GetTaskName()
{
	return string("设备连通状态查询任务");
}
