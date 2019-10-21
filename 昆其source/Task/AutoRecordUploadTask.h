#pragma once
#ifndef AUTORECORDUPLOADTASK_H_H
#define AUTORECORDUPLOADTASK_H_H

#include "DeviceIndependentTask.h"
#include "../BusinessProcess/RecordUploadProcessMgr.h"
#include "../BusinessProcess/RecordUploadProcess.h"

//OSD…Ë÷√ »ŒŒÒ

class AutoRecordUploadTask : public DeviceIndependentTask
{
public:
	AutoRecordUploadTask(void);
	AutoRecordUploadTask(std::string strXML);
	virtual ~AutoRecordUploadTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

	RecordUploadProcessMgr* g_pRecordUploadProcess;
	RecordUploadProcess* F_pRecordUploadProcess;
	bool FupLoad;
	//vector<RecordUpload> vChannelInfo;
	//std::string GetTaskXMlFromUpload(std::string strStandardXML,sRecordUploadTaskInfo& uploadtask);
};

#endif