


#ifndef RUN_PLAN_CHECK_TASK_H
#define RUN_PLAN_CHECK_TASK_H


#include "DeviceIndependentTask.h"
#include "../Foundation/TypeDef.h"
#include <string>

using namespace std;


class RunPlanCheckTask : public DeviceIndependentTask
{
public:
	RunPlanCheckTask()	{ };
	RunPlanCheckTask(std::string strXML);
	virtual ~RunPlanCheckTask()	{ };
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	void TransVecToMap(const RunPlanParamVec& vecRunplanInfo, RunPlanParamMap& mapCodeToParamVec);

private:
	string m_strEquCode;		//…Ë±∏∫≈
};


































#endif