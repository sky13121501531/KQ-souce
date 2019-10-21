#ifndef ChanEncryptQueryTask_h__
#define ChanEncryptQueryTask_h__





#include "DeviceIndependentTask.h"
#include "../Foundation/TypeDef.h"



#ifdef ZONG_JU_ZHAO_BIAO

//小卡卡号查询任务
class ChanEncryptQueryTask : public DeviceIndependentTask
{
public:
	ChanEncryptQueryTask();
	ChanEncryptQueryTask(std::string strXML);
	virtual ~ChanEncryptQueryTask();

public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();


private:
	bool TranslateEncryptXml(std::vector<std::string>& vecTaskXml);

private:
	std::vector<CACardInfo> m_vecCardInfo;
};


#endif















#endif // ChanEncryptQueryTask_h__