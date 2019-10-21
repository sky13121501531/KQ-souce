///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceIndependentTask.h
// 创建者：jiangcheng
// 创建时间：2009-05-26
// 内容描述：与设备操作无关的任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLTask.h"

class DeviceIndependentTask : public XMLTask
{
public:
	DeviceIndependentTask();
	DeviceIndependentTask(std::string strXML);
	virtual ~DeviceIndependentTask();
public:
	bool SendCASXML(const std::string& dstUrl,const std::string& upXML);
	std::string GetBaseObjectName() {return std::string("DeviceIndependentTask");}
	virtual bool Stop(void){return true;};			//任务停止函数，设备无关任务不能停止
	virtual bool IsRealTimeTask(){return true;};	//设备无关任务需要实时执行
};