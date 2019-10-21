///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：ChannelScanQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-08
// 内容描述：频道扫描任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"
using namespace std;
class ChannelScanQueryTask : public DeviceIndependentTask
{
public:
	ChannelScanQueryTask();
	ChannelScanQueryTask(std::string strXML);
	virtual ~ChannelScanQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//实时任务
	string m_strScanTime;
	string m_strFreq;
	string m_strDevID;
	string CreatePreChanScanRet();
};