
#include "./BusinessLayoutMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/AppLog.h"
#include "../Task/XMLTask.h"
#include "../Task/StreamRealtimeQueryTask.h"
#include "TaskExecuteMgr.h"
#include "TaskMonitor.h"
#include "../DBAccess/DBManager.h"
#include <iostream>
#include <algorithm>

using namespace std;

bool Cmp(XMLTask* task1,XMLTask* task2);//任务队列排序规则函数

TaskMonitor::TaskMonitor(void)
{
	bFlag = false;
}

TaskMonitor::~TaskMonitor(void)
{
}
// <summary>初始化调度列表中对应的通道调度类表以及对应通道所要完成的任务名称</summary>
// <param name="devicelist">通道列表</param>
// <retvalue>成功或失败</retvalue>
// <memo>用该调度对象所调度的通道类表初始化，内部通过配置文件获取相应通道所要完成的任务名称</memo>

int TaskMonitor::Init()
{
	//PROPMANAGER::instance()->GetAllDeviceList(DeviceList);	
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	devnum += PROPMANAGER::instance()->GetTunerDevNum();//modify by hjw quality task is use channel
	devnum += 1;//modify by hjw spectrum task is use channel
	devnum = devnum + 1;//用于ChanEncryptQueryTask::TranslateEncryptXml使用逻辑通道
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		devnum = 100;
	}
	//for (list<int>::iterator pDeviceList = DeviceList.begin();pDeviceList!=DeviceList.end();++pDeviceList)
	for (int i=0;i < devnum;i++)
	{
		//初始化调度列表以及执行列表
		std::list<XMLTask*> tasklist;
		
		TaskMonitorMap.insert(make_pair(i,tasklist));

		TaskExecuteMgr* taskexecmgr = new TaskExecuteMgr();
		if (taskexecmgr == NULL)
		{
			string msg = "执行线程管理队列初始化失败";
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return -1;
		}
		TaskExecuteMgrMap.insert(make_pair(i,taskexecmgr));
	}
	t_TaskInfoMapUpdateTime = time(0);

	bFlag = true;
	return 0;
}

