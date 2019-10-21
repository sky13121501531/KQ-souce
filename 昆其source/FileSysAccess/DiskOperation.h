///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DiskOperation.h
// 创建者：jiangcheng
// 创建时间：2009-06-18
// 内容描述：磁盘文件操作类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>
#include <vector>

class DiskOperation
{
public:
	DiskOperation();
	virtual ~DiskOperation();
	
	bool CheckDiskIsFullForRegular(int nTimeValue);							//指定时间间隔内检查磁盘是否已满
	bool CheckDiskIsFullForRecord(std::string& validpath,int deviceid=-1);	//检查磁盘是否已满并返回有效路径
	long GetTotalFreeSpace();	//返回所有有效路径剩余空间

	//处理过期文件
	bool ProcessInprireFiles();
	void SetLowestSpace(long lowestSpace) {mLowestSpace = lowestSpace;}	//设定磁盘最小空间(单位为Mb)

private:
	void Init();

	bool DeleteRecFile(std::string& filename);
	long CalculateFreeSpace(const std::string& dirName);
	
	bool DeleteOldestFileByDays(unsigned short delnum);		//删除最久的delnum条数据
	bool DeleteTempFile(std::string& filename);				//删除临时文件 //wz_101229 删除临时文件
	bool DeleteOldestFileInPath(unsigned short delcount, const std::string& recPath);	//删除某个路径下最老的两个文件		add by jidushan 11.05.04

	bool DeleteExpireLog();			//删除过期日志	add by jidushan 110704
	void DeleteTrashFile();			//删除垃圾录像文件
	void DeleteDbData();			//删除数据库记录

private:
	long mLowestSpace;		//磁盘最小空间(单位为Mb)
	std::vector<std::string> mDirNameVec; 
	
	time_t mPreTime;
private:
	ACE_Thread_Mutex mDelMutex;

};

typedef ACE_Singleton<DiskOperation,ACE_Mutex>  DISKOPERATION;