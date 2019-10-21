///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：StreamRealtimeRoundQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-24
// 内容描述：实时视频轮播任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
#include <string>
#include <vector>
#include "ace/Task.h"
using namespace std;

class Device2ChanelID{
public:
	std::string position;
	std::string	program;
	std::string programid;
	std::string freq;
	std::string	serviceid;
	std::string	videoid;
	std::string	audioid;
	int deviceid;
	std::string chanelid;
	std::string bps;
	bool bhd;
};
class StreamRealtimeRoundQueryTask : public DeviceRelatedTask
{
public:
	StreamRealtimeRoundQueryTask();
	StreamRealtimeRoundQueryTask(std::string strXML);
	virtual ~StreamRealtimeRoundQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//实时任务
	virtual bool IsVedioTask(){return true;};		//音视频任务

	bool SetRelatedTask(StreamRealtimeRoundQueryTask* task){RelatedTask = task;return true;};
	bool SetStopSignal(){StopSignal = true;return true;};		//辅助任务调用
	std::string ReportCreateXML();    //创建主动上报xml
	bool ChangeDevCommand(string incmd,string turnid,string coderid,string& outcmd);

private:
	bool bOneDevice;
	bool bSendToIdleDev;					//bOneDevice:一个通道判断,bSendToIdleDev:是否向空闲通道发送过调频命令
	int idleDeviceID,busyDeviceID;			//idleDeviceID:空闲通道，busyDeviceID:执行任务通道，
	long RoundTime;							//轮播间隔
	int WindowNumber;                      //画面窗口数
	int RunTime;							//视频播放时长
	std::string Program;                    //节目名称
	std::vector<std::string> DecivcXMLVec;	//硬件命令组
	std::vector<Device2ChanelID> MultiIndex;	//硬件命令组
	std::vector<Device2ChanelID> MultiCurIndex;	//硬件命令组

	std::vector<int> DeviceMul1st;			 //1级接收卡通道组//FRee_fanrong
	std::vector<int> deviceindexmul;

	size_t ChannelNum;						//轮播的频道数
	size_t ChannelIndex;					//计数器
	int MultiNum; //多画面轮播的次数

	bool StopSignal;							//停止信号
	StreamRealtimeRoundQueryTask* RelatedTask;	//关联任务,用于处理关联任务的停止
	std::string m_startxml;
	std::string m_stopxml;
	std::string m_sOsd;
	//
	std::vector<int> m_tundevlist;
	std::vector<int> m_coddevlist;
	int m_roundnum;
};