#pragma once

#include "DeviceIndependentTask.h"

class RecordCapabilityQueryTask : public DeviceIndependentTask
{
public:
	RecordCapabilityQueryTask();
	RecordCapabilityQueryTask(std::string strXML);
	virtual ~RecordCapabilityQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

#ifdef ZONG_JU_ZHAO_BIAO

private:
	std::vector<RecChanInfo> m_vecChanInfo;
	std::string m_strRecordType;

#endif
};