int TaskMonitor::open(void*)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 任务调度线程开始执行 !\n"));

	this->activate();

	for (list<int>::iterator pDeviceList = DeviceList.begin();pDeviceList!=DeviceList.end();++pDeviceList)
	{
		TaskExecuteMgrMap[*pDeviceList]->Init();
		TaskExecuteMgrMap[*pDeviceList]->open(0);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 任务执行线程开始执行 !\n"));

	return 0;
}
//任务状态更新、删除空指针或者过期任务
int TaskMonitor::svc()
{
	bFlag = true;
	while (bFlag)
	{
		OSFunction::Sleep(0,2);

		try
		{
			ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

			map<int,list<XMLTask*> >::iterator MapPtr = TaskMonitorMap.begin();
			for (;MapPtr!=TaskMonitorMap.end();++MapPtr)
			{			
				if (MapPtr->second.empty())
					continue;

				XMLTask* task = NULL;

				list<XMLTask*>::iterator Ptr = MapPtr->second.begin();
				for(;Ptr != MapPtr->second.end();)
				{
					task = *Ptr;
					if(task == NULL)
					{
						MapPtr->second.erase(Ptr++);
						continue;
					}
					//更新任务的状态值,删除过期任务
					if(task->UpdataStatus() && task->IsExpired())
					{
						TaskExecuteMgr* pTaskExecuteMgr = TaskExecuteMgrMap[task->GetDeviceID()];
						if (pTaskExecuteMgr != NULL && pTaskExecuteMgr->TaskInExectute(task) == false)
						{
							string successstr = task->GetTaskName() + string("过期，从通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]删除");
							//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSTINFO);
							APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, successstr, LOG_OUTPUT_FILE);
							MapPtr->second.erase(Ptr++);
							delete task;
							task = NULL;
						}
						else
						{
							string successstr = task->GetTaskName() + string("过期，删除时机未到，等待删除.");
							//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSTINFO);
						}
						continue;
					}
					++Ptr;
				}
			}
		}
		catch(...)
		{
			cout<<"task->UpdataStatus() error"<<endl;
		}
		OSFunction::Sleep(0,2);
		try
		{
			ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

			map<int,list<XMLTask*> >::iterator MapPtr = TaskMonitorMap.begin();
			for (;MapPtr!=TaskMonitorMap.end();++MapPtr)
			{			
				if (MapPtr->second.empty())
					continue;

				XMLTask* task = NULL;
				//对未过期的有效任务进行调度;
				list<XMLTask*>::iterator Ptr = MapPtr->second.begin();
				for(Ptr = MapPtr->second.begin();Ptr != MapPtr->second.end();++Ptr)
				{
					TaskExecuteMgr* pTaskExecuteMgr = TaskExecuteMgrMap[MapPtr->first];
					task = *Ptr;
					
					if (task == NULL)//遇到空指针直接跳出，等待下一轮判断时删除空的任务指针
						break;

					if (pTaskExecuteMgr == NULL)//如果出现该通道的执行队列指针为空的情况，直接退出循环
					{
						continue;			//是否还需要做点其他的？
					}
			
					if(task->IsReady())		//当前任务准备好，可调度运行
					{
						if (pTaskExecuteMgr->HasRunTask() == true)		
						{
/*							if (pTaskExecuteMgr->ShareDevice(task) == true)
							{
								if(!task->IsSetTaskOK())
								{
									pTaskExecuteMgr->SetTask(task);
								}
							}
							else */if (StrUtil::Str2Int(pTaskExecuteMgr->GetExecutePriority()) < StrUtil::Str2Int(task->GetPriority()))
							{
								if(pTaskExecuteMgr->StopTask())		//停止低优先级任务执行
								{
									if(!task->IsSetTaskOK())
									{
										pTaskExecuteMgr->SetTask(task);			//将该task传给执行线程
									}
								}
							}
							else if (StrUtil::Str2Int(pTaskExecuteMgr->GetExecutePriority()) == StrUtil::Str2Int(task->GetPriority())/* && \
									StrUtil::Str2Int(pTaskExecuteMgr->GetExecuteMsgID()) <= StrUtil::Str2Int(task->GetMsgID())*/)
							{
								if(pTaskExecuteMgr->StopTask())		//停止低MsgID任务执行
								{
									if(!task->IsSetTaskOK())
									{
										pTaskExecuteMgr->SetTask(task);			//将该task传给执行线程
									}
								}
							}	
						}
						else
						{
							if(!task->IsSetTaskOK())
							{
								pTaskExecuteMgr->SetTask(task);			//将该task传给执行线程
							}
						}
					}
				}
			}
		}
		catch(...)
		{
			cout<<"pTaskExecuteMgr->SetTask(task) error"<<endl;
		}
		try
		{
			if(DelTaskInfoList.size() > 0)
			{
				ACE_Guard<ACE_Thread_Mutex> guardDel(DelTaskInfoMutex);
				std::list<DelTaskInfo>::iterator Ptr = DelTaskInfoList.begin();
				for (;Ptr!=DelTaskInfoList.end();)
				{	
					OSFunction::Sleep(0,10);
					ACE_Guard<ACE_Thread_Mutex> guardMonitor(MonitorMapMutex);
					DelRecTask(Ptr->dvbtype,Ptr->taskid);
					Ptr=DelTaskInfoList.erase(Ptr);
				}
			}

			if(RecTaskInfoList.size() > 0)
			{
				ACE_Guard<ACE_Thread_Mutex> guardAdd(AddRecMutex);
				std::list<XMLTask*>::iterator Ptr = RecTaskInfoList.begin();
				for (;Ptr!=RecTaskInfoList.end();)
				{	
					OSFunction::Sleep(0,10);
					ACE_Guard<ACE_Thread_Mutex> guardMonitor(MonitorMapMutex);
					AddRecTask(*Ptr);
					Ptr=RecTaskInfoList.erase(Ptr);
				}
			}
		}
		catch(...)
		{
			cout<<"AddRecTask DelRecTask error"<<endl;
		}
		{//每秒更新任务状态查询信息
			if(1<(time(0)-t_TaskInfoMapUpdateTime)||(t_TaskInfoMapUpdateTime-time(0)>1))
			{
				ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
				UpDataTaskInfoMapShadow();
				t_TaskInfoMapUpdateTime = time(0);
			}

		}
	}

	//停止所有任务、停止执行线程
	for (list<int>::iterator pDeviceList = DeviceList.begin();pDeviceList!=DeviceList.end();++pDeviceList)
	{
		TaskExecuteMgrMap[*pDeviceList]->StopTask();
		TaskExecuteMgrMap[*pDeviceList]->Stop();
	}

	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 任务调度线程停止 !\n"));

	return 0;
}

