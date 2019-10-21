///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordSetQueryTask.h
// 创建者：jsp
// 创建时间：2017-07-28
// 内容描述：自动录像设置查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RecordSetQueryTask : public DeviceIndependentTask
{
public:
	RecordSetQueryTask();
	RecordSetQueryTask(std::string strXML);
	virtual ~RecordSetQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};