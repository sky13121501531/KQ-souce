///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：BusinessLayoutMgr.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-27
// 内容描述：业务调度规划层接口管理类
///////////////////////////////////////////////////////////////////////////////////////////
#include "BusinessLayoutMgr.h"
#include "../Foundation/PropManager.h"
#include "../Task/XMLTask.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/StrUtil.h"
#include "RecordUploadProcessMgr.h"
#include "ChannelInfoMgr.h"
int g_iCurRealChan = -1;
RecordUploadProcessMgr* g_pRecordUploadProcessMgr;
BusinessLayoutMgr::BusinessLayoutMgr()
{
	CreateInstance();
}

BusinessLayoutMgr::~BusinessLayoutMgr()
{
	DestoryInstance();
}

void BusinessLayoutMgr::CreateInstance()
{
	pTaskMonitor = new TaskMonitor;
	pTaskRealTimeMonitor = new TaskRealTimeMonitor;
	pDiskSpaceMgr = new DiskSpaceMgr;
	pClientInfoMgr = new ClientInfoMgr;
	pDeviceManager = new DeviceManager;
	g_pRecordUploadProcessMgr = new RecordUploadProcessMgr;
}

void BusinessLayoutMgr::DestoryInstance()
{
	if (NULL != pTaskMonitor)
	{
		delete pTaskMonitor;
		pTaskMonitor = NULL;
	}

	if (NULL != pTaskRealTimeMonitor)
	{
		delete pTaskRealTimeMonitor;
		pTaskRealTimeMonitor = NULL;
	}

	if (NULL != pDiskSpaceMgr)
	{
		delete pDiskSpaceMgr;
		pDiskSpaceMgr = NULL;
	}

	if (NULL != pClientInfoMgr)
	{
		delete pClientInfoMgr;
		pClientInfoMgr = NULL;
	}

	if (NULL != pDeviceManager)
	{
		delete pDeviceManager;
		pDeviceManager = NULL;
	}
	if (NULL != g_pRecordUploadProcessMgr)
	{
		delete g_pRecordUploadProcessMgr;
		g_pRecordUploadProcessMgr = NULL;
	}
}

int BusinessLayoutMgr::Start()
{
	//设备相关任务调度执行模块初始化
	if (pTaskMonitor->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)调度执行模块初始化错误!\n"));
		return -1;
	}
	else
	{
		pTaskMonitor->open(0);
	}
	
	if (pTaskRealTimeMonitor != NULL)
		pTaskRealTimeMonitor->open(0); //设备无关任务调度执行模块初始化
	
	if (pDiskSpaceMgr != NULL)	
		pDiskSpaceMgr->open(0);			//磁盘管理模块初始化执行
	
	if (pClientInfoMgr != NULL)
		pClientInfoMgr->open(0);		//用户信息管理线程启动
	
	if (pDeviceManager != NULL)
		pDeviceManager->open(0);		//设备管理（重启线程）
	
	if (g_pRecordUploadProcessMgr != NULL)
		g_pRecordUploadProcessMgr->open(0);		

	return 0;
}

int BusinessLayoutMgr::Stop()
{
	if (NULL != pTaskMonitor)
		pTaskMonitor->Stop();

	if (NULL != pTaskRealTimeMonitor)
		pTaskRealTimeMonitor->Stop();

	if (NULL != pDiskSpaceMgr)
		pDiskSpaceMgr->Stop();

	if (NULL != pClientInfoMgr)
		pClientInfoMgr->Stop();

	if (NULL != pDeviceManager)
		pDeviceManager->Stop();

	if (NULL != g_pRecordUploadProcessMgr)
		g_pRecordUploadProcessMgr->Stop();

	return 0;
}
bool BusinessLayoutMgr::AddDeviceSocket(ACE_SOCK_Stream socket)
{
	return false;
}
///////////////////任务调度相关接口/////////////////////////

