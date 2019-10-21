///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordUploadProcessMgr.h
// 创建者：
// 创建时间：2017-08-29
// 内容描述：
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <vector>
#include "../Foundation/TypeDef.h"
#include "RecordUploadProcess.h"
class RecordUploadProcessMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	RecordUploadProcessMgr();
	~RecordUploadProcessMgr();
public:
	int open(void*);
	int Stop();

	virtual int svc();
public:
	bool AddRecordUploadTask(sRecordUploadTaskInfo &utaskinfo);
private:
	bool bFlag;
};
