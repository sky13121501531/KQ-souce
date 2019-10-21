///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：OSDFormatQueryTask.h
// 创建者：jsp
// 创建时间：2017-08-1
// 内容描述：OSD参数设置查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class OSDFormatQueryTask : public DeviceIndependentTask
{
public:
	OSDFormatQueryTask();
	OSDFormatQueryTask(std::string strXML);
	virtual ~OSDFormatQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};