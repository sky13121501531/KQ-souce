///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：XMLTask.h
// 创建者：jiangcheng
// 创建时间：2009-05-25
// 内容描述：任务基类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../Foundation/TypeDef.h"
#include "../Foundation/PropManager.h"
#include <string>
#include <list>

/****************************状态修改时间************************/
/* WAITING,				//生成任务时置状态为WAITING 
/* READY,				//在运行期间内置状态为READY（单次任务入队后；定时任务在运行期间）	--UpdataStatus()
/* RUNNING,				//运行中置状态为RUNNING												--Run()
/* FINISHED,			//单次运行结束后为FINISHED											--Run()
/* EXPIRED				//任务过期（单次任务结束后为过期；定时任务最后一次执行完毕后为过期）--UpdataStatus()
/****************************************************************/

class Scheduler;
class XMLTask
{
public:
	XMLTask();
	XMLTask(std::string strXML);
	virtual ~XMLTask();
public:
	virtual void Run(void);				//任务执行函数
	virtual bool Stop(void) = 0;		//任务停止函数
	virtual bool UpdataStatus();		//任务状态更新函数,更新任务对象状态. 负责更新任务对象的状态。
										//调度线程要保证循环调用每个任务的该接口以保证对象状态的及时更新和正确性。
public:		
	//调度相关函数
	bool IsReady(){ return (TaskStatus == READY); };
	bool IsRunning(){ return (TaskStatus == RUNNING); };
	bool IsFinised(){ return (TaskStatus == FINISHED); };
	bool IsExpired(){ return (TaskStatus == EXPIRED); };
	bool IsSetTaskOK(){ return bSetOK; };

	bool SetExpired();
	bool SetTaskOK();

protected:
	bool SetWaiting();
	bool SetReady();
	bool SetRunning();
	bool SetFinised();
	
public:
	//获得XML表头信息
	std::string GetVersion() const {return strVersion;};
	std::string GetMsgID() const {return strMsgID;};
	std::string GetType() const {return strType;};
	std::string GetProtocol() const {return strProtocol;};
	std::string GetSetDateTime() const {return strSetDateTime;};//获得任务设置日期时间
	std::string GetSrcCode() const {return strSrcCode;};
	std::string GetDstCode() const {return strDstCode;};
	std::string GetPriority() const {return strPriority;};
	std::string GetSrcURL() const {return strSrcURL;};
	
	//设置头信息
	void SetVersion(std::string strValue) {strVersion = strValue;};
	void SetMsgID(std::string strValue) {strMsgID = strValue;};
	void SetType(std::string strValue) {strType = strValue;};
	void SetProtocol(std::string strValue) {strProtocol = strValue;};
	void SetSetDateTime(std::string strValue) {strSetDateTime = strValue;};
	void SetSrcCode(std::string strValue) {strSrcCode = strValue;};
	void SetDstCode(std::string strValue) {strDstCode = strValue;};
	void SetPriority(std::string strValue) {strPriority = strValue;};

	bool SendXML(const std::string& strXML);

	virtual std::string GetBaseObjectName() = 0;
	virtual std::string GetObjectName() = 0;
	virtual std::string GetTaskName() = 0;
public:
	eTaskRetStatus GetRetValue()const {return RetValue;};
	void SetRetValue(eTaskRetStatus retvalue){RetValue = retvalue;};

	eDVBType GetDVBType() const {return DVBType;};
	void SetDVBType(eDVBType type){DVBType = type;};

	std::string GetTaskID() const  {return TaskID;};
	void SetTaskID(std::string taskid){TaskID = taskid;};

	int GetDeviceID() const  {return DeviceID;};
	void SetDeciveID(int deviceid){DeviceID = deviceid;};
	void SetMultiWindowNumber(int WindowNumber){m_WindowNumber = WindowNumber;};

	std::string GetRunPriority() const  {return strRunPriority;};
	void SetRunPriority(std::string runPriority){strRunPriority = runPriority;};

	virtual bool AddTaskXml(){return true;};
	virtual bool DelTaskXml(){return true;};

	virtual bool IsRealTimeTask(){return false;};	//实时任务
	virtual bool IsVedioTask(){return false;};		//音视频任务
	virtual std::string CreateSchedulerAlarm() {return "";};
	std::string GetTaskFreq()const {return Freq;};
	std::string GetTaskServiceID()const {return strServiceID;};
	std::string GetTaskVideoPID()const {return strVideoPID;};
	std::string GetTaskAudioPID()const {return strAudioPID;};
	std::string GetChannelID()const {return ChannelID;};
	int GetMultiWindowNumber()const {return m_WindowNumber;};
	bool IsMulti(){return m_bmulti;};
	std::string GetDeviceXml()const {return strDeviceXML;};
	std::string GetStandardXml()const {return strStandardXML;};

	time_t GetStartDateTime();
	time_t GetEndDateTime();
	long GetCycleInterval();

	bool IsShareTask(){return PROPMANAGER::instance()->IsShareTask(GetObjectName());};

#ifdef ZONG_JU_ZHAO_BIAO
	virtual bool IsHDProgram(){return mIsHD;};
	virtual bool IsOSDProgram(){return mIsOSD;};
#endif
protected:
	Scheduler* TaskScheduler;	//定时属性对象
	bool bRun;					//执行线程运行与否标识

	std::string TaskID;
	int DeviceID;
	eDVBType DVBType;

	std::string Freq;
	std::string ChannelID;
	
	bool mIsHD;
	bool mIsOSD;

protected:
	std::string strStandardXML;		//任务标准XML
	std::string strDeviceXML;		//任务硬件层XML

	//XML表头信息
	std::string strVersion;		//版本编号
	std::string strMsgID;		//消息编号
	std::string strType;		//消息类型
	std::string strProtocol;	//传输协议类型
	std::string strSetDateTime;	//任务设置日期时间
	std::string strSrcCode;		//消息源编码
	std::string strDstCode;		//消息目标编码
	std::string strPriority;	//命令的优先级
	std::string strSrcURL;      //消息回复URL
	std::string strServiceID;
	std::string strVideoPID;
	std::string strAudioPID;
	BOOL m_bmulti;
	int m_WindowNumber;//多画面窗口数量

protected:
	std::string strRunPriority;	//任务调度优先级

protected:
	eTaskRetStatus RetValue;	//任务执行结果      
private:
	enumTaskStatus TaskStatus;	//任务状态参数
	bool bSetOK;

};