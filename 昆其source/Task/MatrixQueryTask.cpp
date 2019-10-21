
#include "MatrixQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include <vector>
#include <iostream>

using namespace std;

MatrixQueryTask::MatrixQueryTask() : DeviceIndependentTask()
{

}

MatrixQueryTask::MatrixQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

MatrixQueryTask::~MatrixQueryTask()
{
}
void MatrixQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]矩阵切换任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateMatrixQuery(this));//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]矩阵切换任务停止 !\n",DeviceID));
}

string MatrixQueryTask::GetTaskName()
{
	return std::string("矩阵切换任务");
}
std::string MatrixQueryTask::GetObjectName()
{
	return std::string("MatrixQueryTask");
}

