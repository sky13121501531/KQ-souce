#include "OSFunction.h"
#include "ace/os.h"
#include "PropManager.h"
#include <list>
#include "../DBAccess/DBManager.h"  //wz_101229
#include "StrUtil.h"
#include "TimeUtil.h"

using namespace std;
void OSFunction::Sleep(int sencond, int millisecond)
{
	ACE_Time_Value tv(sencond,millisecond*1000);
	ACE_OS::sleep(tv);
}


string OSFunction::GetXmlMsgID(string datetime)
{
	try
	{
		/*{
		std::cout<<"传进来的time："<<datetime<<std::endl;
		//截时间的方法
		//0123456789012345678
		//2011-12-15 15:30:00
		string timeNumber = "";
		timeNumber += datetime.substr(5,2);	//月份
		timeNumber += datetime.substr(8,2);	//天
		timeNumber += datetime.substr(14,2);	//小时
		timeNumber += datetime.substr(17,2);	//秒
		std::cout<<"拼接后的时间："<<timeNumber<<std::endl;
		int nTimeNumber = StrUtil::Str2Int( timeNumber );
		nTimeNumber *= 3;
		string strMsgId = StrUtil::Int2Str(nTimeNumber);
		std::cout<<"生成的MsgId："<<strMsgId<<std::endl;
		return strMsgId;
		}*/
		time_t tm = TimeUtil::StrToDateTime(datetime);
		string strTm = StrUtil::Long2Str(tm);

		string strTmCuted = strTm.substr(5,5);

		int nMsgId = StrUtil::Str2Int( strTmCuted );
		nMsgId += 1024;
		string retMsgId = StrUtil::Int2Str( nMsgId );
		return retMsgId;
	}
	catch (...)
	{
		std::cout<<""<<std::endl;
		return "0";
	}
}

void OSFunction::CompressFile( std::string dstZipPath,std::string srcFilePath )
{
	//wz_101229
	string createtime = TimeUtil::GetCurDateTime();						//压缩文件创建时间
	string DBfilename = dstZipPath;										//压缩文件名
	string expiredays = PROPMANAGER::instance()->GetTempFileExpire();	//过期时间从配置文件获取  jidushan 110406
	bool DBret = DBMANAGER::instance()->AddTempFile(DBfilename, createtime, expiredays);
	if (!DBret)
	{
		string msg = string("压缩文件信息入库失败");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		//return false;
	}
	string msg = string("压缩文件信息入库成功");
	//SYSMSGSENDER::instance()->SendMsg(msg);

	std::string command=std::string("winrar.exe a -ibck -ep -df ")+dstZipPath+std::string(" ")+srcFilePath;
	system(command.c_str());
}
void OSFunction::GetFileName(eDVBType dvbType,std::string taskType,std::string& strFileName)
{
	std::string xmlPath;
	PROPMANAGER::instance()->GetXmlTablePath(dvbType,xmlPath);
	if(xmlPath.size()==0)
	{
		if(dvbType == RADIO)
		{
			xmlPath = "C:/table/fm/";
		}
		else if(dvbType == AM)
		{
			xmlPath = "C:/table/am/";
		}
	}
	if (stricmp(taskType.c_str(),"ChannelScanQueryTask") == 0)  //频道扫描
	{
		strFileName = xmlPath + string("ChannelScanQuery.xml");
		return;
	}
	else if (stricmp(taskType.c_str(),"EPGQuery") == 0)     //EPG查询
	{
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		std::string searchPath = xmlPath + string("*.*");
		hFind = FindFirstFile(searchPath.c_str(), &FindFileData);
        string tempTime;
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				string FileNameTemp = string(FindFileData.cFileName);
				string subString = FileNameTemp.substr(0,8); 
				if (strcmp(subString.c_str(),"EPGQuery") == 0)
				{
                  if (strcmp(tempTime.c_str(),FileNameTemp.substr(8,14).c_str())<0)
                  {
					  tempTime = FileNameTemp.substr(8,14);    //获取最新EPG文件
                  }
				}
			}
			if (!FindNextFile(hFind,&FindFileData))
			{
				break;
			}
		}
		FindClose(hFind);
		strFileName = /*xmlPath + */string("EPGQuery") + tempTime +string(".xml");
		return;

	}
	else if (stricmp(taskType.c_str(),"TableQueryTask") == 0)   //表查询
	{

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		std::string searchPath = xmlPath + string("*.*");
		hFind = FindFirstFile(searchPath.c_str(), &FindFileData);
		string tempTime;
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				string FileNameTemp = string(FindFileData.cFileName);
				string subString = FileNameTemp.substr(0,10); 
				if (strcmp(subString.c_str(),"TableQuery") == 0)
				{
					if (strcmp(tempTime.c_str(),FileNameTemp.substr(10,14).c_str())<0)
					{
						tempTime = FileNameTemp.substr(10,14);    //获取最新TableQuery文件
					}
				}
			}
			if (!FindNextFile(hFind,&FindFileData))
			{
				break;
			}
		}
		FindClose(hFind);
		strFileName = string("TableQuery") + tempTime +string(".zip");
		return;
	}
}
string OSFunction::CreateTaskTypeXml(int deviceid)
{
	string DeviceIp = PROPMANAGER::instance()->GetDeviceIp();
	string DevicePort = PROPMANAGER::instance()->GetDevicePort();

	string LogIp = PROPMANAGER::instance()->GetLogIP();
	string LogPort = PROPMANAGER::instance()->GetLogPort();

	eDVBType dvbtype = UNKNOWN;
	PROPMANAGER::instance()->GetDeviceType(deviceid,dvbtype);
	string tasktype;
	PROPMANAGER::instance()->IsDeviceAvaiable(deviceid,"TSAnalyze",dvbtype) ? tasktype = "TSAnalyze" : tasktype = "TS";

	char* xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>				\
			   <Msg DVBType=\"\">															\
			   <DeviceTaskType TaskType=\"\" />												\
			   <HostAddr IP=\"\" Port=\"\" />												\
			   <LogAddr IP=\"\" Port=\"\" />												\
			   </Msg>";

	XmlParser Parser( xml );
	pXMLNODE RootNode=Parser.GetRootNode();//根节点msg

	//3D需要保证发送给httpserver的码流分析指令xml中的dvbtype为dvbc
	eDVBType tempeDvbtype = dvbtype;
	string strdvbtype = GetStrDVBType(tempeDvbtype);
	Parser.SetAttrNode("DVBType",strdvbtype,RootNode);

	pXMLNODE TypeNode = Parser.GetNodeFromPath( "Msg/DeviceTaskType" );
	Parser.SetAttrNode("TaskType",tasktype,TypeNode);

	pXMLNODE DeviceNode = Parser.GetNodeFromPath( "Msg/HostAddr" );
	Parser.SetAttrNode("IP",DeviceIp,DeviceNode);
	Parser.SetAttrNode("Port",DevicePort,DeviceNode);

	pXMLNODE LogNode = Parser.GetNodeFromPath( "Msg/LogAddr" );
	Parser.SetAttrNode("IP",LogIp,LogNode);
	Parser.SetAttrNode("Port",LogPort,LogNode);
	if (tasktype=="TSAnalyze")
	{
		pXMLNODE ChanneScanNode=Parser.CreateNodePtr(RootNode,"ChannelScanSet");
		
		sFreqScanInfo freqscaninfo;

		PROPMANAGER::instance()->GetFreqScanInfo(dvbtype,freqscaninfo);

		pXMLNODE child=Parser.CreateNodePtr(ChanneScanNode,"ScanInfo");
		Parser.SetAttrNode("CentreFreq",freqscaninfo.CenterFreq,child);
		Parser.SetAttrNode("SymbolRate",freqscaninfo.SymbolRate,child);
		Parser.SetAttrNode("QAM",freqscaninfo.QAM,child);
	}
	else
	{
		return "";
	}

	string retXml;
	Parser.SaveToString(retXml);
	return retXml;
}

