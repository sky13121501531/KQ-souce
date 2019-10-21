


#ifndef INFO_CHECK_REQUEST_TASK_H
#define INFO_CHECK_REQUEST_TASK_H


#include "DeviceIndependentTask.h"
#include <string>

using namespace std;



//配置信息查询任务
class InfoCheckRequestTask : public DeviceIndependentTask
{
public:
	InfoCheckRequestTask();
	InfoCheckRequestTask(std::string strXML);
	virtual ~InfoCheckRequestTask();

public:
	virtual void Run(void);

public:
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	void ChannelCheck();
	void RecordCheck();
	void AlarmParamCheck();
	void TransAlarmVecToMap(const AlarmParamVec& vecAlarmParam, CodeAlarmParamMap& mapAlarmParam);

private:
	typedef enum InfoCheckType
	{
		UNKNOWN_CHECK,
		CHANNEL_CHECK,
		RECORD_CHECK,
		ALARMPARAM_CHECK
	} eInfoCheckType;

	eInfoCheckType m_eInfoCheckType;
};


































#endif