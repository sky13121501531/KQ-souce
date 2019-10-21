
#include "EnvironmentChecher.h"
#include "OS_Environment.h"
//#include "../Alarm/TR290AlarmMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Alarm/AlarmMgr.h"

EnvironmentChecher::EnvironmentChecher()
{
	bFlag = false;
}

EnvironmentChecher::~EnvironmentChecher()
{

}

int EnvironmentChecher::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 环境监测线程开始执行 !\n"));
	this->activate();
	return 0;
}

int EnvironmentChecher::svc()
{
	bFlag = true;
	OS_Environment::CPURealtimeUsage();//抛弃第一次CPU使用率值
	while (bFlag)
	{
	   OSFunction::Sleep(1,0);
	   //检查CPU
	   sCheckParam checkparam;
	   checkparam.AlarmType=ALARM_TR101_290;
	   checkparam.DVBType=CTTB;
	   checkparam.TypeDesc="TS同步丢失";
	   checkparam.TypeID="2";
	   checkparam.Freq="666";
	   checkparam.STD="DMB-T";
	   checkparam.SymbolRate="6875";
	   checkparam.TypedValue=StrUtil::Long2Str(OS_Environment::CPURealtimeUsage());
	   std::string xml;
	  // TR290ALARMMGR::instance()->CheckAlarm(checkparam,xml);
	  
	   //检查内存
	   /*checkparam.AlarmType=ALARM_ENVIRONMENT;
	   checkparam.DVBType=CTTB;
	   checkparam.TypeDesc="Memory使用率";
	   checkparam.TypeID="51";
	   checkparam.TypedValue=StrUtil::Long2Str( OS_Environment::MemoryRealtimeUsage());

	   ALARMMGR::instance()->CheckAlarm(checkparam);
	  */
	}

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 环境监测线程停止执行 !\n"));

	return 0;
}

int EnvironmentChecher::Start()
{
	open(0);
	return 0;
};
int EnvironmentChecher::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}