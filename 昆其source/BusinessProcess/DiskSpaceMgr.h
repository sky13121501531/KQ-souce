///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DiskSpaceMgr.h
// 创建者：jiangcheng
// 创建时间：2009-06-18
// 内容描述：磁盘空间管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class DiskSpaceMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DiskSpaceMgr();
	virtual ~DiskSpaceMgr();
public:
	int open(void*);

	virtual int svc();

	int Stop();
private:
	bool bFlag;
	std::string sysCleanTime;	//每天文件系统清理时间
	std::string lastRunDate;	//上次运行的时期
	int dbCleanInterval;		//每次按照数据库清理录像间隔时间(分钟)
};