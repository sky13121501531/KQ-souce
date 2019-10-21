///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：QualityTask.h
// 创建者：gaoxd
// 创建时间：2009-06-25
// 内容描述：指标任务类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class QualityTask : public DeviceRelatedTask
{
public:
	QualityTask();
	QualityTask(std::string strXML);
	virtual ~QualityTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual void SetDeciveID(int deviceid);
	virtual bool AddTaskXml();
	virtual bool DelTaskXml();
protected:
    bool  GetQualityInfo(std::string orgXML,std::string rtnXML, std::string &Freq,std::string &taskID,std::vector<eQualityInfo> & vecQualityInfo);
	bool ReadyForAlarm(eQualityInfo eQinfo,sCheckParam& alarmVec);
	bool GetQualityInfoXmlFromDevice(int iDevID, std::string strFreq, std::string& strRetXml);
	bool GetBerInfo2ScientificNotation(string sInBer, string sFreq, string sLevel, string& sOutBer);//将 Ber 信息 转换成 科学计数法
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel);//用其它设备(场强卡,频谱仪) 获取场强信息
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, vector<std::string> vecFreqs, map<std::string, std::string>& mapFreqLevels);//用其它设备(场强卡,频谱仪) 获取场强信息


private:
	std::vector<std::string> mVecStdStr;
	bool m_alarmlimit;//用于录像指标录制的报警门限记录标记
};