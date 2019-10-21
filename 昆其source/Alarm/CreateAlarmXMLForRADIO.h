///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：CreateAlarmXMLForRADIO.h
// 创建者：gaoxd
// 创建时间：2011-01-27
// 内容描述：生成FM报警信息
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "../Foundation/TypeDef.h"

class CreateAlarmXMLForRADIO
{
public:
	static std::string CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	static std::string CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	static std::string CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue);
};