///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceRelatedTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：与设备操作相关的任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "DeviceRelatedTask.h"
#include "Scheduler.h"
#include "../Foundation/OSFunction.h"

DeviceRelatedTask::DeviceRelatedTask()
{

}

DeviceRelatedTask::DeviceRelatedTask(std::string strXML) : XMLTask(strXML)
{
	DeviceID = -1;//与设备相关的DeviceID初始化为-1；各个子类需要再次设置。
}

DeviceRelatedTask::~DeviceRelatedTask()
{
	if (TaskScheduler != NULL)
		delete TaskScheduler;
	TaskScheduler = NULL;
}
bool DeviceRelatedTask::Stop()
{
	bRun = false;
	OSFunction::Sleep(0,5);
	if(IsRunning() == true)
	{
		return false;
	}
	return true;
}