int TaskMonitor::Stop()
{
	bFlag = false;

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 任务执行线程停止 !\n"));

	this->wait();
	return 0;
}

bool TaskMonitor::AddTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
	if(TaskMonitorMap.find(task->GetDeviceID())==TaskMonitorMap.end())
	{
		task->SetRetValue(DEVICEID_UNAVAIABLE);
		return false;
	}

	if (task->AddTaskXml() == false)	//会以事务的方式向数据库添加5次，如果失败会返回false
	{
		task->SetRetValue(DATABASEERROR);
		return false;
	}

	TaskMonitorMap[task->GetDeviceID()].push_back(task);
	TaskMonitorMap[task->GetDeviceID()].sort(Cmp);

	task->SetRetValue(SET_SUCCESS);		//设置成功

	string successstr = task->GetTaskName() + string("在通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]设置成功");
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, successstr, LOG_OUTPUT_FILE);
	//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSTINFO);

	return true;
}
bool TaskMonitor::AddRecTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
	if(TaskMonitorMap.find(task->GetDeviceID())==TaskMonitorMap.end())
	{
		task->SetRetValue(DEVICEID_UNAVAIABLE);
		return false;
	}

	if (task->AddTaskXml() == false)	//会以事务的方式向数据库添加5次，如果失败会返回false
	{
		task->SetRetValue(DATABASEERROR);
		return false;
	}

	TaskMonitorMap[task->GetDeviceID()].push_back(task);
	TaskMonitorMap[task->GetDeviceID()].sort(Cmp);

	task->SetRetValue(SET_SUCCESS);		//设置成功

	string successstr = task->GetTaskName() + string("在通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]设置成功");
	APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, successstr, LOG_OUTPUT_FILE);
	//SYSMSGSENDER::instance()->SendMsg(successstr,task->GetDVBType(),VS_MSG_SYSTINFO);

	return true;
}

bool TaskMonitor::DelTask( std::string taskid )
{
	bool ret=true;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	list<int> TempList;
	int idevnum = PROPMANAGER::instance()->GetCoderDevNum();
	idevnum += PROPMANAGER::instance()->GetQuaDevInfoNum();
	//
	for(int i=0;i<idevnum;i++)
	{
		TempList.push_back(i);
	}
	//
	//list<int>::iterator pDeviceList = DeviceList.begin();
	//for (;pDeviceList!=DeviceList.end();++pDeviceList)
	list<int>::iterator pDeviceList = TempList.begin();
	for (;pDeviceList!=TempList.end();++pDeviceList)
	{
		 int deviceid = *pDeviceList;
		 if (deviceid<=0)
			 continue;

		 for (list<XMLTask*>::iterator Ptr=TaskMonitorMap[deviceid].begin();Ptr!=TaskMonitorMap[deviceid].end();++Ptr)
		 {
			 if ((*Ptr)->GetTaskID()==taskid)
			 {
				 ret = (*Ptr)->SetExpired();
				 /*std::string strMsg = "内存中删除任务, taskid   deviceid : ";
				 strMsg += StrUtil::Int2Str( deviceid ) ;
				 APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strMsg, LOG_OUTPUT_FILE);*/
				break;
			 }
		 }
	}
	return ret;
}