bool BusinessLayoutMgr::AddTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->GetBaseObjectName() == "DeviceIndependentTask")
	{
		return pTaskRealTimeMonitor->AddTask(task);	//任务添加通道无关执行队列中
	}
	//与通道相关需要调度的任务
	if(	TaskChannleIsValid(task)		&&		//判断音视频任务通道是否合法
		ProcessTaskDeviceID(task)		&&		//处理任务通道（判断合法性或者自动分配通道）
		ProcessTaskRunPriority(task)	&&		//处理任务运行优先级
		ProcessRealtimeTask(task)		&&		//处理实时任务，判断是否可共享执行，是否有更高优先级占用
		ProcessAutorecordTask(task))			//处理自动录像任务
	{
		return pTaskMonitor->AddTask(task);
	}
	else
	{
		return pTaskRealTimeMonitor->AddTask(task);			//低优先级任务直接进入实施执行队列运行，返回相应的XML
	}
	return false;
}
bool BusinessLayoutMgr::AddRecTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->GetBaseObjectName() == "DeviceIndependentTask")
	{
		return pTaskRealTimeMonitor->AddTask(task);	//任务添加通道无关执行队列中
	}
	//与通道相关需要调度的任务
	if(	TaskChannleIsValid(task)		&&		//判断音视频任务通道是否合法
		ProcessTaskDeviceID(task)		&&		//处理任务通道（判断合法性或者自动分配通道）
		ProcessTaskRunPriority(task)	&&		//处理任务运行优先级
		ProcessRealtimeTask(task)		&&		//处理实时任务，判断是否可共享执行，是否有更高优先级占用
		ProcessAutorecordTask(task))			//处理自动录像任务
	{
		return pTaskMonitor->AddRecTaskInfo(task);
	}
	else if((task->GetObjectName()!="AutoRecord")&& (task->GetObjectName()!="TaskRecord"))//录像没有资源不用执行
	{
		return pTaskRealTimeMonitor->AddTask(task);			//低优先级任务直接进入实施执行队列运行，返回相应的XML
	}
	return false;
}

