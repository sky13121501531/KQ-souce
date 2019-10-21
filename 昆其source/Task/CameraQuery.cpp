#include "CameraQuery.h"
#include "./TranslateUpXML.h"

CameraQueryTask::CameraQueryTask(void) : DeviceIndependentTask()
{
}

CameraQueryTask::~CameraQueryTask(void)
{
}

CameraQueryTask::CameraQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void CameraQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]摄像头视频查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_FAILED;

	SendXML(TranslateUpXML::TranslateCameraQuery(this,strStandardXML));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]摄像头视频查询任务停止 !\n",DeviceID));

}
std::string CameraQueryTask::GetObjectName()
{
	return string("CameraQueryTask");
}
std::string CameraQueryTask::GetTaskName()
{
	return string("摄像头视频查询任务");
}
