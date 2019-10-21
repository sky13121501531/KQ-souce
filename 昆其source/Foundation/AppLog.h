#ifndef _APPLOG_H_
#define _APPLOG_H_

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：AppLog.h
// 创建者：高向东
// 创建时间：2009-05-20
// 内容描述：日志记录
// 备注：实现灵活的日志记录方式。
// 1、根据配置文件确定信息的输出方式（屏幕、文件）
// 2、程序一次性改变日志信息输出方式
// 3、程序动态改变日志信息输出方式
// 4、输出方式选择优先级别：动态改变 > 一次性改变 > 配置文件
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "TypeDef.h"
#include "ace/Singleton.h"

class AppLog
{
public:
	AppLog(void);
	virtual~AppLog(void);
private:
	eLogType mLogType;  //日志类型
	enumLogOutPut mLogOutPut; //日志输出类型
	enumLogOutPut mOldLogOutPut; //从配置文件初始化的类型
private:
	//根据模块名称获得log文件名称（包含路径）、对应的模块字符串名称以及日志消息级别名称
	void GetLogBasicInfo(eModuleType Module,eLogType LogType,std::string& strLogName,std::string& strModule,std::string& strLogTpye);
	bool WriteLogToFile(eModuleType Module,eLogType LogType,const std::string& EventLog);
	//对外接口
public:
	eLogType GetLogType(void){return mLogType;};						//得到日志的类型
	bool SetLogType(eLogType LogType){mLogType=LogType;return true;};   //设置日志的类型

public:
	void ManualSetOutType(enumLogOutPut LogOutPut){mLogOutPut = LogOutPut;};	//手动改变日志输出方式
	void CloseManualSet(){mLogOutPut = mOldLogOutPut;};							//关闭手动改变的输出方式，还原为默认

	bool WriteLog(eModuleType Module,eLogType LogType,const std::string& EventLog,enumLogOutPut LogOutPut = LOG_OUTPUT_UNDEFINE);
};
typedef ACE_Singleton <AppLog, ACE_Thread_Mutex>  APPLOG;
#endif
