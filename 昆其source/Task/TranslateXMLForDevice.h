#pragma once
#include <string>
#include <vector>
class TranslateXMLForDevice
{
public:
	TranslateXMLForDevice(void);
	~TranslateXMLForDevice(void);
public:
	//实时视频任务标准XML转化为与硬件通讯的XML
	static std::string TranslateStreamRealTimeXML(std::string strStdXML);
	//指标查询任务标准XML转化为与硬件通讯的XML
	static std::string TranslateQualityRealTimeQuery(std::string strStdXML);
	//录像任务标准XML转化为与硬件通讯的XML
	static std::string TranslateRecordTask(std::string strStdXML);
	//轮播任务标准XML转化为与硬件通讯的XML
	static std::vector<std::string> TranslateRoundStreamXML(std::string strStdXML);
	static std::vector<std::string> TranslateRoundRecordXML(std::string strStdXML);
	//任务指标标准XML转化为与硬件通讯的XML
	static std::vector<std::string> TranslateQualityTask(std::string strStdXML);
	static std::string TranslateSpectrumScan(std::string strStdXML);
	static std::string TranslateSpectrumTask(std::string strStdXML);

	//偱切录像任务标准XML转化为与硬件通讯的XML
	static std::vector<std::string> TranslateRoundRoundXML(std::string strStdXML);
private:
	static std::string GetXMLHeader(std::string downType);
};