bool TaskMonitor::DelTask( int deviceid,std::string taskid )
{
	bool ret=false;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
	if(TaskMonitorMap.find(deviceid)==TaskMonitorMap.end())
		return false;

	for (list<XMLTask*>::iterator Ptr=TaskMonitorMap[deviceid].begin();Ptr!=TaskMonitorMap[deviceid].end();++Ptr)
	{
		if ((*Ptr)->GetTaskID()==taskid)
		{
			ret = (*Ptr)->SetExpired();

			//std::string strMsg = "内存中删除任务  deviceid-taskid  deviceid : ";
			//strMsg += StrUtil::Int2Str( deviceid );
			//APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strMsg, LOG_OUTPUT_FILE);
		}
	}

	return ret;
}
bool TaskMonitor::AddRecTaskInfo(XMLTask* task)
{
	bool ret=true;
	ACE_Guard<ACE_Thread_Mutex> guard(AddRecMutex);
	RecTaskInfoList.push_back(task);
	return ret;
}
bool TaskMonitor::DelRecInfo(eDVBType dvbtype,std::string taskid)
{
	bool ret=true;
	ACE_Guard<ACE_Thread_Mutex> guard(DelTaskInfoMutex);
	DelTaskInfo	TaskInfo;
	TaskInfo.dvbtype = dvbtype;
	TaskInfo.taskid = taskid;
	DelTaskInfoList.push_back(TaskInfo);
	return ret;
}

bool TaskMonitor::DelRecTask(eDVBType dvbtype,std::string taskid)
{
	bool ret=true;

	list<int> TempList;

	int idevnum = PROPMANAGER::instance()->GetCoderDevNum();
	//
	for(int i=0;i<idevnum;i++)
	{
		TempList.push_back(i);
	}
	//
	list<int>::iterator pDeviceList = TempList.begin();
	for (;pDeviceList!=TempList.end();++pDeviceList)
	{
		if (*pDeviceList<=0)
			continue;

		list<XMLTask*>::iterator Ptr=TaskMonitorMap[*pDeviceList].begin();
		for (;Ptr!=TaskMonitorMap[*pDeviceList].end();++Ptr)
		{
			if ((*Ptr)->GetDVBType()==dvbtype && (*Ptr)->GetTaskID()==taskid)
			{
				ret = (*Ptr)->SetExpired();

				/*std::string strMsg = "内存中删除任务：dvbtype-taskid  taskid : ";
				strMsg += taskid ;
				strMsg += "   deviceid : ";
				strMsg += StrUtil::Int2Str( *pDeviceList );
				APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strMsg, LOG_OUTPUT_FILE);*/
			}
		}
	}
	return ret;
}

bool TaskMonitor::DelTask(eDVBType dvbtype,std::string taskid)
{
	bool ret=true;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	for(int devi = 0;devi < devnum;devi++)
	{
		list<XMLTask*>::iterator Ptr=TaskMonitorMap[devi].begin();
		for (;Ptr!=TaskMonitorMap[devi].end();++Ptr)
		{
			if ((*Ptr)->GetDVBType()==dvbtype && (*Ptr)->GetTaskID()==taskid)
			{
				ret = (*Ptr)->SetExpired();

				/*std::string strMsg = "内存中删除任务：dvbtype-taskid  taskid : ";
				strMsg += taskid ;
				strMsg += "   deviceid : ";
				strMsg += StrUtil::Int2Str( *pDeviceList );
				APPLOG::instance()->WriteLog(DATABASE, LOG_EVENT_ERROR, strMsg, LOG_OUTPUT_FILE);*/
			}
		}
	}
	return ret;
}

