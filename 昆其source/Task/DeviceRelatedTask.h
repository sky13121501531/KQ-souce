///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceRelatedTask.h
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：与设备操作相关的任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLTask.h"

class DeviceRelatedTask : public XMLTask
{
public:
	DeviceRelatedTask();
	DeviceRelatedTask(std::string strXML);
	virtual ~DeviceRelatedTask();
public:
	std::string GetBaseObjectName() {return std::string("DeviceRelatedTask");}
	virtual bool Stop(void);			//任务停止函数
};