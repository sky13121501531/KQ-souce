#include "CardDevStatus.h"
#include "./TranslateUpXML.h"

CardDevStatusTask::CardDevStatusTask(void) : DeviceIndependentTask()
{
}

CardDevStatusTask::~CardDevStatusTask(void)
{
}

CardDevStatusTask::CardDevStatusTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void CardDevStatusTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]机箱板卡工作状态查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateCardDevStatusQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]机箱板卡工作状态查询任务停止 !\n",DeviceID));

}
std::string CardDevStatusTask::GetObjectName()
{
	return string("CardDevStatusTask");
}
std::string CardDevStatusTask::GetTaskName()
{
	return string("机箱板卡工作状态查询任务");
}