string OSFunction::CreateCardRebootXml(int deviceid)
{
	eDVBType dvbtype = UNKNOWN;
	PROPMANAGER::instance()->GetDeviceType(deviceid,dvbtype);

	string strdvbtype = GetStrDVBType(dvbtype);

	string tasktype = "Reboot";
	
	char* xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>				\
			   <Msg DVBType=\"\">															\
			   <DeviceTaskType TaskType=\"\" />												\
			   <HostAddr IP=\"\" Port=\"\" />												\
			   <LogAddr IP=\"\" Port=\"\" />												\
			   </Msg>";

	XmlParser Parser( xml );
	pXMLNODE RootNode=Parser.GetRootNode();//根节点msg

	Parser.SetAttrNode("DVBType",strdvbtype,RootNode);

	pXMLNODE TypeNode = Parser.GetNodeFromPath( "Msg/DeviceTaskType" );
	Parser.SetAttrNode("TaskType",tasktype,TypeNode);

	string retXml;
	Parser.SaveToString(retXml);
	return retXml;
}

string OSFunction::CreateTableVersionXml(enumDVBType dvbtype)
{
	return "";
}

eDVBType OSFunction::GetEnumDVBType(string StrDVBType)
{
	eDVBType eDVBType = UNKNOWN;
	if (StrDVBType == "CTTB" || StrDVBType =="1")
	{
		eDVBType = CTTB;
	}
	else if (StrDVBType == "CMMB" || StrDVBType =="2")
	{
		eDVBType = CMMB;
	}
	else if (StrDVBType == "DVBC"|| StrDVBType =="3")
	{
		eDVBType = DVBC;
	}
	else if (StrDVBType == "RADIO"|| StrDVBType =="4")
	{
		eDVBType = RADIO;
	}
	else if (StrDVBType == "ATV"|| StrDVBType =="5")
	{
		eDVBType = ATV;
	}
	else if (StrDVBType == "AM"|| StrDVBType =="6")
	{
		eDVBType = AM;
	}
	else if(StrDVBType == "DVBS"|| StrDVBType =="7")
	{
		eDVBType = DVBS;
	}
	else if (StrDVBType == "CTV"|| StrDVBType =="8")
	{
		eDVBType = CTV;
	}
	else
	{
		eDVBType = UNKNOWN;
	}
	return eDVBType;
}
string OSFunction::GetStrDVBType(eDVBType eDVBType)
{
	string StrDVBType;
	switch (eDVBType)
	{
	case CTTB:
		StrDVBType = "CTTB";
		break;
	case CMMB:
		StrDVBType = "CMMB";
		break;
	case DVBC:
		StrDVBType = "DVBC";
		break;
	case RADIO:
		StrDVBType = "RADIO";
		break;
	case ATV:
		StrDVBType = "ATV";
		break;
	case AM:
		StrDVBType = "AM";
		break;
	case DVBS:
		StrDVBType = "DVBS";
		break;
	case CTV:
		StrDVBType = "CTV";
		break;
	default:
		StrDVBType = "UNKNOWN";
	}
	return StrDVBType;
}
bool OSFunction::SetOSDTimeMode(int mode,OSDInfo& osd)
{
	switch (mode)
	{
	case 1:
		osd.TimeType =  StrUtil::Int2Str(OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH | OSD_CWEEK | OSD_HOUR24);	//2000年1月1日 星期一 15:00:00
		break;
	case 2:
		osd.TimeType =  StrUtil::Int2Str(OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH | OSD_HOUR24);				//2000年1月1日 15:00:00
		break;
	case 3:
		osd.TimeType =  StrUtil::Int2Str(OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH | OSD_CWEEK);				//2000年1月1日 星期一
		break;
	case 4:
		osd.TimeType =  StrUtil::Int2Str(OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH);							//2000年1月1日
		break;
	case 5:
		osd.TimeType =  StrUtil::Int2Str(OSD_YEAR4 |  OSD_MONTH | OSD_CWEEK | OSD_HOUR24);				//2000-1-1 星期一 15:00:00
		break;
	case 6:
		osd.TimeType =  StrUtil::Int2Str(OSD_YEAR4 |  OSD_MONTH | OSD_HOUR24);							//2000-1-1 15:00:00
		break;
	case 7:
		osd.TimeType =  StrUtil::Int2Str(OSD_YEAR4 |  OSD_MONTH | OSD_CWEEK);							//2000-1-1 星期一
		break;
	case 8:
		osd.TimeType =  StrUtil::Int2Str(OSD_YEAR4 |  OSD_MONTH);										//2000-1-1
		break;
	//case 9:
	//	osd.TimeType =  StrUtil::Int2Str(OSD_HOUR24);													//15:00:00
	//	break;
	default:
		osd.TimeType =  StrUtil::Int2Str(OSD_YEAR4 |  OSD_MONTH | OSD_HOUR24);							//2000-1-1 15:00:00
		break;
	}
	return true;
}

