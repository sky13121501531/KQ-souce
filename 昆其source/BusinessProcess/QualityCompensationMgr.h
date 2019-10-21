#pragma once
#include "../Foundation/TypeDef.h"
#include <vector>
#include "ace/Synch.h"
#include "ace/Singleton.h"
class QualityCompensationMgr
{
public:
	QualityCompensationMgr();
	virtual ~QualityCompensationMgr();
public:
	bool UpdateQualityCompensation(std::vector<sQualityCompensation> qualityCmpVec);
	bool GetQualityCompensation(eDVBType dvbtype,std::string deviceid,std::string type,std::string freq,std::string level,std::string& valu);
private:
	void init(void);
private:
	ACE_Thread_Mutex QualityComMutex;
	std::vector<sQualityCompensation> mQulaityCmpVec;
};
typedef ACE_Singleton<QualityCompensationMgr,ACE_Mutex>  QUALITYCOMPENSATIONMGR;