#include "ControlCardTimeQuery.h"
#include "./TranslateUpXML.h"

ControlCardTimeQuery::ControlCardTimeQuery(void) : DeviceIndependentTask()
{
}

ControlCardTimeQuery::~ControlCardTimeQuery(void)
{
}

ControlCardTimeQuery::ControlCardTimeQuery(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void ControlCardTimeQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 主控卡当前时间查询 !\n"));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateControlCardTimeQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)主控卡当前时间查询 !\n"));

}
std::string ControlCardTimeQuery::GetObjectName()
{
	return string("ControlCardTimeQuery");
}
std::string ControlCardTimeQuery::GetTaskName()
{
	return string("主控卡当前时间查询");
}
