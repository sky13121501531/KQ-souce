///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordSetTask.h
// 创建者：jiangcheng
// 创建时间：2009-06-01
// 内容描述：自动录像设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RecordSetTask : public DeviceIndependentTask
{
public:
	RecordSetTask();
	RecordSetTask(std::string strXML);
	virtual ~RecordSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
private:
	bool CreateQualityTask(string freq,string CheckInterval,string TunerIndex);
	bool TestTaskIsEffective(std::vector<std::string> vecTaskXml);
	int GetRetChannelSetSouce(std::string strXML);
};