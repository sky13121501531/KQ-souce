///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordUploadProcess.h
// 创建者：
// 创建时间：2017-08-29
// 内容描述：
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <vector>
#include "../Foundation/TypeDef.h"
using namespace std;
extern sRecordUploadTaskInfo m_utaskinfo;
class RecordUploadProcess : public ACE_Task<ACE_MT_SYNCH>
{
public:
	RecordUploadProcess();
	~RecordUploadProcess();
public:
	int open(void*);
	int Stop();

	virtual int svc();
public:
	int AddRecordUploadTask(sRecordUploadTaskInfo utaskinfo)
	{
		PaserTaskXml(utaskinfo.s_taskinfo);
		m_utaskinfo = utaskinfo;
		this->open(NULL);
		return 0;
	}
	bool TaskFinished(){return Finished;};
	void SetEnd(){bEnd = true;};
	void SetPause(bool pause){bPause = pause;};
	string GetTaskID(){return m_utaskinfo.s_taskid;};
	string GetActive(){return m_utaskinfo.s_active;};

	bool bFlag;
	bool Finished;
	bool bEnd;
	bool bPause;
	//
	void PaserTaskXml(string txml);
	bool SpliterRecordFile(vector <sRecordInfo>& fileVector,string StartDateTime,string EndDateTime);
	bool CreateUploadFile(string newfilepathname,vector <sRecordInfo>& vecRecordInfo);
	bool CopyUploadFile(string newpathfilename,string respathfilename,DWORD& Filesize);
	string CreateUploadResultXMLFile(string StartDateTime,string EndDateTime,DWORD Filesize,DWORD LoadTime);
	bool OpenShareServer();
	eDVBType sqltype;
	RecordUpload pUpload;
	string UpType;
	string m_StartDateTime;
	string m_EndDateTime;
};