bool TaskMonitor::QueryTaskInfo( int deviceid,vector<sTaskInfo>& taskinfovec )
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if(TaskMonitorMap.find(deviceid)==TaskMonitorMap.end())
		return false;

	list<XMLTask*>::iterator ptr=TaskMonitorMap[deviceid].begin();
	for (;ptr!=TaskMonitorMap[deviceid].end();++ptr)
	{
		if ((*ptr) == NULL)
			continue;
		
		sTaskInfo taskinfo;

		//从调度之外的角度看，任务除了正在运行就是处于等待状态，无论是该执行而通道被占用，还是时间未到
		((*ptr)->IsRunning()) ? taskinfo.status=RUNNING : taskinfo.status=WAITING;
		
		taskinfo.taskid		= (*ptr)->GetTaskID();
		taskinfo.taskname	= (*ptr)->GetObjectName();
		taskinfo.dvbtype	= (*ptr)->GetDVBType();
		taskinfo.channelid  = (*ptr)->GetChannelID();
		taskinfo.freq		= (*ptr)->GetTaskFreq();
		taskinfo.priority	= (*ptr)->GetPriority();
		taskinfo.runpriority= (*ptr)->GetRunPriority();

		taskinfovec.push_back(taskinfo);
	}
	return true;
}
bool TaskMonitor::QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if(deviceid>=500||TaskInfoList[deviceid].taskname.size()<=0)
	{	
		return false;
	}
	taskinfo=TaskInfoList[deviceid];
	return true;
}
bool TaskMonitor::QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if(TaskMonitorMap.find(deviceid)==TaskMonitorMap.end())
		return false;

	list<XMLTask*>::iterator ptr=TaskMonitorMap[deviceid].begin();
	for (;ptr!=TaskMonitorMap[deviceid].end();++ptr)
	{
		if ((*ptr) == NULL)
			continue;

		if ((*ptr)->IsRunning() == true)
		{
			taskinfo.status		= RUNNING;
			taskinfo.taskid		= (*ptr)->GetTaskID();
			taskinfo.taskname	= (*ptr)->GetObjectName();
			taskinfo.dvbtype	= (*ptr)->GetDVBType();
			taskinfo.channelid  = (*ptr)->GetChannelID();
			taskinfo.freq		= (*ptr)->GetTaskFreq();
			taskinfo.priority	= (*ptr)->GetPriority();
			taskinfo.runpriority= (*ptr)->GetRunPriority();
#ifdef ZONG_JU_ZHAO_BIAO
			taskinfo.deviceid= (*ptr)->GetDeviceID();
#endif
			break;
		}
	}
	if (ptr == TaskMonitorMap[deviceid].end())
		return false;

	return true;
}
bool TaskMonitor::QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec)
{
	bool ret=true;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	for(int devi = 0;devi < devnum;devi++)
	{
		list<XMLTask*>::iterator Ptr=TaskMonitorMap[devi].begin();
		for (;Ptr!=TaskMonitorMap[devi].end();++Ptr)
		{
			sTaskInfo taskinfo;
			taskinfo.deviceid			= (*Ptr)->GetDeviceID();
			taskinfo.msgid				= (*Ptr)->GetMsgID();
			taskinfo.setdatetime		= (*Ptr)->GetSetDateTime();
			taskinfo.startdatetime		= (*Ptr)->GetStartDateTime();
			taskinfo.enddatetime		= (*Ptr)->GetEndDateTime();
			taskinfo.cycleinterval		= (*Ptr)->GetCycleInterval();
			taskinfo.taskid				= (*Ptr)->GetTaskID();
			taskinfo.taskname			= (*Ptr)->GetObjectName();
			taskinfo.dvbtype			= (*Ptr)->GetDVBType();
			taskinfo.channelid			= (*Ptr)->GetChannelID();
			taskinfo.freq				= (*Ptr)->GetTaskFreq();
			taskinfo.priority			= (*Ptr)->GetPriority();
			taskinfo.runpriority		= (*Ptr)->GetRunPriority();

			taskinfovec.push_back(taskinfo);
		}
	}
	return ret;
}
bool TaskMonitor::GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid,std::string videopid,std::string audiopid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if (TaskMonitorMap.size() == 0)
		return false;

	deviceid = -1;
	std::map<int,std::list<XMLTask*> >::iterator ptr= TaskMonitorMap.begin();
	for (;ptr!=TaskMonitorMap.end();++ptr)
	{
		std::list<XMLTask*>::iterator in_ptr = ptr->second.begin();
		for (;in_ptr!= ptr->second.end();++in_ptr)
		{
			if ((*in_ptr)->GetObjectName()=="AutoRecord"&&(*in_ptr)->GetDVBType()==dvbtype && (*in_ptr)->GetTaskFreq()==freq && ((*in_ptr)->GetTaskServiceID()==serviceid||serviceid.size()==0) && ((*in_ptr)->GetTaskVideoPID()==videopid||videopid.size()==0) && ((*in_ptr)->GetTaskAudioPID()==audiopid||audiopid.size()==0))
			{
				deviceid = ptr->first;
				break;
			}
		}
	}
	if (deviceid == -1)
		return false;

	return true;
}

