///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：ATVChannelScanQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-08
// 内容描述：频道扫描任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
using namespace std;
class ATVChannelScanQueryTask : public DeviceRelatedTask
{
public:
	ATVChannelScanQueryTask();
	ATVChannelScanQueryTask(std::string strXML);
	virtual ~ATVChannelScanQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};