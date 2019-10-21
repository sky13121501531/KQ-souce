///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXML.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"

class TranslateDownXML
{
public:
	//返回标准的xml
	static bool TranslateTaskXML(const std::string& strOriginalXML,std::string& strStandardXML);

	//返回标准的录像执行任务XML
	static bool TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的偱切录像执行任务XML
	static bool TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的定时指标查询任务xml
	static bool TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//返回标准的定时频谱任务xml
	static bool TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
};