bool TaskMonitor::GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid,std::string videopid,std::string audiopid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if (TaskMonitorMap.size() == 0)
		return false;

	deviceid = -1;
	std::map<int,std::list<XMLTask*> >::iterator ptr= TaskMonitorMap.begin();
	for (;ptr!=TaskMonitorMap.end();++ptr)
	{
		std::list<XMLTask*>::iterator in_ptr = ptr->second.begin();
		for (;in_ptr!= ptr->second.end();++in_ptr)
		{
			if ((*in_ptr)->GetObjectName()=="AutoRecord"&&(*in_ptr)->GetDVBType()==dvbtype && (*in_ptr)->GetTaskFreq()==freq && ((*in_ptr)->GetTaskServiceID()==serviceid||serviceid.size()==0) && ((*in_ptr)->GetTaskVideoPID()==videopid||videopid.size()==0) && ((*in_ptr)->GetTaskAudioPID()==audiopid||audiopid.size()==0))
			{
				deviceid = ptr->first;
				break;
			}
		}
	}
	if (deviceid == -1)
		return false;

	return true;
}

bool TaskMonitor::GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if (TaskMonitorMap.size() == 0)
		return false;

	deviceid = -1;
	std::map<int,std::list<XMLTask*> >::iterator ptr= TaskMonitorMap.begin();
	for (;ptr!=TaskMonitorMap.end();++ptr)
	{
		std::list<XMLTask*>::iterator in_ptr = ptr->second.begin();
		for (;in_ptr!= ptr->second.end();++in_ptr)
		{
			if ((*in_ptr)->GetObjectName()=="AutoRecord"&&(*in_ptr)->GetDVBType()==dvbtype && (*in_ptr)->GetChannelID()==channelid)
			{
				deviceid = ptr->first;
				break;
			}
		}
	}
	if (deviceid == -1)
		return false;

	return true;
}
bool TaskMonitor::UpdateRunPriority(std::vector<sTaskInfo> taskinfovec )
{
	return true;
}

bool TaskMonitor::SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml)
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	sTaskInfo taskinfo;
	list<XMLTask*>::iterator ptr=TaskMonitorMap[deviceid].begin();
	for (;ptr!=TaskMonitorMap[deviceid].end();++ptr)
	{
		if ((*ptr) == NULL)
			continue;
		if ((*ptr)->GetObjectName() == "StreamRealtimeQueryTask" && (*ptr)->IsRunning())
			return ((StreamRealtimeQueryTask*)(*ptr))->StartManualRecord(Taskxml);
	}
	return false;
}

bool TaskMonitor::ShareDevice(XMLTask* task)
{
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);
	if(TaskMonitorMap.find(task->GetDeviceID())==TaskMonitorMap.end())
	{
		return false;
	}
	return TaskExecuteMgrMap[task->GetDeviceID()]->ShareDevice(task);
}


