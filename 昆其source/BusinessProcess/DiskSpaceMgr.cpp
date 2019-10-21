///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DiskSpaceMgr.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-18
// 内容描述：磁盘空间管理类
///////////////////////////////////////////////////////////////////////////////////////////
#include "DiskSpaceMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../FileSysAccess/DiskOperation.h"

DiskSpaceMgr::DiskSpaceMgr()
{
	bFlag = false;
	sysCleanTime = PROPMANAGER::instance()->GetSystemCleanTime();//获得每天的清理时间
	dbCleanInterval = StrUtil::Str2Int(PROPMANAGER::instance()->GetDBCleanInterval());
}

DiskSpaceMgr::~DiskSpaceMgr()
{

}

int DiskSpaceMgr::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 磁盘空间管理线程开始执行 !\n"));
	this->activate();
	return 0;
}

int DiskSpaceMgr::svc()
{
	bFlag = true;

	while (bFlag)
	{
		OSFunction::Sleep(0,20);

		if (!sysCleanTime.empty())
		{
			time_t currentTime = time(0);//当前时间
			std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
			std::string strDay = TimeUtil::GetCurDate();
			strDay += " ";
			strDay += sysCleanTime;

			if (abs(TimeUtil::DiffMinute(strCurTime,strDay)) < 3)//与清理时间上下不超过3分钟
			{
				if (lastRunDate.compare(TimeUtil::GetCurDate()) != 0)
					DISKOPERATION::instance()->ProcessInprireFiles();//清理过期文件
				lastRunDate= TimeUtil::GetCurDate();//更新上次清理时间
			}

		}

		DISKOPERATION::instance()->CheckDiskIsFullForRegular(dbCleanInterval*60);//磁盘空间检查

	}
	
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 磁盘空间管理线程停止执行 !\n"));

	return 0;
}

int DiskSpaceMgr::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}