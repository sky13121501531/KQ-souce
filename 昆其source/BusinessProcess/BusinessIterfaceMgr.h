///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：BusinessIterfaceMgr.h
// 创建者：jiangcheng
// 创建时间：2009-05-21
// 内容描述：业务接口层接口管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>

class DownXMLProcess;
class BusinessIterfaceMgr
{
public:
	BusinessIterfaceMgr();
	~BusinessIterfaceMgr();
public:
	int Start();
	int Stop();	
	int UplaodXML(const std::string& strXML);
private:
	void CreateInstance();
	void DestoryInstance();
private:
	DownXMLProcess* pDownXMLProcess;
};

typedef ACE_Singleton<BusinessIterfaceMgr,ACE_Mutex>  BUSINESSITERFACEMGR;