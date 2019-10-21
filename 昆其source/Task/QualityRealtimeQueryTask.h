///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：QualityRealtimeQueryTask.cpp
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：实时指标任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class QualityRealtimeQueryTask : public DeviceRelatedTask
{
public:
	QualityRealtimeQueryTask();
	QualityRealtimeQueryTask(std::string strXML);
	virtual ~QualityRealtimeQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//实时任务

private:
	int CreateTcpDataForRadio(std::string strRtnXML,unsigned char* TcpData, int size_n);
	int CreateTcpDataForATV(std::string strRtnXML,unsigned char* TcpData, int size_n);
	int CreateTcpDataForCTTB(std::string strRtnXML,unsigned char* TcpData,int size_n);

	bool ReadyForAlarm(std::string strXML,std::vector<sCheckParam>& alarmVec);
	string Periodicity;

	//用其它设备(场强卡,频谱仪) 获取场强信息
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel);

private:
	string m_lastLevel;
	int m_tunDeviceID;
};