bool BusinessLayoutMgr::TaskChannleIsValid(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->IsVedioTask() && task->GetChannelID().length()<1)
	{
		string msg = "视频任务频道非法，频道ID为空";
		//SYSMSGSENDER::instance()->SendMsg(msg,task->GetDVBType(),VS_MSG_SYSALARM);

		task->SetRetValue(CHANNELID_UNAVAIABLE);
		return false;
	}

	return true;
}
bool BusinessLayoutMgr::ProcessTaskDeviceID(XMLTask* task)
{
	if(task == NULL)
		return false;

	//处理页面设置实时视频通道
	if(task->GetObjectName()=="StreamRealtimeQueryTask"||task->GetObjectName()=="StreamRealtimeRoundQueryTask")
	{
		list<int> DevList;
		int i_index = 0;
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else if(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9"||PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")
		{
			PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeRoundQueryTask", task->GetDVBType(), DevList);
			std::list<int>::iterator pt=DevList.begin();
			for(;pt!=DevList.end();pt++)
			{
				if(g_iCurRealChan != *pt)
				{
					i_index = *pt;
					g_iCurRealChan = i_index;
					break;
				}
			}
		}
		else
		{
			PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
			list<int>::iterator iter_list = DevList.begin();
			if(DevList.size()>0)
			{
				i_index = *iter_list;
			}
		}
		if(task->GetDVBType() == DVBC)//总局有线数字前端通道设置接口有停用
		{
			if (task->GetDeviceID() < 0)
			{
				task->SetDeciveID(i_index);
			}
			return true;
		}
		task->SetDeciveID(i_index);
	}
	else if(task->GetObjectName()=="QualityRealtimeQueryTask" || task->GetObjectName()=="QualityTask")
	{
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				int i_index = PROPMANAGER::instance()->GetCoderDevNum();//在任务队列TaskMonitorMap中指标通道排在转码设备后面,否则都有0,1等会产生通道混淆,在执行时从新取通道或减去GetCoderDevNum,MER-BER肯定需要从新取
				task->SetDeciveID(i_index+task->GetDeviceID());
				return true;
			}
		}
		else
		{
			list<int> DevList;
			int i_index = 0;
			PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
			list<int>::iterator iter_list = DevList.begin();
			if((task->GetDVBType() == DVBC)||(task->GetDVBType() == CTTB))//数字信号取非Mer-Ber设备,和模拟信号一致
			{
				for(iter_list; iter_list != DevList.end(); iter_list++)
				{
					i_index = *iter_list;
					Quality_DevInfo* p = NULL;
					p = PROPMANAGER::instance()->GetQuaDevInfo(i_index);
					if (p->s_devType != "Mer-Ber")
					{
						break;
					}
				}
			}
			else
			{
				if(DevList.size()>0)
				{
					i_index = *iter_list;
				}
			}
			i_index += PROPMANAGER::instance()->GetCoderDevNum();//在任务队列TaskMonitorMap中指标通道排在转码设备后面,否则都有0,1等会产生通道混淆,在执行时从新取通道或减去GetCoderDevNum,MER-BER肯定需要从新取
			task->SetDeciveID(i_index);
			return true;
		}
	}
	else if(task->GetObjectName()=="SpectrumScanTask" || task->GetObjectName()=="SpectrumTask")
	{
		list<int> DevList;
		int i_index = 0;
		PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
		list<int>::iterator iter_list = DevList.begin();
		i_index = *iter_list;
		i_index += PROPMANAGER::instance()->GetCoderDevNum();//在任务队列TaskMonitorMap中频谱通道排在转码设备+指标通道后面,否则都有0,1等会产生通道混淆,在执行时从新取通道
		i_index += PROPMANAGER::instance()->GetQuaDevInfoNum();
		task->SetDeciveID(i_index);
		return true;
	}
	else if(task->GetObjectName()=="ChannelScanQueryTask")
	{//线程处理使用调频通道1
		return true;
	}
	else if(task->GetObjectName()=="AutoRecord"||task->GetObjectName()=="TaskRecord")
	{
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (task->GetDeviceID() >= 0)		//有通道判断
			{
				if (false == PROPMANAGER::instance()->IsDeviceAvaiable(task->GetDeviceID()))
				{
					task->SetRetValue(DEVICEID_UNAVAIABLE);
					string errstr = task->GetTaskName() + string("通道[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]不存在或无法完成任务");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);

					errstr =task->GetTaskName() + string("设置失败");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				int devid = task->GetDeviceID();
				if(false == PROPMANAGER::instance()->GetDevID(devid,task->GetDVBType(),StrUtil::Str2Int(task->GetPriority()),task->GetTaskFreq(),task->GetTaskServiceID(),task->GetTaskID(),task->IsHDProgram()))
				{
					string errstr = task->GetTaskName()+task->GetTaskFreq()+ string("任务没有资源,任务没有执行!");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				task->SetDeciveID(task->GetDeviceID());
				return true;
			}
			else	//无通道处理
			{
				int devid = -1;
				if(false == PROPMANAGER::instance()->GetDevID(devid,task->GetDVBType(),StrUtil::Str2Int(task->GetPriority()),task->GetTaskFreq(),task->GetTaskServiceID(),task->GetTaskID(),task->IsHDProgram()))
				{
					string errstr = task->GetTaskName()+task->GetTaskFreq()+ string("]没有资源使用,任务没有执行!");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				task->SetDeciveID(devid);
				return true;
			}
		}
	}

	return true;
}
bool BusinessLayoutMgr::ProcessTaskRunPriority(XMLTask* task)
{
	if(task == NULL)
		return false;

	return true;
}
bool BusinessLayoutMgr::ProcessRealtimeTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->IsRealTimeTask() == false) //非实时任务
		return true;

	if(pTaskMonitor->ShareDevice(task) == true)
		return true;

	vector<sTaskInfo> taskinfovec;
	pTaskMonitor->QueryTaskInfo(task->GetDeviceID(),taskinfovec);

	vector<sTaskInfo>::iterator ptr = taskinfovec.begin();
	for (;ptr!=taskinfovec.end();++ptr)
	{
		if ((*ptr).status == RUNNING)
			break;
	}
	if (ptr!=taskinfovec.end() && StrUtil::Str2Int((*ptr).priority) > StrUtil::Str2Int(task->GetPriority()))	//高优先级任务占用
	{
		task->SetRetValue(PREFERENTIALTASK_USE);
		return false;
	}
	return true;
}
bool BusinessLayoutMgr::ProcessAutorecordTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	std::string tasktype = task->GetObjectName();
	if (tasktype!="AutoRecord")
		return true;

	std::vector<sTaskInfo> taskinfovec;
	pTaskMonitor->QueryTaskInfo(task->GetDeviceID(),taskinfovec);

	std::vector<sTaskInfo>::iterator Ptr=taskinfovec.begin();
	for (;Ptr!=taskinfovec.end();++Ptr)
	{
		if ((*Ptr).taskname=="AutoRecord")
			break;
	}
	if (Ptr != taskinfovec.end())
	{
		if (StrUtil::Str2Int(task->GetPriority()) >= StrUtil::Str2Int((*Ptr).priority)) //要设置的自动录像任务优先级别大于等于其他自动录像任务的优先级别
		{
			return pTaskMonitor->DelTask(task->GetDeviceID(),"0");	//历史任务设置过期
		}
		else
		{
			task->SetRetValue(PREFERENTIALTASK_USE);
			return false;
		}
	}
	//如果没有找到自动录像任务直接设置即可
	return true;
}
bool BusinessLayoutMgr::DeviceStatusQuery( int deviceid,int& DeviceStatus,std::string& TaskName )
{
	//判断默认通道号是否合法
	//list<int> DeviceList;
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	//list<int>::iterator pDeviceList = DeviceList.begin();
	//for (;pDeviceList!=DeviceList.end();++pDeviceList)
	int devi = 0;
	for(;devi < devnum;devi++)
	{
		if (deviceid == devi)
			break;
	}
	if (devi == devnum)//该默认通道不在该调度所包含的通道组中
	{
		DeviceStatus = 1;
		TaskName = string("无板卡");
		return true;
	}
	std::vector<sTaskInfo> taskinfo;
	pTaskMonitor->QueryTaskInfo(deviceid,taskinfo);    //查询该通道的任务信息
	if (taskinfo.size()>0) 
	{
		size_t i=0;
		for (;i!=taskinfo.size();++i)
		{
			if(taskinfo[i].status==RUNNING)
			{
				DeviceStatus = 2;
				TaskName = taskinfo[i].taskname;      //获得任务名称
				break;
			}
		}
		if (i==taskinfo.size())
		{
			DeviceStatus = 0;
			TaskName = string("正常状态");
		}	
	}
	else
	{
		DeviceStatus = 0;//设备处于空闲状态
        TaskName = string("正常状态");
	}
	return true;
}

