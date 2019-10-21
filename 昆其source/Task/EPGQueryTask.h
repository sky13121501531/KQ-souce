///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：EPGQuery.h
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：EPG查询任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class EPGQuery : public DeviceIndependentTask
{
public:
	EPGQuery();
	EPGQuery(std::string strXML);
	virtual ~EPGQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();	
	string CreateEpgScanRet();

public:
	std::string m_strScanTime;
	string m_strDevID;
	string m_strFreq;
};