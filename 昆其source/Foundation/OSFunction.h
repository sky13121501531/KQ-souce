#pragma once
#include <string>
#include "TypeDef.h"
#include "../DeviceAccess/CardType.h"
class OSFunction
{
public:
	static void Sleep(int sencond,int millisecond=0);
	static void CompressFile(std::string dstZipPath,std::string srcFilePath);  //压缩文件并入库
	static void GetFileName(eDVBType dvbType,std::string taskType,std::string& strFileName);

	static string GetXmlMsgID(string datetime);

	static eDVBType GetEnumDVBType(string StrDVBType);
	static string   GetStrDVBType(eDVBType eDVBType);
	static string   GetAlarmTypeID(eDVBType type,string alarmtype);
	static bool SetOSDTimeMode(int mode,OSDInfo& osd);

	//创建板卡任务指令xml
	static string CreateTaskTypeXml(int deviceid);
	static string CreateCardRebootXml(int deviceid);
	//创建板卡表版本xml
	static string CreateTableVersionXml(enumDVBType dvbtype);

	static void ExitProcess(string info);
};