bool BusinessLayoutMgr::DelTask( std::string taskid )
{
#ifdef ZONG_JU_ZHAO_BIAO
	//总局入网
	//如果taskid为空，则可能删除掉所有的自动录像，因为自动录像的taskid均为空
	if (taskid.length()<1)
		return false;
#endif
	
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(taskid);
}

bool BusinessLayoutMgr::DelTask( int deviceid,std::string taskid )
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(deviceid,taskid);
}

bool BusinessLayoutMgr::DelTask(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(dvbtype,taskid);
}

bool BusinessLayoutMgr::DelRecInfo(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelRecInfo(dvbtype,taskid);
}

bool BusinessLayoutMgr::SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->SetManualRecord(deviceid,dvbtype,Taskxml);
}
bool BusinessLayoutMgr::QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryRunTaskInfo(deviceid,taskinfo);
}
bool BusinessLayoutMgr::QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryRecRunTaskInfo(deviceid,taskinfo);
}
bool BusinessLayoutMgr::QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryAllTaskInfo(dvbtype,taskinfovec);
}
bool BusinessLayoutMgr::GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid,std::string videopid,std::string audiopid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByFreq(dvbtype,freq,deviceid,serviceid,videopid,audiopid);
}

bool BusinessLayoutMgr::GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid,std::string videopid,std::string audiopid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByFreqEx(dvbtype,freq,deviceid,isHD,serviceid,videopid,audiopid);
}

bool BusinessLayoutMgr::GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByChannelID(dvbtype,channelid,deviceid);
}

//音视频用户管理
bool BusinessLayoutMgr::AddUser(ACE_SOCK_Stream client,string& xml)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->AddUser(client,xml);
}
bool BusinessLayoutMgr::GetUser(enumDVBType DvbType,std::vector<sVedioUserInfo>& uservec)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->GetUser(DvbType,uservec);
}
bool BusinessLayoutMgr::StopUser(int deviceid)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->StopUser(deviceid);
}


#ifdef ZONG_JU_ZHAO_BIAO

bool BusinessLayoutMgr::QueryTaskInfo( enumDVBType eDvbType, int deviceId, std::vector<sTaskInfo>& taskInfoVec )
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryTaskInfo(deviceId, taskInfoVec);
}

bool BusinessLayoutMgr::IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->IsExistTaskByTaskid(dvbtype,taskid);
}
bool BusinessLayoutMgr::IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->IsExistRecTaskByFreq(dvbtype,freq);
}

#endif