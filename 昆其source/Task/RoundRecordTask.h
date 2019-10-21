///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RoundRecordTask.h
// 创建者：gaoxd
// 创建时间：2011-04-16
// 内容描述：偱切录像任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class RoundRecordTask : public DeviceRelatedTask
{
public:
	RoundRecordTask();
	RoundRecordTask(std::string strXML);
	virtual ~RoundRecordTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual void SetDeciveID(int deviceid);
	virtual bool AddTaskXml(void);
	virtual bool DelTaskXml(void);
	virtual bool IsVedioTask(){return true;};		//音视频任务
private:
	long RoundTime;
	std::vector<std::string> DecivcXMLVec;	//硬件命令组
	size_t ChannelNum;						//轮播的频道数
	size_t ChannelIndex;					//计数器
};