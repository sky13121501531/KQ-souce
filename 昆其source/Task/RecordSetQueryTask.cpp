#include "RecordSetQueryTask.h"
#include "../DBAccess/DBManager.h"
#include "TranslateUpXML.h"
using namespace std;
RecordSetQueryTask::RecordSetQueryTask() : DeviceIndependentTask()
{

}

RecordSetQueryTask::RecordSetQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RecordSetQueryTask::~RecordSetQueryTask()
{

}
void RecordSetQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]自动录像设置查询任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();

	std::vector<std::string>  vctXML;
	if( DBMANAGER::instance()->QueryTask(DVBType,vctXML) ==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateRecordSetQuery(this,vctXML)); //发送到前端
	
	SetFinised(); 
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]自动录像设置查询任务停止 !\n",DeviceID));
	return;
}

string RecordSetQueryTask::GetTaskName()
{
	return "自动录像设置查询任务";
}
std::string RecordSetQueryTask::GetObjectName()
{
	return m_strObjectName;
}

