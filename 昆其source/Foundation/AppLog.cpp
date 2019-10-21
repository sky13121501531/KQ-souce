
#include <iostream>
#include <time.h>
#include "AppLog.h"
#include "TimeUtil.h"
#include "FileOperater.h"
#include "TimeUtil.h"
#include "PropManager.h"
using namespace std;

AppLog::AppLog(void)
{
	mOldLogOutPut = mLogOutPut = PROPMANAGER::instance()->GetOutputFile();
	mLogType =PROPMANAGER::instance()->GetLogType();
}

AppLog::~AppLog(void)
{
}

void AppLog::GetLogBasicInfo(eModuleType Module,eLogType LogType,string& strLogName,string& strModule,string& strLogTpye)
{
	switch(Module)
	{
		case OTHER:
			strModule = PROPMANAGER::instance()->GetLogOther();
				break;
		case DATAANALYSER:
			strModule = PROPMANAGER::instance()->GetLogAnalyser();
			break;
		case RECORD:
			strModule = PROPMANAGER::instance()->GetLogRecord();
			break;
		case DEVICE:
			strModule = PROPMANAGER::instance()->GetLogDevice();
			break;
		case VIDEO:
			strModule = PROPMANAGER::instance()->GetLogVideo();
			break;
		case TASK:
			strModule = "task";
			break;
		case DATABASE:
			strModule = "database";
			break;
		default:
			strModule = "";
			break;
	}

	strLogName=PROPMANAGER::instance()->GetLogPath();
	strLogName += strModule;
	strLogName += "/";
	strLogName += TimeUtil::GetCurDate();
	strLogName += ".log";

	switch(LogType)
	{
	case LOG_EVENT_DEBUG:
		strLogTpye = "LOG_EVENT_DEBUG";
		break;
	case LOG_EVENT_WARNNING:
		strLogTpye = "LOG_EVENT_WARNNING";
		break;
	case LOG_EVENT_ERROR:
		strLogTpye = "LOG_EVENT_ERROR";
		break;
	default:
		strLogTpye = "LOG_EVENT_DEBUG";
		break;
	}
}
bool AppLog::WriteLogToFile(eModuleType Module,eLogType LogType,const std::string& EventLog)
{
	bool ret = false;
	FileOperater pFile;
	string strLogName,strModule,strLogType;

	GetLogBasicInfo(Module,LogType,strLogName,strModule,strLogType);

	if (pFile.OpenOrCreate(strLogName.c_str()))
	{
		string LogInfo ;
		
		LogInfo += "\xd\xa#-----------------------------";
		LogInfo += TimeUtil::GetCurDateTime(); 
		LogInfo += "-----------------------------#\xd\xa";
		
		LogInfo += (string("模块名称：") + strModule  + string("\xd\xa"));
		LogInfo += (string("日志级别：") + strLogType + string("\xd\xa"));
		LogInfo += (string("日志内容：") + EventLog   + string("\xd\xa"));

		pFile.WriteBuf((void*)LogInfo.c_str(),(int)LogInfo.length());

		pFile.Close();
		ret = true;
	}
	return ret;
}
bool AppLog::WriteLog(eModuleType Module,eLogType LogType,const std::string& EventLog,enumLogOutPut LogOutPut)
{
	bool ret = false;
	enumLogOutPut tempLogOutPut;

	LogOutPut != LOG_OUTPUT_UNDEFINE ? tempLogOutPut = LogOutPut : tempLogOutPut = mLogOutPut;

	if (tempLogOutPut == LOG_OUTPUT_BOTH)
	{
		std::cout << TimeUtil::DateTimeToStr(time(0)) << ": " << EventLog << std::endl;
		ret = WriteLogToFile(Module,LogType,EventLog);
	}
	else if (tempLogOutPut == LOG_OUTPUT_SCREEN)
	{
		std::cout << TimeUtil::DateTimeToStr(time(0)) << ": " << EventLog << std::endl;
		ret = true;
	}
	else if (tempLogOutPut == LOG_OUTPUT_FILE)
	{
		ret = WriteLogToFile(Module,LogType,EventLog);
	} 
	else
	{
		std::cout << TimeUtil::DateTimeToStr(time(0)) << ": " << EventLog << std::endl;
		ret = WriteLogToFile(Module,LogType,EventLog);
	}

	return ret;
}