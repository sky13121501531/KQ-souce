///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordTask.h
// 创建者：jiangcheng
// 创建时间：2009-06-01
// 内容描述：录像任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"


class RecordTask : public DeviceRelatedTask
{
public:
	RecordTask();
	RecordTask(std::string strXML);
	virtual ~RecordTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsVedioTask(){return true;};		//音视频任务

	virtual std::string CreateSchedulerAlarm();
	virtual bool AddTaskXml(void);
	virtual bool DelTaskXml(void);

	virtual void SetDeciveID(int deviceid);

private:
	std::string m_strChannelName;
#ifdef ZONG_JU_ZHAO_BIAO
	virtual bool IsHDProgram();
private:
	bool m_bIsHDProgram;
#endif

};