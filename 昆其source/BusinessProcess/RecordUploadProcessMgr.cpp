
#include "RecordUploadProcessMgr.h"
#include "../Foundation/OSFunction.h"
#include "RecordUploadProcess.h"
#include "../DBAccess/DBManager.h"

ACE_Thread_Mutex TaskExecuteMutex;
std::vector<RecordUploadProcess*> mRecordUploadTaskVec;
RecordUploadProcessMgr::RecordUploadProcessMgr()
{
	bFlag = false;
}

RecordUploadProcessMgr::~RecordUploadProcessMgr()
{

}

int RecordUploadProcessMgr::open(void*)
{
	activate();
	return 0;
}

int RecordUploadProcessMgr::svc()
{
	bFlag = true;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传管理线程开始执行 !\n"));
	//
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传管理线程开始加载数据库上传任务...!\n"));
	vector<sRecordUploadTaskInfo> VecUploadtask;
	DBMANAGER::instance()->QueryUploadTask(VecUploadtask);
	for(int i=0;i<VecUploadtask.size();i++)
	{
		AddRecordUploadTask(VecUploadtask[i]);
	}
	//
	while (bFlag)
	{		
		OSFunction::Sleep(0,100);

		ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);

		std::vector<RecordUploadProcess*>::iterator ptr = mRecordUploadTaskVec.begin();
		for (;ptr!=mRecordUploadTaskVec.end();)
		{
			if ((*ptr)!=NULL && (*ptr)->TaskFinished())
			{
				(*ptr)->Stop();
				delete (*ptr);
				(*ptr) = NULL;
				ptr = mRecordUploadTaskVec.erase(ptr);
			}
			else
			{
				++ptr;
			}
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 录像上传管理线程停止执行 !\n"));
	return 0;
}

bool RecordUploadProcessMgr::AddRecordUploadTask(sRecordUploadTaskInfo &utaskinfo)
{
	ACE_Guard<ACE_Thread_Mutex> guard(TaskExecuteMutex);
	bool bExist = false;
	std::vector<RecordUploadProcess*>::iterator ptr = mRecordUploadTaskVec.begin();
	for (;ptr!=mRecordUploadTaskVec.end();ptr++)
	{
		if ((*ptr)->GetTaskID()==utaskinfo.s_taskid)
		{
			bExist = true;
			if(utaskinfo.s_active == "1")
			{
				(*ptr)->SetEnd();
			}
			else if(utaskinfo.s_active == "2")
			{
				(*ptr)->SetPause(true);
			}
			else if(utaskinfo.s_active == "0")
			{
				(*ptr)->SetPause(false);
			}
		}
	}
	if(!bExist)
	{
		RecordUploadProcess* pRecordUploadProcess = new RecordUploadProcess();
		if (pRecordUploadProcess != NULL)
		{
			pRecordUploadProcess->AddRecordUploadTask(utaskinfo);	
			mRecordUploadTaskVec.push_back(pRecordUploadProcess);
		}
	}
	return true;
}
int RecordUploadProcessMgr::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}