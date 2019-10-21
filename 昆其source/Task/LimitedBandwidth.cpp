#include "LimitedBandwidth.h"
#include "./TranslateUpXML.h"

LimitedBandwidth::LimitedBandwidth(void) : DeviceIndependentTask()
{
}

LimitedBandwidth::~LimitedBandwidth(void)
{
}

LimitedBandwidth::LimitedBandwidth(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void LimitedBandwidth::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传带宽控制任务执行 !\n"));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateLimitedBandwidth(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传带宽控制任务停止 !\n"));

}
std::string LimitedBandwidth::GetObjectName()
{
	return string("LimitedBandwidth");
}
std::string LimitedBandwidth::GetTaskName()
{
	return string("录像上传带宽控制");
}
