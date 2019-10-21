///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TaskMonitor.h
// 创建者：gaoxd
// 创建时间：2009-05-26
// 内容描述：任务调度类
// 使用说明：调度类将执行类作为成员属性使用，使用时需要用std::list<int/*通道号*/>变量初始化。
//			 初始化后会根据list中的设备号初始化相应的执行类对象。
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>
#include <map>
#include <string>
#include "ace/Task.h"
#include <vector>
#include "../Foundation/TypeDef.h"
class XMLTask;
class TaskExecuteMgr;
struct DelTaskInfo
{
	eDVBType dvbtype;
	std::string taskid;
};
class TaskMonitor: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskMonitor(void);
	~TaskMonitor(void);

public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

public:
	bool AddTask(XMLTask* task);											//添加任务
	bool AddRecTask(XMLTask* task);											//添加任务
	bool AddRecTaskInfo(XMLTask* task);									    //添加任务
	bool DelTask(std::string taskid);										//删除所有通道内的指定任务
	bool DelTask(int deviceid,std::string taskid);							//删除指定通道的指定任务
	bool DelTask(eDVBType dvbtype,std::string taskid);						//删除指定指定监测的指定任务（用于删除安播的自动录像）
	//
	bool DelRecTask(eDVBType dvbtype,std::string taskid);						//
	bool DelRecInfo(eDVBType dvbtype,std::string taskid);						//
	//
	bool QueryTaskInfo(int deviceid,std::vector<sTaskInfo>& taskinfovec);		//获取指定通道内全部任务信息
	bool QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo);					//获取指定通道内运行任务信息
	bool QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo);					//
	bool QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec);//获取指定类型的全部任务信息

	bool GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid="",std::string videopid="",std::string audiopid="");//通过类型和频点获取通道信息
	bool GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid="",std::string videopid="",std::string audiopid="");		//通过类型和频点获取通道信息
	bool GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid); //通过监测类型，channelid获取通道信息

	bool UpdateRunPriority(std::vector<sTaskInfo> taskinfovec);				//更新任务执行优先级
	bool SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml);			//设置手动录像

	bool ShareDevice(XMLTask* task);	//判断新任务能否共享硬件 
	int  UnGetDevIdSize();//由于分配通道在任务执行前,多个未分配通道的任务同时下发,存在多任务分配同一通道情况
	//
	bool IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid);//录像录制指标使用,添加指标任务时判断指标任务是否已经存在,由于录像存在一个频点多个节目
	bool IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq);//录像录制指标使用,删除录像任务时,是否删除指标任务,看此频点的录像任务是否存在,存在就不删除,不存在就删除
private:
	bool bFlag;
	time_t t_TaskInfoMapUpdateTime;
	sTaskInfo TaskInfoList[500];
	bool UpDataTaskInfoMapShadow();

	ACE_Thread_Mutex MonitorMapMutex;

	std::list<int>							DeviceList;	//通道队列；
	//
	ACE_Thread_Mutex DelTaskInfoMutex;
	std::list<DelTaskInfo>					DelTaskInfoList;	//删除任务信息；
	//
	ACE_Thread_Mutex AddRecMutex;
	std::list<XMLTask*>						RecTaskInfoList;	//删除任务信息；
	//
	std::map<int,std::list<XMLTask*> >		TaskMonitorMap;	//调度队列；map<通道号,list<任务指针> >
	std::map<int,TaskExecuteMgr*>			TaskExecuteMgrMap;	//执行队列；map<通道号,list<执行对象指针> >
	
};