bool TaskMonitor::UpDataTaskInfoMapShadow()
{
	bool ret=true;

	map<int,list<XMLTask*> >::iterator MapPtr = TaskMonitorMap.begin();
	for (;MapPtr!=TaskMonitorMap.end();++MapPtr)
	{			
		if (MapPtr->second.empty())
			continue;

		list<XMLTask*>::iterator Ptr = MapPtr->second.begin();
		if(MapPtr->first >= 500)
		{
			continue;
		}
		for(;Ptr != MapPtr->second.end();Ptr++)
		{
			if ((*Ptr)->IsRunning() == true)
			{
				TaskInfoList[MapPtr->first].deviceid		= (*Ptr)->GetDeviceID();
				TaskInfoList[MapPtr->first].msgid			= (*Ptr)->GetMsgID();
				TaskInfoList[MapPtr->first].setdatetime		= (*Ptr)->GetSetDateTime();
				TaskInfoList[MapPtr->first].startdatetime	= (*Ptr)->GetStartDateTime();
				TaskInfoList[MapPtr->first].enddatetime		= (*Ptr)->GetEndDateTime();
				TaskInfoList[MapPtr->first].cycleinterval	= (*Ptr)->GetCycleInterval();
				TaskInfoList[MapPtr->first].taskid			= (*Ptr)->GetTaskID();
				TaskInfoList[MapPtr->first].taskname		= (*Ptr)->GetObjectName();
				TaskInfoList[MapPtr->first].dvbtype			= (*Ptr)->GetDVBType();
				TaskInfoList[MapPtr->first].channelid		= (*Ptr)->GetChannelID();
				TaskInfoList[MapPtr->first].freq			= (*Ptr)->GetTaskFreq();
				TaskInfoList[MapPtr->first].priority		= (*Ptr)->GetPriority();
				TaskInfoList[MapPtr->first].runpriority		= (*Ptr)->GetRunPriority();
				TaskInfoList[MapPtr->first].status			= RUNNING;
				break;
			}
		}
	}
	return ret;
}
bool Cmp(XMLTask* task1,XMLTask* task2)
{
	if (task1 == NULL || task2 == NULL)
		return false;

	if(task1->GetPriority().length()<1 || task1->GetPriority().length() == 0)
	{
		string successstr = task1->GetTaskName() + string("优先级为空，默认为1000");
		//SYSMSGSENDER::instance()->SendMsg(successstr,task1->GetDVBType(),VS_MSG_SYSTINFO);

		task1->SetPriority("1000");
	}
	if(task2->GetPriority().length()<1 || task2->GetPriority().length() == 0)
	{
		string successstr = task2->GetTaskName() + string("优先级为空，默认为1000");
		//SYSMSGSENDER::instance()->SendMsg(successstr,task2->GetDVBType(),VS_MSG_SYSTINFO);

		task2->SetPriority("1000");
	}

	//优先级别比较
	int Priority_1 = StrUtil::Str2Int(task1->GetPriority());
	int Priority_2 = StrUtil::Str2Int(task2->GetPriority());

	time_t SetTime_1 = TimeUtil::StrToDateTime(task1->GetSetDateTime());
	time_t SetTime_2 = TimeUtil::StrToDateTime(task2->GetSetDateTime());

	if (Priority_1 > Priority_2)
		return true;
	else if (Priority_1 < Priority_2)
		return false;
	else if (Priority_1 == Priority_2 && SetTime_1 > SetTime_2)
		return true;
	else if (Priority_1 == Priority_2 && SetTime_1 <= SetTime_2)
		return false;

	return false;
}
int  TaskMonitor::UnGetDevIdSize()
{
	return RecTaskInfoList.size();
}
bool TaskMonitor::IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid)
{
	bool isexist = false;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if (TaskMonitorMap.size() == 0)
	{
		return isexist;
	}
	std::map<int,std::list<XMLTask*> >::iterator ptr= TaskMonitorMap.begin();
	for (;ptr!=TaskMonitorMap.end();++ptr)
	{
		std::list<XMLTask*>::iterator in_ptr = ptr->second.begin();
		for (;in_ptr!= ptr->second.end();++in_ptr)
		{
			if ((*in_ptr)->GetDVBType()==dvbtype && (*in_ptr)->GetTaskID()==taskid)
			{
				isexist = true;
				break;
			}
		}
	}	return isexist;
}
bool TaskMonitor::IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq)
{
	bool isexist = false;
	ACE_Guard<ACE_Thread_Mutex> guard(MonitorMapMutex);

	if (TaskMonitorMap.size() == 0)
	{
		return isexist;
	}
	std::map<int,std::list<XMLTask*> >::iterator ptr= TaskMonitorMap.begin();
	for (;ptr!=TaskMonitorMap.end();++ptr)
	{
		std::list<XMLTask*>::iterator in_ptr = ptr->second.begin();
		for (;in_ptr!= ptr->second.end();++in_ptr)
		{
			if ((*in_ptr)->GetObjectName()=="AutoRecord"&&(*in_ptr)->GetDVBType()==dvbtype && (*in_ptr)->GetTaskFreq()==freq && (!((*in_ptr)->IsExpired())))
			{
				isexist = true;
				break;
			}
		}
	}
	return isexist;
}