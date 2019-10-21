///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：InfraredRemoteControlQueryTask.h
// 创建时间：2016-03-01
// 内容描述：红外遥控器 虚拟键处理类
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DeviceIndependentTask.h"

class InfraredRemoteControlQuery : public DeviceIndependentTask
{
public:
	InfraredRemoteControlQuery();
	InfraredRemoteControlQuery(std::string strXML);
	virtual ~InfraredRemoteControlQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();	
};