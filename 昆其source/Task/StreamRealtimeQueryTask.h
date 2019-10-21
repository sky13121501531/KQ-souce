///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：StreamRealtimeQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-08
// 内容描述：实时视频任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
#include <string>
#include "ace/Task.h"
#include "ace/OS.h"
#include "../Foundation/FileOperater.h"


class StreamRealtimeQueryTask : public DeviceRelatedTask
{
public:
	StreamRealtimeQueryTask();
	StreamRealtimeQueryTask(std::string strXML);
	virtual ~StreamRealtimeQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName(){return std::string("StreamRealtimeQueryTask");};
	virtual std::string GetTaskName(){return std::string("实时视频任务");};
	virtual bool IsRealTimeTask(){return true;};	
	virtual bool IsVedioTask(){return true;};
	bool StartManualRecord(std::string ManualRecordXml);//通知开始手动录像接口
private:
	bool bManualRecord;		//是否有手动录像任务
	std::string mRecordTime,mManualRecordXML,mURL;
	std::string mStartTime;
	std::string mBps;
	FileOperater mRecordFile;

	//手动录像文件改为在tempfile表中存储，sRecordInfo不需要了 jidushan 110406
	//sRecordInfo mRecordInfo;
	std::string mRecordFileName;
	std::string m_cdevid,m_tdevid;

#ifdef ZONG_JU_ZHAO_BIAO
	std::string m_strBeginTime;
	int m_nRunTime;

	std::string m_strExpiredTime;
#endif
};
