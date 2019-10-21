#include "./DeviceIndependentTask.h"
#include <string>
class OSDSetTask:public DeviceIndependentTask
{
public:
	OSDSetTask(void);
	OSDSetTask(std::string strXML);
	~OSDSetTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};
