

#ifndef GetRecVideoTimeTask_h__
#define GetRecVideoTimeTask_h__






#include "DeviceIndependentTask.h"
#include "../Foundation/Config.h"

#ifdef ZONG_JU_ZHAO_BIAO

//Â¼ÏñÑ¯Ê±ÈÎÎñ
class GetRecVideoTimeTask : public DeviceIndependentTask
{
public:
	GetRecVideoTimeTask();
	GetRecVideoTimeTask(std::string strXML);
	virtual ~GetRecVideoTimeTask();

public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};


#endif















#endif // GetRecVideoTimeTask_h__