void OSFunction::ExitProcess(string info)
{
	
	cout<<"\n\n";
	for (int i=5;i>0;--i)
	{
		cout<<"\t"<<info<<" 该进程将在"<<i<<"秒后退出\r";
		OSFunction::Sleep(1);
	}
	string killcmd="taskkill -im vscttb.exe ";
	system(killcmd.c_str());
	// exit(-1);
}

string   OSFunction::GetAlarmTypeID(eDVBType type,string alarmtype)
{
	string TypeID;
	if(type==DVBC||type==DVBS||type==CTTB)
	{
		if(alarmtype=="NoSignal")
		{
			if(type==CTTB)
				TypeID = "0";
			else
				TypeID = "1";
		}
		else if(alarmtype=="ImageStill")
		{
			TypeID = "31";
		}
		else if(alarmtype=="BlackBurst")
		{
			TypeID = "32";
		}
		else if(alarmtype=="NoAudio")
		{
			TypeID = "33";
		}
	}
	else if(type==ATV||type ==CTV)
	{
		if(alarmtype=="NoSignal")
		{
				TypeID = "10";
		}
		else if(alarmtype=="ImageStill")
		{
			TypeID = "13";
		}
		else if(alarmtype=="BlackBurst")
		{
			TypeID = "18";
		}
		else if(alarmtype=="NoAudio")
		{
			TypeID = "12";
		}
		else if(alarmtype == "NoVideo")
		{
			TypeID = "11";
		}
		else if(alarmtype == "ColorBar")
		{
			TypeID = "14";
		}
	}
	else if(type==RADIO||type==AM)
	{
		if(alarmtype == "NoSignal")
		{
			TypeID = "23";
		}
		else if(alarmtype =="NoAudio")
		{
			TypeID = "24";
		}
	}
	return TypeID;
}