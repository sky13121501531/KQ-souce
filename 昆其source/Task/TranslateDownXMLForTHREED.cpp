///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForTHREED.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为原子任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#include "TranslateDownXMLForTHREED.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"

//返回标准的频道扫描任务XML
bool TranslateDownXMLForTHREED::TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//频道扫描标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery></ChannelScanQuery></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
	}
	else
	{
		return false;
	}

	//设置头信息
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("ChannelScanQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	pXMLNODE childNode=parser.GetNodeFirstChild(rootNode);
	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/ChannelScanQuery");

	pXMLNODELIST nodelist=parser.GetNodeList(childNode);
	
	int count=nodelist->Size();
	string ScanTime;
	string STD, Freq, SymbolRate, StartFreq, EndFreq, StepFreq,QAM;

	parser.GetAttrNode( childNode,"ScanTime",ScanTime );
	if (count==0)
	{
		
		//获取频道扫描各属性
	
		parser.GetAttrNode( childNode,"SymbolRate",SymbolRate );
		parser.GetAttrNode( childNode,"QAM",QAM );
		//设置频道扫描属性
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
		tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
		tempParser.SetAttrNode( "STD",STD,chScanNode );
		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
		tempParser.SetAttrNode( "QAM",QAM,chScanNode );
		tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
		tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
		tempParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		tempParser.SaveToString(strStandardXML);//保存到字符串
	}
	else
	{
		for (int i=0;i!=count;++i)
		{
			pXMLNODE childnode=nodelist->GetNextNode();
			
			parser.GetAttrNode( childnode,"SymbolRate",SymbolRate );
			parser.GetAttrNode( childnode,"QAM",QAM );

			//设置频道扫描属性
			pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
			tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
			tempParser.SetAttrNode( "STD",STD,chScanNode );
			tempParser.SetAttrNode( "Freq",Freq,chScanNode );
			tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
			tempParser.SetAttrNode( "QAM",QAM,chScanNode );
			tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
			tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
			tempParser.SaveToString(strStandardXML);//保存到字符串
		}
	}
	return true;
}
//返回标准的实时视频任务XML
bool TranslateDownXMLForTHREED::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRealtimeQuery><RealtimeQueryTask/></StreamRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	//获取根元素
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
	}
	else
	{
		return false;
	}

	//设置头信息
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("StreamRealtimeQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );


	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );
	//获取视频查询元素
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( childNode );

	//初始化属性值

	string deviceID, freq, OrgNetID,tsID, serviceID, videoPID, audioPID;
	string Width, Height,Fps,Bps,ThreeDSet;
	string dataType;

	//获取子属性
	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	pXMLNODE realTimeNode = tempParser.GetNodeFirstChild( tempChildNode );


	//获取视频查询各属性
	parser.GetAttrNode( realStreamNode,"Freq",freq );
	parser.GetAttrNode( realStreamNode,"ServiceID",serviceID );
	parser.GetAttrNode( realStreamNode,"VideoPID",videoPID );
	parser.GetAttrNode( realStreamNode,"AudioPID",audioPID);

	parser.GetAttrNode(realStreamNode,"Width",Width);
	parser.GetAttrNode(realStreamNode,"Height",Height);
	parser.GetAttrNode(realStreamNode,"Fps",Fps);
	parser.GetAttrNode(realStreamNode,"Bps",Bps);
	parser.GetAttrNode(realStreamNode,"ThreeDSet",ThreeDSet);
	//获取OrgNetID,tsID
	CHANNELINFOMGR::instance()->GetOrgNetID(THREED,freq,audioPID,videoPID,serviceID,OrgNetID );
	CHANNELINFOMGR::instance()->GetTsID(THREED,freq,audioPID,videoPID,serviceID,tsID);

	tempParser.SetAttrNode( "DeviceID",deviceID,realTimeNode );
	tempParser.SetAttrNode( "Freq",freq,realTimeNode );
	tempParser.SetAttrNode( "OrgNetID",OrgNetID,realTimeNode );
	tempParser.SetAttrNode( "TsID",tsID,realTimeNode );
	tempParser.SetAttrNode( "ServiceID",serviceID,realTimeNode );
	tempParser.SetAttrNode( "VideoPID",videoPID,realTimeNode );
	tempParser.SetAttrNode( "AudioPID",audioPID,realTimeNode );
	tempParser.SetAttrNode( "DataType",dataType,realTimeNode );
	tempParser.SetAttrNode( "Width",Width,realTimeNode );
	tempParser.SetAttrNode( "Height",Height,realTimeNode );
	tempParser.SetAttrNode( "Fps",Fps,realTimeNode );
	tempParser.SetAttrNode( "Bps",Bps,realTimeNode);
	tempParser.SetAttrNode( "ThreeDSet",ThreeDSet,realTimeNode);

	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的视频轮播任务XML
bool TranslateDownXMLForTHREED::TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi)
{
	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRoundQuery></StreamRoundQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	//获取根元素
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
	}
	else
	{
		return false;
	}

	//设置头信息
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("StreamRoundQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	string deviceID, freq,code, RoundTime, OrgNetID,tsID, serviceID, videoPID, audioPID;
	string Width, Height,Fps,Bps,ThreeDSet;
	string dataType;

	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundInfoQuery/RoundStream");
	parser.GetAttrNode(childNode,"RoundTime",RoundTime);

	pXMLNODE tempNode=tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	tempParser.SetAttrNode("DeviceID",deviceID,tempNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempNode);

	pXMLNODELIST roundStreamList=parser.GetNodeList(childNode);
	for(int i=0;i<roundStreamList->Size();++i)
	{
		pXMLNODE tempChannelNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundChannel");
		pXMLNODE channelNode=parser.GetNextNode(roundStreamList);
		parser.GetAttrNode( channelNode,"Freq",freq );
		parser.GetAttrNode( channelNode,"ServiceID",serviceID );
		parser.GetAttrNode( channelNode,"VideoPID",videoPID );
		parser.GetAttrNode( channelNode,"AudioPID",audioPID);
		parser.GetAttrNode(childNode,"Width",Width);
		parser.GetAttrNode(childNode,"Height",Height);
		parser.GetAttrNode(childNode,"Fps",Fps);
		parser.GetAttrNode(childNode,"Bps",Bps);
		parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);
		//获取OrgNetID,tsID
		CHANNELINFOMGR::instance()->GetOrgNetID(THREED,freq,audioPID,videoPID,serviceID,OrgNetID );
		CHANNELINFOMGR::instance()->GetTsID(THREED,freq,audioPID,videoPID,serviceID,tsID);


		tempParser.SetAttrNode( "Freq",freq,tempChannelNode );
		tempParser.SetAttrNode("Code",code,tempChannelNode);
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,tempChannelNode );
		tempParser.SetAttrNode( "TsID",tsID,tempChannelNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,tempChannelNode );
		tempParser.SetAttrNode( "VideoPID",videoPID,tempChannelNode );
		tempParser.SetAttrNode( "AudioPID",audioPID,tempChannelNode );
		tempParser.SetAttrNode( "DataType",dataType,tempChannelNode );
		tempParser.SetAttrNode( "Width",Width,tempChannelNode );
		tempParser.SetAttrNode( "Height",Height,tempChannelNode );
		tempParser.SetAttrNode( "Fps",Fps,tempChannelNode );
		tempParser.SetAttrNode( "Bps",Bps,tempChannelNode );
		tempParser.SetAttrNode( "ThreeDSet",ThreeDSet,tempChannelNode);

	}
	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的录像设置任务XML
bool TranslateDownXMLForTHREED::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	//获取根元素
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"3DDown") )
	{
		downType = "THREED";
	}
	else 
	{
		return false;
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("RecordTaskSet"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	if(strOriginalXML.find("SetAutoRecordChannel")!=string::npos)
	{
		pXMLNODE autoRecordSetNode=tempParser.CreateNodePtr("Msg","AutoRecordSet");
		pXMLNODE autoRecordNode=parser.GetNodeFromPath("Msg/SetAutoRecordChannel");
		pXMLNODELIST autoRecordList=parser.GetNodeList(autoRecordNode);
		string action;
		for (int i=0;i<autoRecordList->Size();++i)
		{
			pXMLNODE node=parser.GetNextNode(autoRecordList);
			parser.GetAttrNode(node,"Action",action);
			pXMLNODELIST nodeList=parser.GetNodeList(node);
			for (int j=0;j<nodeList->Size();++j)
			{
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,Index,ThreeDSet;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"Index",Index);
				parser.GetAttrNode(childNode,"Freq",Freq);
				parser.GetAttrNode(childNode,"VideoPID",VideoPID);
				parser.GetAttrNode(childNode,"AudioPID",AudioPID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				parser.GetAttrNode(childNode,"Width",Width);
				parser.GetAttrNode(childNode,"Height",Height);
				parser.GetAttrNode(childNode,"Fps",Fps);
				parser.GetAttrNode(childNode,"Bps",Bps);
				parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);
				
				PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"THREED");

				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,TsID);
				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
				tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
				tempParser.SetAttrNode("Freq",Freq,recordNode);
				tempParser.SetAttrNode("Code",Code,recordNode);
				tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
				tempParser.SetAttrNode("TsID",TsID,recordNode);
				tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
				tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
				tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
				tempParser.SetAttrNode("DataType",DataType,recordNode);
				tempParser.SetAttrNode("Width",Width,recordNode);
				tempParser.SetAttrNode("Height",Height,recordNode);
				tempParser.SetAttrNode("Fps",Fps,recordNode);
				tempParser.SetAttrNode("Bps",Bps,recordNode);
				tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);


				tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
			}

		}
	}
	else if(strOriginalXML.find("ProvisionalRecordTaskSet")!=string::npos)
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		string Action;

		pXMLNODE proRecordTaskSetNode=parser.GetNodeFromPath("Msg/ProvisionalRecordTaskSet");
		pXMLNODELIST proRecordTaskSetList=parser.GetNodeList(proRecordTaskSetNode);

		for (int j=0;j<proRecordTaskSetList->Size();++j)
		{
			string DeviceID,Width,Height,Fps,Bps,TaskID,Index,ThreeDSet;
			pXMLNODE taskRecordNode=parser.GetNextNode(proRecordTaskSetList);
			parser.GetAttrNode(taskRecordNode,"TaskID",TaskID);
			parser.GetAttrNode(taskRecordNode,"Action",Action);
			parser.GetAttrNode(taskRecordNode,"Index",Index);
			parser.GetAttrNode(taskRecordNode,"Width",Width);
			parser.GetAttrNode(taskRecordNode,"Height",Height);
			parser.GetAttrNode(taskRecordNode,"Fps",Fps);
			parser.GetAttrNode(taskRecordNode,"Bps",Bps);
			parser.GetAttrNode(taskRecordNode,"ThreeDSet",ThreeDSet);
			PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"THREED");

			if (Action=="Del" || Action=="del")
			{
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				continue;
			}

			pXMLNODELIST taskRecordList=parser.GetNodeList(taskRecordNode);
			for (int k=0;k<taskRecordList->Size();++k)
			{
				string OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Freq,Code,DataType;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(taskRecordList);

				parser.GetAttrNode(childNode,"Freq",Freq);
				parser.GetAttrNode(childNode,"VideoPID",VideoPID);
				parser.GetAttrNode(childNode,"AudioPID",AudioPID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);

				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,TsID);
				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
				parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
				parser.GetAttrNode(childNode,"StartTime",StartTime);
				parser.GetAttrNode(childNode,"EndTime",EndTime);
				parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
				tempParser.SetAttrNode("Freq",Freq,recordNode);
				tempParser.SetAttrNode("Code",Code,recordNode);
				tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
				tempParser.SetAttrNode("TsID",TsID,recordNode);
				tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
				tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
				tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
				tempParser.SetAttrNode("DataType",DataType,recordNode);
				tempParser.SetAttrNode("Width",Width,recordNode);
				tempParser.SetAttrNode("Height",Height,recordNode);
				tempParser.SetAttrNode("Fps",Fps,recordNode);
				tempParser.SetAttrNode("Bps",Bps,recordNode);
				tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);

				tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
			}
		}	

	}
	else
	{
		return false;
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}
//返回标准的录像执行任务XML
bool TranslateDownXMLForTHREED::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	//录像标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	//获取根元素
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"DVBType",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("RecordTask"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	pXMLNODE MyRecordNode;
	pXMLNODE MyRecordSetNode;
	if(strOriginalXML.find("AutoRecordSet")!=string::npos)
	{
		MyRecordNode=tempParser.CreateNodePtr(tempRootNode,"AutoRecord");
		MyRecordSetNode=parser.GetNodeFromPath("Msg/AutoRecordSet");
	}
	else if(strOriginalXML.find("TaskRecordSet")!=string::npos)
	{
		MyRecordNode=tempParser.CreateNodePtr(tempRootNode,"TaskRecord");
		MyRecordSetNode=parser.GetNodeFromPath("Msg/TaskRecordSet");
	}
	else 
	{
		return false;
	}
	pXMLNODELIST recordSetList=parser.GetNodeList(MyRecordSetNode);
	for(int i=0;i<recordSetList->Size();++i)
	{
		string xml;
		string Action,DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID,VideoPID,AudioPID,Freq,DataType,ThreeDSet;
		string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
		pXMLNODE recordNode=tempParser.CreateNodePtr(MyRecordNode,"Record");
		pXMLNODE childNode=parser.GetNextNode(recordSetList);
		parser.GetAttrNode(childNode,"TaskID",TaskID);
		parser.GetAttrNode(childNode,"Action",Action);
		parser.GetAttrNode(childNode,"DeviceID",DeviceID);
		parser.GetAttrNode(childNode,"Freq",Freq);
		parser.GetAttrNode(childNode,"Code",Code);
		parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(childNode,"TsID",TsID);
		parser.GetAttrNode(childNode,"ServiceID",ServiceID);
		parser.GetAttrNode(childNode,"VideoPID",VideoPID);
		parser.GetAttrNode(childNode,"AudioPID",AudioPID);
		parser.GetAttrNode(childNode,"DataType",DataType);
		parser.GetAttrNode(childNode,"Width",Width);
		parser.GetAttrNode(childNode,"Height",Height);
		parser.GetAttrNode(childNode,"Fps",Fps);
		parser.GetAttrNode(childNode,"Bps",Bps);
		parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);

		parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
		parser.GetAttrNode(childNode,"StartTime",StartTime);
		parser.GetAttrNode(childNode,"EndTime",EndTime);
		parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
		parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);

		tempParser.SetAttrNode("TaskID",TaskID,recordNode);
		tempParser.SetAttrNode("Action",Action,recordNode);
		tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
		tempParser.SetAttrNode("Freq",Freq,recordNode);
		tempParser.SetAttrNode("Code",Code,recordNode);
		tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
		tempParser.SetAttrNode("TsID",TsID,recordNode);
		tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
		tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
		tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
		tempParser.SetAttrNode("DataType",DataType,recordNode);
		tempParser.SetAttrNode("Width",Width,recordNode);
		tempParser.SetAttrNode("Height",Height,recordNode);
		tempParser.SetAttrNode("Fps",Fps,recordNode);
		tempParser.SetAttrNode("Bps",Bps,recordNode);
		tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);

		tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
		tempParser.SetAttrNode("StartTime",StartTime,recordNode);
		tempParser.SetAttrNode("EndTime",EndTime,recordNode);
		tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
		tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);

		tempParser.SaveToString(xml);
		tempParser.DeleteNode(MyRecordNode,recordNode);
		strStandardXMLVec.push_back(xml);
	}
	return true;
}
//返回标准的录像查询、下载XML
bool TranslateDownXMLForTHREED::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//初始化属性值
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, OrgNetID, tsID, serviceID,VideoID,AudioID,code, StartDateTime, EndDateTime, DeviceID;
	std::string finalXml;

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取原始XML根元素
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	std::string nodeName = parser.GetNodeName( childNode );      //获取Msg的子节点名称
	if (nodeName =="NVRVideoHistoryDownInquiry")
	{
		nodeName = "AutoRecordFileQuery";                        //修改查询类型节点名称
	}
	else if (nodeName =="NVRVideoHistoryInquiry")
	{
		nodeName = "AutoRecordQuery";                            //修改查询类型节点名称
	}
	else if (nodeName =="NVRTaskRecordInquiry")
	{
		nodeName = "TaskRecordQuery";                            //修改查询类型节点名称
	}
	else if (nodeName =="NVRTaskRecordDownInquiry")
	{
		nodeName = "TaskRecordFileQuery";                        //修改查询类型节点名称
	}
	char * taskType = const_cast<char *>(nodeName.c_str());
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("RecordQuery"),tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);             //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);          //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);               //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);               //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                 //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);             //设置Msg节点的Priority属性值

	pXMLNODE query = tempParser.CreateNodePtr(tempRootNode,taskType);	         //创建查询类型节点
	pXMLNODE recordTaskNode = tempParser.CreateNodePtr(query,"RecordQueryTask"); //创建RecordQueryTask节点

	//获取录像任务各属性
	if ( (nodeName.compare("AutoRecordFileQuery") == 0)||(nodeName.compare("AutoRecordQuery") == 0) )
	{
		parser.GetAttrNode( recordNode,"Index",DeviceID);                    //获取Index属性值
		PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"THREED");	//将逻辑通道转化为实际通道
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //获取StartDateTime属性值
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //获取EndDateTime属性值
	}
	else if (nodeName.compare("TaskRecordQuery") == 0)
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );                    //获取TaskID属性值
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //获取StartDateTime属性值
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //获取EndDateTime属性值
	}
	else if (nodeName.compare("TaskRecordFileQuery")==0)
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );                    //获取TaskID属性值

	}
	//设置录像任务属性
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );               //设置RecordQueryTask节点的TaskID属性值
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,recordTaskNode );           //设置RecordQueryTask节点的OrgNetID属性值
		tempParser.SetAttrNode( "TsID",tsID,recordTaskNode );                   //设置RecordQueryTask节点的TsID属性值
		tempParser.SetAttrNode( "ServiceID",serviceID,recordTaskNode );         //设置RecordQueryTask节点的ServiceID属性值
		tempParser.SetAttrNode( "VideoPID",VideoID,recordTaskNode );             //设置RecordQueryTask节点的VideoID属性值
		tempParser.SetAttrNode( "AudioPID",AudioID,recordTaskNode);              //设置RecordQueryTask节点的AudioID属性值
		tempParser.SetAttrNode( "Code",code,recordTaskNode);                    //设置RecordQueryTask节点的Code属性值
		tempParser.SetAttrNode( "DeviceID",DeviceID,recordTaskNode );           //设置RecordQueryTask节点的DeviceID属性值
		tempParser.SetAttrNode( "StartDateTime",StartDateTime,recordTaskNode ); //设置RecordQueryTask节点的StartDateTime属性值
		tempParser.SetAttrNode( "EndDateTime",EndDateTime,recordTaskNode );     //设置RecordQueryTask节点的EndDateTime属性值
	}
	catch(...)
	{
		string error ="设置标准XML文档元素属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}
	//保存到字符串
	tempParser.SaveToString( strStandardXML);
	return true;
}

//返回标准的偱切录像设置任务XML
bool TranslateDownXMLForTHREED::TranslateRoundRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser originalParser( strOriginalXML.c_str() );
	XmlParser tempParser(source);

	//获取根元素
	pXMLNODE orgRootNode=originalParser.GetRootNode();
	pXMLNODE tempRootNode=tempParser.GetRootNode();

	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	originalParser.GetAttrNode(orgRootNode,"Version",version);
	originalParser.GetAttrNode(orgRootNode,"MsgID",msgid);
	originalParser.GetAttrNode(orgRootNode,"Type",downtype);
	originalParser.GetAttrNode(orgRootNode,"Protocol",protocol);
	originalParser.GetAttrNode(orgRootNode,"DateTime",datetime);
	originalParser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	originalParser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	originalParser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	originalParser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	string tasktype="RoundRecordSet";
	tempParser.SetAttrNode("Version",	version,	tempRootNode);
	tempParser.SetAttrNode("MsgID",		msgid,		tempRootNode);
	tempParser.SetAttrNode("DVBType",	downtype,	tempRootNode);
	tempParser.SetAttrNode("TaskType",	tasktype,	tempRootNode);
	tempParser.SetAttrNode("Protocol",	protocol,	tempRootNode);
	tempParser.SetAttrNode("DateTime",	datetime,	tempRootNode);
	tempParser.SetAttrNode("SrcCode",	srccode,	tempRootNode);
	tempParser.SetAttrNode("DstCode",	dstcode,	tempRootNode);
	tempParser.SetAttrNode("SrcURL",	srcurl,		tempRootNode);
	tempParser.SetAttrNode("Priority",	priority,	tempRootNode);

	//TaskID="100" Action="Set" Index="1" StartDateTime="" EndDateTime="" RoundTime=""
	std::string taskid,action,index,startdatetime,enddatetime,roundtime;
	int orgRdRecSetCount = originalParser.GetChildCount(orgRootNode);	//RoundRecordSet节点的个数
	pXMLNODELIST RdRecSetList = originalParser.GetNodeList(orgRootNode);//RoundRecordSet节点列表
	for (int nSet=0; nSet<orgRdRecSetCount; ++nSet)
	{
		pXMLNODE RdRecSetNode = originalParser.GetNextNode(RdRecSetList);	//<RoundRecordSet/>
		originalParser.GetAttrNode(RdRecSetNode, "TaskID",			taskid);
		originalParser.GetAttrNode(RdRecSetNode, "Action",			action);
		originalParser.GetAttrNode(RdRecSetNode, "Index",			index);
		originalParser.GetAttrNode(RdRecSetNode, "StartDateTime",	startdatetime);
		originalParser.GetAttrNode(RdRecSetNode, "EndDateTime",		enddatetime);
		originalParser.GetAttrNode(RdRecSetNode, "RoundTime",		roundtime);

		//TaskID="100" Action="Set" DeviceID="1" StartDateTime="" EndDateTime="" RoundTime=""
		pXMLNODE tempRdRecSetNode = tempParser.CreateNodePtr(tempRootNode, "RoundRecordSet");	//创建RoundRecordSet节点
		tempParser.SetAttrNode("TaskID",		taskid,			tempRdRecSetNode);
		tempParser.SetAttrNode("Action",		action,			tempRdRecSetNode);
		tempParser.SetAttrNode("DeviceID",		index,			tempRdRecSetNode);
		tempParser.SetAttrNode("StartDateTime", startdatetime,	tempRdRecSetNode);
		tempParser.SetAttrNode("EndDateTime",	enddatetime,	tempRdRecSetNode);
		tempParser.SetAttrNode("RoundTime",		roundtime,		tempRdRecSetNode);
		if ("Del" == action)	//如果是del操作，则跳过;  待测试!!!!!!!!!!!
		{
			continue;
		}
		//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />
		std::string freq,symbolrate,qam,serviceid,videopid,audiopid;
		int orgChanCount = originalParser.GetChildCount(RdRecSetNode);
		pXMLNODELIST orgChanList = originalParser.GetNodeList(RdRecSetNode);
		for (int nChan=0; nChan<orgChanCount; ++nChan)
		{
			pXMLNODE orgChanNode = originalParser.GetNextNode(orgChanList);
			originalParser.GetAttrNode(orgChanNode,	"Freq",			freq);
			originalParser.GetAttrNode(orgChanNode,	"SymbolRate",	symbolrate);
			originalParser.GetAttrNode(orgChanNode,	"QAM",			qam);
			originalParser.GetAttrNode(orgChanNode,	"ServiceID",	serviceid);
			originalParser.GetAttrNode(orgChanNode,	"VideoPID",		videopid);
			originalParser.GetAttrNode(orgChanNode,	"AudioPID",		audiopid);

			//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />
			pXMLNODE tempChanNode = tempParser.CreateNodePtr(tempRdRecSetNode, "Channel");
			tempParser.SetAttrNode("Freq",			freq,		tempChanNode);
			tempParser.SetAttrNode("SymbolRate",	symbolrate,	tempChanNode);
			tempParser.SetAttrNode("QAM",			qam,		tempChanNode);
			tempParser.SetAttrNode("ServiceID",		serviceid,	tempChanNode);
			tempParser.SetAttrNode("VideoPID",		videopid,	tempChanNode);
			tempParser.SetAttrNode("AudioPID",		audiopid,	tempChanNode);
		}
	}

	tempParser.SaveToString(strStandardXML);
	return true;
}
//返回标准的偱切录像执行任务XML
bool TranslateDownXMLForTHREED::TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RoundRecord></RoundRecord></Msg>";

	XmlParser orgXmlParser( strOriginalXML.c_str() );
	pXMLNODE orgRootNode = orgXmlParser.GetRootNode();
	std::string version,msgid,dvbtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	orgXmlParser.GetAttrNode(orgRootNode, "Version", version);
	orgXmlParser.GetAttrNode(orgRootNode, "MsgID", msgid);
	orgXmlParser.GetAttrNode(orgRootNode, "DVBType", dvbtype);
	orgXmlParser.GetAttrNode(orgRootNode, "Protocol", protocol);
	orgXmlParser.GetAttrNode(orgRootNode, "DateTime", datetime);
	orgXmlParser.GetAttrNode(orgRootNode, "SrcCode", srccode);
	orgXmlParser.GetAttrNode(orgRootNode, "DstCode", dstcode);
	orgXmlParser.GetAttrNode(orgRootNode, "SrcURL", srcurl);
	orgXmlParser.GetAttrNode(orgRootNode, "Priority", priority);
	//std::string tasktype = "RoundRecord";

	int RdRecSetCount = orgXmlParser.GetChildCount(orgRootNode);
	pXMLNODELIST RdRecSetList = orgXmlParser.GetNodeList(orgRootNode);
	for (int nSet=0;nSet<RdRecSetCount;++nSet)
	{
		XmlParser tempparser( source );
		pXMLNODE tempMsgNode = tempparser.GetRootNode();
		tempparser.SetAttrNode("Version", version, tempMsgNode);
		tempparser.SetAttrNode("MsgID", msgid, tempMsgNode);
		tempparser.SetAttrNode("DVBType", dvbtype, tempMsgNode);
		tempparser.SetAttrNode("TaskType",string("RoundRecordTask"),tempMsgNode);
		tempparser.SetAttrNode("Protocol", protocol, tempMsgNode);
		tempparser.SetAttrNode("DateTime", datetime, tempMsgNode);
		tempparser.SetAttrNode("SrcCode", srccode, tempMsgNode);
		tempparser.SetAttrNode("DstCode", dstcode, tempMsgNode);
		tempparser.SetAttrNode("SrcURL", srcurl, tempMsgNode);
		tempparser.SetAttrNode("Priority", priority, tempMsgNode);
		if (protocol.length()<1)
		{
			protocol = "HTTP";
		}

		pXMLNODE RdRecSetNode = orgXmlParser.GetNextNode(RdRecSetList);

		//<RoundRecordSet TaskID="100" Action="Set" DeviceID="1" StartDateTime="" EndDateTime="" RoundTime="">    
		std::string taskid,action,startdatetime,enddatetime,roundtime,deviceid;
		orgXmlParser.GetAttrNode(RdRecSetNode, "TaskID", taskid);
		orgXmlParser.GetAttrNode(RdRecSetNode, "Action", action);
		orgXmlParser.GetAttrNode(RdRecSetNode, "DeviceID", deviceid);
		orgXmlParser.GetAttrNode(RdRecSetNode, "StartDateTime", startdatetime);
		orgXmlParser.GetAttrNode(RdRecSetNode, "EndDateTime", enddatetime);
		orgXmlParser.GetAttrNode(RdRecSetNode, "RoundTime", roundtime);

		pXMLNODE tempSetNode = tempparser.GetNodeFromPath("Msg/RoundRecord");
		tempparser.SetAttrNode("TaskID", taskid, tempSetNode);
		tempparser.SetAttrNode("Action", action, tempSetNode);
		tempparser.SetAttrNode("DeviceID", deviceid, tempSetNode);
		tempparser.SetAttrNode("StartDateTime", startdatetime, tempSetNode);
		tempparser.SetAttrNode("EndDateTime", enddatetime, tempSetNode);
		tempparser.SetAttrNode("RoundTime", roundtime, tempSetNode);

		int ChanCount = orgXmlParser.GetChildCount(RdRecSetNode);
		pXMLNODELIST ChanList = orgXmlParser.GetNodeList(RdRecSetNode);
		for (int nChan=0;nChan<ChanCount;++nChan)
		{
			//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />   
			std::string freq,symbolrate,qam,serviceid,videopid,audiopid,OrgNetID,TsID;
			pXMLNODE orgChanNode = orgXmlParser.GetNextNode(ChanList);
			orgXmlParser.GetAttrNode(orgChanNode, "Freq", freq);
			orgXmlParser.GetAttrNode(orgChanNode, "SymbolRate", symbolrate);
			orgXmlParser.GetAttrNode(orgChanNode, "QAM", qam);
			orgXmlParser.GetAttrNode(orgChanNode, "ServiceID", serviceid);
			orgXmlParser.GetAttrNode(orgChanNode, "VideoPID", videopid);
			orgXmlParser.GetAttrNode(orgChanNode, "AudioPID", audiopid);

			enumDVBType Downtype = OSFunction::GetEnumDVBType(dvbtype);
			CHANNELINFOMGR::instance()->GetOrgNetID(Downtype,freq,audiopid,videopid,serviceid,OrgNetID);
			CHANNELINFOMGR::instance()->GetTsID(Downtype,freq,audiopid,videopid,serviceid,TsID);
			pXMLNODE tempChanNode = orgXmlParser.CreateNodePtr(tempSetNode, "Channel");
			tempparser.SetAttrNode("Freq", freq, tempChanNode);
			tempparser.SetAttrNode("OrgNetID", OrgNetID, tempChanNode);
			tempparser.SetAttrNode("TsID", TsID, tempChanNode);
			tempparser.SetAttrNode("SymbolRate", symbolrate, tempChanNode);
			tempparser.SetAttrNode("QAM", qam, tempChanNode);
			tempparser.SetAttrNode("ServiceID", serviceid, tempChanNode);
			tempparser.SetAttrNode("VideoPID", videopid, tempChanNode);
			tempparser.SetAttrNode("AudioPID", audiopid, tempChanNode);
		}

		std::string tempxml;
		tempparser.SaveToString(tempxml);
		strStandardXMLVec.push_back(tempxml);
	}
	return true;
}


//返回标准的实时指标查询XML
bool TranslateDownXMLForTHREED::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	string downType;//定义检测类型

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><QualityRealtimeQuery><QualityQueryTask/><QualityParam></QualityParam></QualityRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取根元素
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	pXMLNODE childNode = parser.GetNodeFirstChild(rootNode);;
	parser.GetAttrNode(rootNode,"Type",downType);

	//初始化属性值
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,msgid,version,protocol,srccode,dstcode,srcurl;
	string dayOfWeek,startTime,endTime,startDateTime,endDateTime,checkInterval;
	string Type,Desc,Value;
	int count = 0;
	string tasktype="QualityRealtimeQuery";	
	string periodicity="Once";
	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);
	parser.GetAttrNode( rootNode,"Priority",priority );

	if(downType=="3DDown")
	{
		downType="THREED";
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("Version",version,tempRootNode);
	tempParser.SetAttrNode("MsgID",msgid,tempRootNode);
	tempParser.SetAttrNode("DVBType",downType,tempRootNode );
	tempParser.SetAttrNode("TaskType",tasktype,tempRootNode );
	tempParser.SetAttrNode("Protocol",protocol,tempRootNode);
	tempParser.SetAttrNode("DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode("SrcCode",srccode,tempRootNode);
	tempParser.SetAttrNode("DstCode",dstcode,tempRootNode);
	tempParser.SetAttrNode("SrcURL",srcurl,tempRootNode);
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	//获取子元素属性
	parser.GetAttrNode( childNode,"Freq",freq );
	parser.GetAttrNode(childNode,"SymbolRate",symbolRate);


	tempParser.SetAttrNode( "TaskID",taskID,tempChildNode );
	tempParser.SetAttrNode( "Action",action,tempChildNode );
	tempParser.SetAttrNode("Periodicity",periodicity,tempChildNode);

	pXMLNODELIST childNodeList = tempParser.GetNodeList( tempChildNode );

	//获取QualityQueryTask元素
	pXMLNODE qualityTaskNode = tempParser.GetFirstNode( childNodeList ); 

	try
	{
		//设置QualityQueryTask元素属性
		tempParser.SetAttrNode( "DeviceID",deviceID,qualityTaskNode );
		tempParser.SetAttrNode( "STD",STD,qualityTaskNode );
		tempParser.SetAttrNode( "Freq",freq,qualityTaskNode );	
		tempParser.SetAttrNode( "SymbolRate",symbolRate,qualityTaskNode);

	}
	catch(...)
	{
		string error = "设置指标查询属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	pXMLNODE qualityParamNode = tempParser.GetNextNode( childNodeList );

	count = parser.GetChildCount( childNode );

	//创建返回XML的QualityIndex元素列表
	for ( int i = 0;i< count;i++ )
	{
		tempParser.CreateNode("Msg/QualityRealtimeQuery/QualityParam","QualityIndex");
	}
	//获取QualityIndex子元素列表
	pXMLNODELIST tempQualityIndexList = tempParser.GetNodeList( qualityParamNode );
	pXMLNODELIST qualityIndexList = parser.GetNodeList( childNode );
	pXMLNODE  listNode;
	pXMLNODE  tempListNode;

	try
	{
		//设置返回XML的QualityIndex元素属性
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				listNode = parser.GetFirstNode( qualityIndexList );
				tempListNode = tempParser.GetFirstNode( tempQualityIndexList );
			}
			else
			{
				listNode = parser.GetNextNode( qualityIndexList );
				tempListNode = tempParser.GetNextNode( tempQualityIndexList );
			}

			parser.GetAttrNode( listNode,"Type",Type );
			parser.GetAttrNode( listNode,"Desc",Desc );
			tempParser.SetAttrNode( "Type",Type,tempListNode );
			tempParser.SetAttrNode( "Desc",Desc,tempListNode );
			tempParser.SetAttrNode( "Value",Value,tempListNode );
		}
	}
	catch(...)
	{
		string error = "设置指标属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}

	//保存到字符串
	tempParser.SaveToString( strStandardXML );

	return true;
}
//返回标准的定时指标设置任务xml
bool TranslateDownXMLForTHREED::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{  
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //获取Msg节点的protocol属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}

	tempParser.SetAttrNode("TaskType",string("QualityTaskSet"),tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	pXMLNODE qualityReportNode = parser.GetNodeFirstChild( childNode );
	pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/Channel");      //获取原始XML中Channel节点
	pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/QualityParam"); //获取原始XML中QualityParam节点

	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"QualityTaskSet");  //创建QualityTaskSet节点
	pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //创建TaskParm节点
	pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //创建QualityParam节点
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //创建TimeParam节点
	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //获取原始XML中QualityReport节点的TaskID属性
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //获取原始XML中QualityReport节点的Action属性
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //设置QualityQuerySet节点的TaskID属性
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //设置QualityQuerySet节点的Action属性

	std::string STD,Freq,SymbolRate;
	pXMLNODELIST ChanneList=parser.GetNodeList(channelNode);
	for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
	{  
		pXMLNODE channelInfo = parser.GetNextNode(ChanneList);
		parser.GetAttrNode(channelInfo,"STD",STD);                                     //获取Channel节点的STD属性
		parser.GetAttrNode(channelInfo,"Freq",Freq);								   //获取Channel节点的Freq属性
		parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //获取Channel节点的SymbolRate属性
		pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
		tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
		tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
		tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
	}
	std::string Type,Desc;
	pXMLNODELIST QualityList=parser.GetNodeList(qualityNode);
	for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
	{
		pXMLNODE qualityIndex = parser.GetNextNode(QualityList);
		parser.GetAttrNode(qualityIndex,"Type",Type);                                          //获取原始XML中QualityIndex节点的Type属性
		parser.GetAttrNode(qualityIndex,"Desc",Desc);										   //获取原始XML中QualityIndex节点的Desc属性
		pXMLNODE qualityIndexNode = tempParser.CreateNodePtr(qualityParamNode,"QualityIndex"); //创建QualityParam节点的子节点
		tempParser.SetAttrNode("Type",Type,qualityIndexNode);                                  //设置QualityIndex节点的Type属性
		tempParser.SetAttrNode("Desc",Desc,qualityIndexNode);								   //设置QualityIndex节点的Desc属性
	}
	std::string DayOfWeek,StartTime,EndTime,StartDateTime,EndDateTime,CheckInterval;
	pXMLNODELIST paramNodeList = parser.GetNodeList(qualityReportNode);
	for (int p = 0;p<parser.GetChildCount(qualityReportNode);p++)
	{
		pXMLNODE checkTimeNode = parser.GetNextNode(paramNodeList);
		std::string strNodeName = parser.GetNodeName(checkTimeNode);
		if (strNodeName =="CycleReportTime"||strNodeName =="SingleReportTime")
		{
			if (strNodeName =="CycleReportTime")
			{
				parser.GetAttrNode(checkTimeNode,"DayOfWeek",DayOfWeek);                   //获取原始XML中CycleReportTime节点的DayOfWeek属性
				parser.GetAttrNode(checkTimeNode,"StartTime",StartTime);                   //获取原始XML中CycleReportTime节点的StartTime属性
				parser.GetAttrNode(checkTimeNode,"EndTime",EndTime);                       //获取原始XML中CycleReportTime节点的EndTime属性
				parser.GetAttrNode(checkTimeNode,"CheckInterval",CheckInterval);           //获取原始XML中CycleReportTime节点的CheckInterval属性
			}
			else if (strNodeName =="SingleReportTime")
			{
				parser.GetAttrNode(checkTimeNode,"StartDateTime",StartDateTime);          //获取原始XML中SingleReportTime节点的StartDateTime属性
				parser.GetAttrNode(checkTimeNode,"EndDateTime",EndDateTime);			  //获取原始XML中SingleReportTime节点的EndDateTime属性
				parser.GetAttrNode(checkTimeNode,"CheckInterval",CheckInterval);          //获取原始XML中SingleReportTime节点的CheckInterval属性
			}
			pXMLNODE timeIndexNode = tempParser.CreateNodePtr(timeParamNode,"TimeIndex");   //创建TimeParam节点的子节点

			tempParser.SetAttrNode("DayOfWeek",DayOfWeek,timeIndexNode);                    //设置TimeIndex节点的DayOfWeek属性
			tempParser.SetAttrNode("StartTime",StartTime,timeIndexNode);                    //设置TimeIndex节点的StartTime属性
			tempParser.SetAttrNode("EndTime",EndTime,timeIndexNode);                        //设置TimeIndex节点的EndTime属性
			tempParser.SetAttrNode("StartDateTime",StartDateTime,timeIndexNode);            //设置TimeIndex节点的StartDateTime属性
			tempParser.SetAttrNode("EndDateTime",EndDateTime,timeIndexNode);                //设置TimeIndex节点的EndDateTime属性
			tempParser.SetAttrNode("CheckInterval",CheckInterval,timeIndexNode);            //设置TimeIndex节点的CheckInterval属性
			//重新初始化指标设置中与时间相关变量
			DayOfWeek = "";
			StartTime = "";
			EndTime ="";
			StartDateTime ="";
			EndDateTime = "";
			CheckInterval = "";
		}
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}
//返回标准的定时指标执行任务xml
bool TranslateDownXMLForTHREED::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	size_t pos;
	std::string orgXML = strOriginalXML;
	std::vector<std::string> timeIndexVct;
	while ((pos=orgXML.find("<TimeIndex"))!=std::string::npos&&orgXML.length()>0)//查找xml中是否含有TimeIndex
	{
		size_t pos2=orgXML.find(">",pos);
		if (pos2>0)
		{
			timeIndexVct.push_back(orgXML.substr(pos,pos2-pos+1));//TimeIndex节点信息保存
			orgXML.erase(pos,pos2-pos+1);//删除xml中的TimeIndex节点
		}
	}

	for (size_t p=0;p<timeIndexVct.size();++p)
	{
		std::string tmpstr=orgXML;
		std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
		std::string TaskID, Action,DevideID;

		char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser parser( tmpstr.c_str());
		XmlParser tempParser( source );

		pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
		parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
		parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
		parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //获取Msg节点的protocol属性值
		parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
		parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
		parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
		parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值	   
		parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值


		pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
		tempParser.SetAttrNode("Version",strVersion,tempRootNode);                   //设置Msg节点的Version属性值
		tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                       //设置Msg节点的MsgID属性值
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);               //设置Msg节点的DVBType属性值
		tempParser.SetAttrNode("TaskType",string("QualityTask"),tempRootNode);    //设置Msg节点的TaskType属性值
		tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
		tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
		tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
		tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
		tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
		tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

		pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
		pXMLNODE taskNode = parser.GetNodeFromPath("Msg/QualityTaskSet/TaskParm");      //获取原始XML中TaskParm节点
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityParam"); //获取原始XML中QualityParam节点

		pXMLNODE qualityTaskNode = tempParser.CreateNodePtr(tempRootNode,"QualityTask");  //创建QualityTask节点
		pXMLNODE taskParmNode = tempParser.CreateNodePtr(qualityTaskNode,"TaskParm");         //创建TaskParm节点
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(qualityTaskNode,"QualityParam"); //创建QualityParam节点
		parser.GetAttrNode(childNode,"TaskID",TaskID);				     			  //获取原始XML中QualityTaskSet节点的TaskID属性
		parser.GetAttrNode(childNode,"Action",Action);                                //获取原始XML中QualityTaskSet节点的Action属性
		tempParser.SetAttrNode("TaskID",TaskID,qualityTaskNode);                              //设置QualityTask节点的TaskID属性
		tempParser.SetAttrNode("Action",Action,qualityTaskNode);                              //设置QualityTask节点的Action属性
		tempParser.SetAttrNode("DeviceID",DevideID,qualityTaskNode);                          //设置QualityTask节点的DevieceID属性

		std::string STD,Freq,SymbolRate;
		pXMLNODELIST taskList=parser.GetNodeList(taskNode);
		for ( int i = 0;i<parser.GetChildCount(taskNode);i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskList);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //获取TaskParm节点的STD属性
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //获取TaskParm节点的Freq属性
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //获取TaskParm节点的SymbolRate属性
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
		}
		std::string Type,Desc;
		pXMLNODELIST QualityList=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(QualityList);
			parser.GetAttrNode(qualityIndex,"Type",Type);                                          //获取原始XML中QualityIndex节点的Type属性
			parser.GetAttrNode(qualityIndex,"Desc",Desc);										   //获取原始XML中QualityIndex节点的Desc属性
			pXMLNODE qualityIndexNode = tempParser.CreateNodePtr(qualityParamNode,"QualityIndex"); //创建QualityParam节点的子节点
			tempParser.SetAttrNode("Type",Type,qualityIndexNode);                                  //设置QualityIndex节点的Type属性
			tempParser.SetAttrNode("Desc",Desc,qualityIndexNode);								   //设置QualityIndex节点的Desc属性
		}
		tempParser.SaveToString(tmpstr);

		//根据timeIndexVct中的TimeIndex节点信息生成任务
		size_t pos4=tmpstr.find("</QualityParam>");
		if (pos4>0)
		{
			std::string timeParam = "<TimeParam>"+ timeIndexVct[p]+"</TimeParam>";
			tmpstr.insert(pos4+15,timeParam);
			strStandardXMLVec.push_back(tmpstr);
		}
	}
	return true;
}
//返回标准的定时指标结果查询任务xml
bool TranslateDownXMLForTHREED::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //获取Msg节点的protocol属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("QualityQuery"),tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

	pXMLNODE qualityQueryNode = parser.GetNodeFromPath("Msg/QualityQuery");
	pXMLNODE qualityNode = parser.GetNodeFirstChild(qualityQueryNode);
	pXMLNODE tempQualityQueryNode =tempParser.CreateNodePtr(tempRootNode,"QualityQuery");  //创建QualityQuery节点
	pXMLNODE tempQualityNode = tempParser.CreateNodePtr(tempQualityQueryNode,"Quality");   //创建Quality节点

	std::string TaskID, EquCode,STD,Freq,StartDateTime,EndDateTime,ReportTime;
	parser.GetAttrNode(qualityNode,"TaskID",TaskID);                             //获取原始XML中Quality节点的TaskID属性值
	parser.GetAttrNode(qualityNode,"EquCode",EquCode);                           //获取原始XML中Quality节点的EquCode属性值
	parser.GetAttrNode(qualityNode,"STD",STD);                                   //获取原始XML中Quality节点的STD属性值
	parser.GetAttrNode(qualityNode,"Freq",Freq);                                 //获取原始XML中Quality节点的Freq属性值
	parser.GetAttrNode(qualityNode,"StartDateTime",StartDateTime);               //获取原始XML中Quality节点的StartDateTime属性值
	parser.GetAttrNode(qualityNode,"EndDateTime",EndDateTime);                   //获取原始XML中Quality节点的EndDateTime属性值
	parser.GetAttrNode(qualityNode,"ReportTime",ReportTime);                     //获取原始XML中Quality节点的ReportTime属性值

	tempParser.SetAttrNode("TaskID",TaskID,tempQualityNode);                     //设置标准XML中Quality节点的TaskID属性值
	tempParser.SetAttrNode("EquCode",EquCode,tempQualityNode);					 //设置标准XML中Quality节点的EquCode属性值
	tempParser.SetAttrNode("STD",STD,tempQualityNode);							 //设置标准XML中Quality节点的STD属性值
	tempParser.SetAttrNode("Freq",Freq,tempQualityNode);						 //设置标准XML中Quality节点的Freq属性值
	tempParser.SetAttrNode("StartDateTime",StartDateTime,tempQualityNode);		 //设置标准XML中Quality节点的StartDateTime属性值
	tempParser.SetAttrNode("EndDateTime",EndDateTime,tempQualityNode);			 //设置标准XML中Quality节点的EndDateTime属性值
	tempParser.SetAttrNode("ReportTime",ReportTime,tempQualityNode);			 //设置标准XML中Quality节点的ReportTime属性值

	tempParser.SaveToString(strStandardXML);   //保存到字符串
	return true;
}
//返回标准的频道设置任务xml
bool TranslateDownXMLForTHREED::TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,
		strDstCode,strSrcURL,strPriority;

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);           //获取Msg节点的SrcUrl属性值
	if (strType =="3DDown")
	{
		 strType = "THREED";          //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	string strTaskType = "ChannelSet";
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	tempParser.SetAttrNode("DVBType",strType,tempRootNode);
	tempParser.SetAttrNode("TaskType",strTaskType,tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

	pXMLNODE channelSetNode = parser.GetNodeFromPath("Msg/ChannelSet");
	pXMLNODELIST channelNodeList = parser.GetNodeList(channelSetNode);
	pXMLNODE tempChannelSetNode =tempParser.CreateNodePtr(tempRootNode,"ChannelSet");  //创建QualityQuery节点

	std::string STD,Freq,QAM,HeaderMode,CarrierNumber,EncodeEfficiency,InterwovenMode;
	std::string DoublePilot,PpnPhase,OrgNetID,TsID,Program,ServiceID,VideoPID,AudioPID,Encryption;
	for (int i = 0;i<parser.GetChildCount(channelSetNode);i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);

		parser.GetAttrNode(channelNode,"STD",STD);                                //获取原始XML中Channel节点的STD属性值
		parser.GetAttrNode(channelNode,"Freq",Freq);                              //获取原始XML中Channel节点的Freq属性值
		parser.GetAttrNode(channelNode,"QAM",QAM);                                //获取原始XML中Channel节点的QAM属性值
		parser.GetAttrNode(channelNode,"HeaderMode",HeaderMode);                  //获取原始XML中Channel节点的HeaderMode属性值
		parser.GetAttrNode(channelNode,"CarrierNumber",CarrierNumber);            //获取原始XML中Channel节点的CarrierNumber属性值
		parser.GetAttrNode(channelNode,"EncodeEfficiency",EncodeEfficiency);      //获取原始XML中Channel节点的EncodeEfficiency属性值
		parser.GetAttrNode(channelNode,"InterwovenMode",InterwovenMode);          //获取原始XML中Channel节点的InterwovenMode属性值
		parser.GetAttrNode(channelNode,"DoublePilot",DoublePilot);                //获取原始XML中Channel节点的DoublePilot属性值
		parser.GetAttrNode(channelNode,"PpnPhase",PpnPhase);                      //获取原始XML中Channel节点的PpnPhase属性值
		parser.GetAttrNode(channelNode,"OrgNetID",OrgNetID);                      //获取原始XML中Channel节点的OrgNetID属性值
		parser.GetAttrNode(channelNode,"TsID",TsID);                              //获取原始XML中Channel节点的TsID属性值
		parser.GetAttrNode(channelNode,"Program",Program);                        //获取原始XML中Channel节点的Program属性值
		parser.GetAttrNode(channelNode,"ServiceID",ServiceID);                    //获取原始XML中Channel节点的ServiceID属性值
		parser.GetAttrNode(channelNode,"VideoPID",VideoPID);                      //获取原始XML中Channel节点的VideoPID属性值
		parser.GetAttrNode(channelNode,"AudioPID",AudioPID);                      //获取原始XML中Channel节点的AudioPID属性值
		parser.GetAttrNode(channelNode,"Encryption",Encryption);                  //获取原始XML中Channel节点的Encryption属性值


		pXMLNODE tempChannelNode = tempParser.CreateNodePtr(tempChannelSetNode,"Channel");//创建标准XML中Channel节点
		tempParser.SetAttrNode("STD",STD,tempChannelNode);                                //设置标准XML中Channel节点的STD属性值
		tempParser.SetAttrNode("Freq",Freq,tempChannelNode);                              //设置标准XML中Channel节点的Freq属性值
		tempParser.SetAttrNode("QAM",QAM,tempChannelNode);                                //设置标准XML中Channel节点的QAM属性值
		tempParser.SetAttrNode("HeaderMode",HeaderMode,tempChannelNode);                  //设置标准XML中Channel节点的HeaderMode属性值
		tempParser.SetAttrNode("CarrierNumber",CarrierNumber,tempChannelNode);            //设置标准XML中Channel节点的CarrierNumber属性值
		tempParser.SetAttrNode("EncodeEfficiency",EncodeEfficiency,tempChannelNode);      //设置标准XML中Channel节点的EncodeEfficiency属性值
		tempParser.SetAttrNode("InterwovenMode",InterwovenMode,tempChannelNode);          //设置标准XML中Channel节点的InterwovenMode属性值
		tempParser.SetAttrNode("DoublePilot",DoublePilot,tempChannelNode);                //设置标准XML中Channel节点的DoublePilot属性值
		tempParser.SetAttrNode("PpnPhase",PpnPhase,tempChannelNode);                      //设置标准XML中Channel节点的PpnPhase属性值
		tempParser.SetAttrNode("OrgNetID",OrgNetID,tempChannelNode);                      //设置标准XML中Channel节点的OrgNetID属性值
		tempParser.SetAttrNode("TsID",TsID,tempChannelNode);                              //设置标准XML中Channel节点的TsID属性值
		tempParser.SetAttrNode("Program",Program,tempChannelNode);                        //设置标准XML中Channel节点的Program属性值
		tempParser.SetAttrNode("ServiceID",ServiceID,tempChannelNode);                    //设置标准XML中Channel节点的ServiceID属性值
		tempParser.SetAttrNode("VideoPID",VideoPID,tempChannelNode);                      //设置标准XML中Channel节点的VideoPID属性值
		tempParser.SetAttrNode("AudioPID",AudioPID,tempChannelNode);                      //设置标准XML中Channel节点的AudioPID属性值
		tempParser.SetAttrNode("Encryption",Encryption,tempChannelNode);                  //设置标准XML中Channel节点的Encryption属性值
	}
	tempParser.SaveToString(strStandardXML);   //保存到字符串
	return true;
}
//返回标准的客户信息查询任务xml
bool TranslateDownXMLForTHREED::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ClientInfoQuery></ClientInfoQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ClientInfoQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);


	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/ClientInfoQuery","HeadendInfo");
	}
	//获取ClientIndex子元素列表
	pXMLNODE    ClientInfo=temParser.GetNodeFromPath("Msg/ClientInfoQuery");
	pXMLNODELIST stdClientIndexList = temParser.GetNodeList( ClientInfo );
	pXMLNODELIST orgClientIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string index;
	try
	{
		//设置返回XML的ClientIndex元素属性
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				orglistNode = parser.GetFirstNode( orgClientIndexList );
				stdListNode = temParser.GetFirstNode( stdClientIndexList );
			}
			else
			{
				orglistNode = parser.GetNextNode( orgClientIndexList );
				stdListNode = temParser.GetNextNode( stdClientIndexList );
			}

			parser.GetAttrNode( orglistNode,"Index",index );
			temParser.SetAttrNode( "Index",index,stdListNode );
		}
	}
	catch(...)
	{
		string error = "设置用客户信息属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);

	return true;
}
//返回标准的通道状态查询任务xml
bool TranslateDownXMLForTHREED::TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><DeviceStatusQuery></DeviceStatusQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="DeviceStatusQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/DeviceStatusQuery","DeviceStatus");
	}

	pXMLNODE    StatusInfo=temParser.GetNodeFromPath("Msg/DeviceStatusQuery");
	string index,equcode;
	temParser.SetAttrNode("Index",index,StatusInfo);
	temParser.SetAttrNode("EquCode",equcode,StatusInfo);
	//获取StatusIndex子元素列表
	pXMLNODELIST stdStatusIndexList = temParser.GetNodeList( StatusInfo );
	pXMLNODELIST orgStatusIndexList = parser.GetNodeList( childNode );
	string deviceid;

	try
	{
		//设置返回XML的StatusIndex元素属性
		for ( int j = 0;j< count;j++ )
		{
			pXMLNODE  orglistNode = parser.GetNextNode( orgStatusIndexList );
			pXMLNODE  stdListNode = temParser.GetNextNode( stdStatusIndexList );
			parser.GetAttrNode( orglistNode,"Index",deviceid );
			temParser.SetAttrNode( "DeviceID",deviceid,stdListNode );
		}
	}
	catch(...)
	{
		string error = "设置通道状态属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);

	return true;
}
//返回标准的表查询任务xml
bool TranslateDownXMLForTHREED::TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TableQuery></TableQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="TableQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODELIST nodeList=parser.GetNodeList(orgRootNode);
	for(int i=0;i<nodeList->Size();++i)
	{
		pXMLNODE stdTabelNode=temParser.CreateNodePtr("Msg/TableQuery","Table");
		pXMLNODE orgTableNode=parser.GetNextNode(nodeList);
		parser.GetAttrNode(orgTableNode,"EquCode",equcode);
		parser.GetAttrNode(orgTableNode,"STD",stdtype);
		parser.GetAttrNode(orgTableNode,"Freq",freq);
		parser.GetAttrNode(orgTableNode,"SymbolRate",rate);
		parser.GetAttrNode(orgTableNode,"QueryTime",querytime);

		temParser.SetAttrNode("EquCode",equcode,stdTabelNode);
		temParser.SetAttrNode("STD",stdtype,stdTabelNode);
		temParser.SetAttrNode("Freq",freq,stdTabelNode);
		temParser.SetAttrNode("SymbolRate",rate,stdTabelNode);
		temParser.SetAttrNode("QueryTime",querytime,stdTabelNode);
	}

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

//返回标准的运行图设置任务xml
bool TranslateDownXMLForTHREED::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RunPlanSet></RunPlanSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="RunPlanSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE stdRunSetNode=temParser.GetNodeFromPath("Msg/RunPlanSet");

	pXMLNODELIST orgRunPlanList = parser.GetNodeList( orgRootNode );
	int runPlanCount = parser.GetChildCount( orgRootNode );	//how many RunPlanSet node?
	try
	{
		for ( int runPlanIndex=0; runPlanIndex< runPlanCount; runPlanIndex++ )
		{
			pXMLNODE RunPlanSetNode = parser.GetNextNode( orgRunPlanList );

			string EquCode,Freq,OrgNetID,TsID,ServiceID,VideoPID, AudioPID;
			parser.GetAttrNode(RunPlanSetNode,"EquCode",EquCode);
			parser.GetAttrNode(RunPlanSetNode,"Freq",Freq);
			parser.GetAttrNode(RunPlanSetNode,"ServiceID",ServiceID);
			parser.GetAttrNode(RunPlanSetNode,"VideoPID",VideoPID);
			parser.GetAttrNode(RunPlanSetNode,"AudioPID",AudioPID);

			enumDVBType dvbtype = OSFunction::GetEnumDVBType(downtype);
			CHANNELINFOMGR::instance()->GetOrgNetID(dvbtype,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
			CHANNELINFOMGR::instance()->GetTsID(dvbtype,Freq,AudioPID,VideoPID,ServiceID,TsID);

			int timeParamCount = parser.GetChildCount(RunPlanSetNode);
			pXMLNODELIST timeParamList = parser.GetNodeList( RunPlanSetNode );
			for(int timeIndex=0; timeIndex<timeParamCount; timeIndex++)
			{
				pXMLNODE timeParamNode = parser.GetNextNode( timeParamList );

				string Month,Day,DayOfWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				if(parser.GetNodeName(timeParamNode)=="MonthTime")
				{
					parser.GetAttrNode(timeParamNode,"Month",Month);
					parser.GetAttrNode(timeParamNode,"Day",Day);
					parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
					parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
				}
				else if(parser.GetNodeName(timeParamNode)=="WeeklyTime")
				{
					parser.GetAttrNode(timeParamNode,"DayofWeek",DayOfWeek);
					parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
					parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
				}
				else if(parser.GetNodeName(timeParamNode)=="DayTime")
				{
					parser.GetAttrNode(timeParamNode,"StartDateTime",StartDateTime);
					parser.GetAttrNode(timeParamNode,"EndDateTime",EndDateTime);
				}

				string RunPlanType, AlarmEndTime;
				parser.GetAttrNode(timeParamNode, "Type", RunPlanType);
				parser.GetAttrNode(timeParamNode, "AlarmEndTime", AlarmEndTime);

				string ValidStartDateTime,ValidEndDateTime;
				pXMLNODE stdListNode = temParser.CreateNodePtr(stdRunSetNode, "RunPlanChannel");
				temParser.SetAttrNode("EquCode",EquCode,stdListNode);
				temParser.SetAttrNode("OrgNetID",OrgNetID,stdListNode);
				temParser.SetAttrNode("TsID",TsID,stdListNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdListNode);
				temParser.SetAttrNode("VideoPID",VideoPID,stdListNode);
				temParser.SetAttrNode("AudioPID",AudioPID,stdListNode);
				temParser.SetAttrNode("Month",Month,stdListNode);
				temParser.SetAttrNode("DayOfMonth",Day,stdListNode);
				temParser.SetAttrNode("DayOfWeek",DayOfWeek,stdListNode);
				temParser.SetAttrNode("StartTime",StartTime,stdListNode);
				temParser.SetAttrNode("EndTime",EndTime,stdListNode);
				temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,stdListNode);

				//把AlarmEndTime放到ValidEndDateTime中
				temParser.SetAttrNode("ValidEndDateTime",AlarmEndTime,stdListNode);
				temParser.SetAttrNode("StartDateTime",StartDateTime,stdListNode);
				temParser.SetAttrNode("EndDateTime",EndDateTime,stdListNode);
				temParser.SetAttrNode("Type", RunPlanType, stdListNode);
			}
		}
	}
	catch(...)
	{
		string error = "设置运行图属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;

	}

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回标准的手动录制任务xml
bool TranslateDownXMLForTHREED::TranslateManualRecordSet(const std::string &strOriginalXML, std::string &strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ManualRecordSet></ManualRecordSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ManualRecordSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE  manualRecordQueryNode = parser.GetNodeFromPath("Msg/ManualRecordQuery");
	pXMLNODE  tempRecordSetNode = temParser.GetNodeFromPath("Msg/ManualRecordSet");
	pXMLNODELIST recordNodelist = parser.GetNodeList(manualRecordQueryNode);      //获取ManualRecordQuery的子节点列表
	int count = parser.GetChildCount(manualRecordQueryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE recordNode = parser.GetNextNode(recordNodelist);
		pXMLNODE  tempRecordNode = temParser.CreateNodePtr(tempRecordSetNode,"ManualRecord");    //创建ManualRecord节点
		std::string deviceid,time,path;
		parser.GetAttrNode(recordNode,"Index",deviceid);
		parser.GetAttrNode(recordNode,"Time",time);
		parser.GetAttrNode(recordNode,"Path",path);
		temParser.SetAttrNode("DeviceID",deviceid,tempRecordNode);
		temParser.SetAttrNode("Time",time,tempRecordNode);
		temParser.SetAttrNode("Path",path,tempRecordNode);
	}
	temParser.SaveToString(strStandardXML);
	return true;

}
//返回标准的数据业务分析时间设置任务xml
bool TranslateDownXMLForTHREED::TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AutoAnalysisTimeSet></AutoAnalysisTimeSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="AutoAnalysisTimeSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE  timeQueryNode = parser.GetNodeFromPath("Msg/AutoAnalysisTimeQuery");
	pXMLNODE  tempTimeQueryNode = temParser.GetNodeFromPath("Msg/AutoAnalysisTimeSet");
	pXMLNODELIST queryNodelist = parser.GetNodeList(timeQueryNode);     //获取AutoAnalysisTimeQuery的子节点列表
	int count = parser.GetChildCount(timeQueryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE timeNode = parser.GetNextNode(queryNodelist);
		pXMLNODE tempTimeNode = temParser.CreateNodePtr(tempTimeQueryNode,"AutoAnalysisTime");  //创建AutoAnalysisTime节点
		std::string starttime,type;
		parser.GetAttrNode(timeNode,"StartTime",starttime);
		parser.GetAttrNode(timeNode,"Type",type);
		temParser.SetAttrNode("StartTime",starttime,tempTimeNode);
		temParser.SetAttrNode("Type",type,tempTimeNode);
	}
	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForTHREED::TranslateEPGQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><EPGQuery></EPGQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="EPGQuery";
	string ScanTime;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE queryNode=parser.GetNodeFromPath("Msg/EPGQuery");
	parser.GetAttrNode(queryNode,"ScanTime",ScanTime);
	pXMLNODE retQueryNode=temParser.CreateNodePtr(orgRootNode,"EPGQuery");
	temParser.SetAttrNode("ScanTime",ScanTime,retQueryNode);

	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForTHREED::TranslateMHPQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><MHPQuery></MHPQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="MHPQuery";
	string ScanTime;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE queryNode=parser.GetNodeFromPath("Msg/MHPQuery");
	parser.GetAttrNode(queryNode,"ScanTime",ScanTime);
	pXMLNODE retQueryNode=temParser.CreateNodePtr(orgRootNode,"MHPQuery");
	temParser.SetAttrNode("ScanTime",ScanTime,retQueryNode);
	temParser.SaveToString(strStandardXML);
	return true;
}

//返回标准的环境指标查询任务xml
bool TranslateDownXMLForTHREED::TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><EnvironmentQuery></EnvironmentQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="EnvironmentQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/EnvironmentQuery","Environment");
	}

	pXMLNODE    IndexDesc=temParser.GetNodeFromPath("Msg/EnvironmentQuery");


	pXMLNODELIST stdIndexList = temParser.GetNodeList( IndexDesc );
	pXMLNODELIST orgIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string type,desc;

	try
	{
		//设置返回XML的StatusIndex元素属性
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				orglistNode = parser.GetFirstNode( orgIndexList );
				stdListNode = temParser.GetFirstNode( stdIndexList );
			}
			else
			{
				orglistNode = parser.GetNextNode( orgIndexList );
				stdListNode = temParser.GetNextNode( stdIndexList );
			}

			parser.GetAttrNode( orglistNode,"Type",type );
			parser.GetAttrNode(orglistNode,"Desc",desc);
			temParser.SetAttrNode( "Type",type,stdListNode );
			temParser.SetAttrNode("Desc",desc,stdListNode);
		}
	}
	catch(...)
	{
		string error = "设置环境指标属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}


	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回标准的频谱扫描任务xml
bool TranslateDownXMLForTHREED::TranslateSpectrumScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><SpectrumScanQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,STD,SymbolRate,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="SpectrumScanQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE orgchildNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdchildNode=temParser.GetNodeFromPath("Msg/SpectrumScanQuery");
	parser.GetAttrNode(orgchildNode,"ScanTime",ScanTime);
	parser.GetAttrNode(orgchildNode,"STD",STD);
	parser.GetAttrNode(orgchildNode,"SymbolRate",SymbolRate);
	parser.GetAttrNode(orgchildNode,"QAM",QAM);
	parser.GetAttrNode(orgchildNode,"StartFreq",StartFreq);
	parser.GetAttrNode(orgchildNode,"EndFreq",EndFreq);
	parser.GetAttrNode(orgchildNode,"StepFreq",StepFreq);

	temParser.SetAttrNode("ScanTime",ScanTime,stdchildNode);
	temParser.SetAttrNode("STD",STD,stdchildNode);
	temParser.SetAttrNode("SymbolRate",SymbolRate,stdchildNode);
	temParser.SetAttrNode("QAM",QAM,stdchildNode);
	temParser.SetAttrNode("StartFreq",StartFreq,stdchildNode);
	temParser.SetAttrNode("EndFreq",EndFreq,stdchildNode);
	temParser.SetAttrNode("StepFreq",StepFreq,stdchildNode);

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回指标补偿标准xml
bool TranslateDownXMLForTHREED::TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="QualityCompensationSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);


	int count = 0;
	count=parser.GetChildCount( orgRootNode );
	for(int j=0;j<count;j++)
	{
		temParser.CreateNode("Msg","QualityCompensationSet");
	}

	pXMLNODELIST stdIndexList = temParser.GetNodeList( stdRootNode );
	pXMLNODELIST orgIndexList = parser.GetNodeList( orgRootNode );
	for (int i=0;i<count;i++)
	{
		pXMLNODE orglistNode;
		pXMLNODE stdListNode;
		string deviceid;
		if ( i == 0 )
		{
			orglistNode = parser.GetFirstNode( orgIndexList );
			stdListNode = temParser.GetFirstNode( stdIndexList );
		}
		else
		{
			orglistNode = parser.GetNextNode( orgIndexList );
			stdListNode = temParser.GetNextNode( stdIndexList );
		}
		parser.GetAttrNode(orglistNode,"Index",deviceid);
		temParser.SetAttrNode("DeviceID",deviceid,stdListNode);

		int childcount=parser.GetChildCount(orglistNode);
		int k=0;
		for(k=0;k<childcount;k++)
		{
			temParser.CreateNodePtr(stdListNode,"Compensation");
		}
		pXMLNODELIST stdChildIndexList = temParser.GetNodeList(stdListNode);
		pXMLNODELIST orgChildIndexList = parser.GetNodeList(orglistNode);
		for(k=0;k<childcount;k++)
		{
			pXMLNODE orgChildlistNode;
			pXMLNODE stdChildListNode;
			string type,desc,val;
			if(k==0)
			{
				orgChildlistNode = parser.GetFirstNode(orgChildIndexList);
				stdChildListNode = parser.GetFirstNode(stdChildIndexList);
			}
			else
			{
				orgChildlistNode = parser.GetNextNode(orgChildIndexList);
				stdChildListNode = parser.GetNextNode(stdChildIndexList);
			}
			parser.GetAttrNode(orgChildlistNode,"Type",type);
			parser.GetAttrNode(orgChildlistNode,"Desc",desc);
			parser.GetAttrNode(orgChildlistNode,"Value",val);

			temParser.SetAttrNode("Type",type,stdChildListNode);
			temParser.SetAttrNode("Desc",desc,stdChildListNode);
			temParser.SetAttrNode("Value",val,stdChildListNode);
		}
	}
	temParser.SaveToString(strStandardXML);

	return true;
}
//返回报警参数设置标准xml
bool TranslateDownXMLForTHREED::TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="AlarmParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string STD, Freq,SymbolRate;
	pXMLNODE FreqNode=temParser.GetNodeFromPath("Msg/AlarmParamSet");
	temParser.SetAttrNode("STD",STD,FreqNode);
	temParser.SetAttrNode("Freq",Freq,FreqNode);
	temParser.SetAttrNode("SymbolRate",SymbolRate,FreqNode);
	int count=parser.GetChildCount(orgRootNode);
	pXMLNODELIST HoldList=parser.GetNodeList(orgRootNode);
	for(int n=0;n<count;n++)
	{
		pXMLNODE orgSetNode =parser.GetNextNode(HoldList);
		pXMLNODE stdSetNode =temParser.GetNodeFirstChild(stdRootNode);
		string SetName=parser.GetNodeName(orgSetNode);
		pXMLNODELIST SetList=parser.GetNodeList(orgSetNode);

		parser.GetAttrNode(orgSetNode,"Freq",Freq);
		for(int i=0;i<SetList->Size();i++)
		{
			string Type,Desc,Switch,Duration,ServiceID,VideoPID,AudioPID;
			pXMLNODE HoldNode = parser.GetNextNode(SetList);
			if(parser.GetNodeName(HoldNode)=="AlarmProgram")
			{
				pXMLNODELIST childSetList=parser.GetNodeList(HoldNode);
				parser.GetAttrNode(HoldNode,"ServiceID",ServiceID);
				parser.GetAttrNode(HoldNode,"VideoPID",VideoPID);
				parser.GetAttrNode(HoldNode,"AudioPID",AudioPID);
				string Tsid,Netid,Channelid;

				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,Netid);
				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,Tsid);
				CHANNELINFOMGR::instance()->GetChannelID(THREED,Netid,Tsid,ServiceID,VideoPID,AudioPID,"",Channelid);
				for(int k=0;k<childSetList->Size();k++)
				{
					pXMLNODE childHoldNode = parser.GetNextNode(childSetList);
					pXMLNODE stdHoldNode = temParser.CreateNodePtr(FreqNode,"AlarmParam");
					
					temParser.SetAttrNode("Freq",Freq,stdHoldNode);
					temParser.SetAttrNode("ChannelID",Channelid,stdHoldNode);

					parser.GetAttrNode(childHoldNode,string("Type"),Type);//获得Type属性
					parser.GetAttrNode(childHoldNode,string("Desc"),Desc);//获得Desc属性
					temParser.SetAttrNode("Type",Type,stdHoldNode);
					temParser.SetAttrNode("Desc",Desc,stdHoldNode);
					string Num,TimeInterval;
					parser.GetAttrNode(childHoldNode,string("Num"),Num);//错误次数
					parser.GetAttrNode(childHoldNode,string("TimeInterval"),TimeInterval);//时间间隔

					temParser.SetAttrNode("Num",Num,stdHoldNode);
					temParser.SetAttrNode("TimeInterval",TimeInterval,stdHoldNode);
					string DownThreshold,UpThreshold;

					parser.GetAttrNode(childHoldNode,string("DownThreshold"),DownThreshold);//下限
					parser.GetAttrNode(childHoldNode,string("UpThreshold"),UpThreshold);//上限

					temParser.SetAttrNode("DownThreshold",DownThreshold,stdHoldNode);
					temParser.SetAttrNode("UpThreshold",UpThreshold,stdHoldNode);

					parser.GetAttrNode(childHoldNode,string("Duration"),Duration);
					temParser.SetAttrNode("Duration",Duration,stdHoldNode);
					parser.GetAttrNode(childHoldNode,string("Switch"),Switch);
					temParser.SetAttrNode("Switch",Switch,stdHoldNode);
				}
			}
			else
			{
				pXMLNODE stdHoldNode = temParser.CreateNodePtr(FreqNode,"AlarmParam");
				parser.GetAttrNode(HoldNode,string("Switch"),Switch);
				if(Freq.length()<1||Freq=="All")
				{
					Freq="All";
					Switch="1";
				}
				temParser.SetAttrNode("Freq",Freq,stdHoldNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdHoldNode);
				temParser.SetAttrNode("VideoPID",VideoPID,stdHoldNode);
				temParser.SetAttrNode("AudioPID",AudioPID,stdHoldNode);
				parser.GetAttrNode(HoldNode,string("Type"),Type);//获得Type属性
				parser.GetAttrNode(HoldNode,string("Desc"),Desc);//获得Desc属性
				temParser.SetAttrNode("Type",Type,stdHoldNode);
				temParser.SetAttrNode("Desc",Desc,stdHoldNode);
				string Num,TimeInterval;
				parser.GetAttrNode(HoldNode,string("Num"),Num);//错误次数
				parser.GetAttrNode(HoldNode,string("TimeInterval"),TimeInterval);//时间间隔

				temParser.SetAttrNode("Num",Num,stdHoldNode);
				temParser.SetAttrNode("TimeInterval",TimeInterval,stdHoldNode);
				string DownThreshold,UpThreshold;

				parser.GetAttrNode(HoldNode,string("DownThreshold"),DownThreshold);//下限
				parser.GetAttrNode(HoldNode,string("UpThreshold"),UpThreshold);//上限

				temParser.SetAttrNode("DownThreshold",DownThreshold,stdHoldNode);
				temParser.SetAttrNode("UpThreshold",UpThreshold,stdHoldNode);

				parser.GetAttrNode(HoldNode,string("Duration"),Duration);
				temParser.SetAttrNode("Duration",Duration,stdHoldNode);

				
				temParser.SetAttrNode("Switch",Switch,stdHoldNode);
			}

		}
	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateAlarmReportSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="AlarmTypeSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	temParser.CreateNode("Msg", "AlarmTypeSet");
	pXMLNODE alarmTypeSetNode = parser.GetNodeFromPath("Msg/AlarmTypeSet");

	string TypeSetType, TypeSetURL, TypeSetTimeInterval;
	parser.GetAttrNode(alarmTypeSetNode, "Type", TypeSetType);
	parser.GetAttrNode(alarmTypeSetNode, "URL", TypeSetURL);
	parser.GetAttrNode(alarmTypeSetNode, "TimeInterval", TypeSetTimeInterval);

	pXMLNODE tempTypeSetNode = temParser.GetNodeFromPath("Msg/AlarmTypeSet");
	temParser.SetAttrNode("Type", TypeSetType, tempTypeSetNode);
	temParser.SetAttrNode("URL", TypeSetURL, tempTypeSetNode);
	temParser.SetAttrNode("TimeInterval", TypeSetTimeInterval, tempTypeSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
		size_t pos1=strStandardXML.find("Type=");
		strStandardXML.replace(pos1,4,"DVBType");
		size_t firPos=strStandardXML.find("\"",pos1);
		strStandardXML.replace(firPos+1,7,"THREED");
		size_t secPos=strStandardXML.find(" ",firPos+1);
		strStandardXML.insert(secPos+1,std::string("TaskType=\"AlarmTypeSet\" "));
	
	return true;
	*/
}

bool TranslateDownXMLForTHREED::TranslateAlarmQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmQuerySet></AlarmQuerySet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="AlarmQuerySet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE orgchildNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdchildNode=temParser.GetNodeFromPath("Msg/AlarmQuerySet");
	string Index,Freq, ServiceID,VideoPID, AudioPID;
	parser.GetAttrNode(orgchildNode,"Index",Index);
	parser.GetAttrNode(orgchildNode,"Freq",Freq);
	parser.GetAttrNode(orgchildNode,"ServiceID",ServiceID);
	parser.GetAttrNode(orgchildNode,"VideoPID",VideoPID);
	parser.GetAttrNode(orgchildNode,"AudioPID",AudioPID);

	temParser.SetAttrNode("Index",Index,stdchildNode);
	temParser.SetAttrNode("Freq",Freq,stdchildNode);
	temParser.SetAttrNode("ServiceID",ServiceID,stdchildNode);
	temParser.SetAttrNode("VideoPID",VideoPID,stdchildNode);
	temParser.SetAttrNode("AudioPID",AudioPID,stdchildNode);
	string AlarmType=parser.GetNodeName(orgchildNode);//三种报警类型  频率 节目 环境
	temParser.SetAttrNode("AlarmType",AlarmType,stdchildNode);
	pXMLNODELIST orgList=parser.GetNodeList(orgchildNode);
	int count=parser.GetChildCount(orgchildNode);
	for(int i=0;i<count;i++)
	{   
		string Type,Desc;
		pXMLNODE orgNode=parser.GetNextNode(orgList);
		pXMLNODE stdNode=temParser.CreateNodePtr(stdchildNode,"AlarmQuery");
		parser.GetAttrNode(orgNode,"Type",Type);
		parser.GetAttrNode(orgNode,"Desc",Desc);

		temParser.SetAttrNode("Type",Type,stdNode);
		temParser.SetAttrNode("Desc",Desc,stdNode);
	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateOSDSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="OSDSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string index, infoOSD, infoOSDx, infoOSDy, timeOSDType, timeOSDx, timeOSDy;
	pXMLNODE OSDSetNode = parser.GetNodeFromPath("Msg/OSDSet");
	parser.GetAttrNode(OSDSetNode, "Index", index);
	parser.GetAttrNode(OSDSetNode, "InfoOSD", infoOSD);
	parser.GetAttrNode(OSDSetNode, "InfoOSDX", infoOSDx);
	parser.GetAttrNode(OSDSetNode, "InfoOSDY", infoOSDy);
	parser.GetAttrNode(OSDSetNode, "TimeOSDType", timeOSDType);
	parser.GetAttrNode(OSDSetNode, "TimeOSDX", timeOSDx);
	parser.GetAttrNode(OSDSetNode, "TimeOSDY", timeOSDy);

	temParser.CreateNode("Msg", "OSDSet");
	pXMLNODE tempOSDSetNode = temParser.GetNodeFromPath("Msg/OSDSet");
	temParser.SetAttrNode("Index", index, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSD", infoOSD, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSDX", infoOSDx, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSDY", infoOSDy, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDType", timeOSDType, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDX", timeOSDx, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDY", timeOSDy, tempOSDSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateStreamRealtimeRate(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="NVRSteamRateSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string index, width, height, fps, bps;
	pXMLNODE rateSetNode = parser.GetNodeFromPath("Msg/NVRSteamRateSet");
	parser.GetAttrNode(rateSetNode, "Index", index);
	parser.GetAttrNode(rateSetNode, "Width", width);
	parser.GetAttrNode(rateSetNode, "Height", height);
	parser.GetAttrNode(rateSetNode, "Fps", fps);
	parser.GetAttrNode(rateSetNode, "Bps", bps);

	temParser.CreateNode("Msg", "NVRSteamRateSet");
	pXMLNODE tempRateSetNode = temParser.GetNodeFromPath("Msg/NVRSteamRateSet");
	temParser.SetAttrNode("Index", index, tempRateSetNode);
	temParser.SetAttrNode("Width", width, tempRateSetNode);
	temParser.SetAttrNode("Height", height, tempRateSetNode);
	temParser.SetAttrNode("Fps", fps, tempRateSetNode);
	temParser.SetAttrNode("Bps", bps, tempRateSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateRecordCapabilityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="RecordCapabilityQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	temParser.CreateNode("Msg", "RecordCapabilityQuery");
	pXMLNODE tempCapabilityNode = temParser.GetNodeFromPath("Msg/RecordCapabilityQuery");

	string freq, serviceId, programId;
	pXMLNODE CapabilityNode = parser.GetNodeFromPath("Msg/RecordCapabilityQuery");
	pXMLNODELIST channelInfolist = parser.GetNodeList(CapabilityNode);
	int channelInfoCount = 0;
	channelInfoCount = parser.GetChildCount(CapabilityNode);
	for (int i=0; i!=channelInfoCount; i++)
	{
		pXMLNODE channelInfoNode = parser.GetNextNode(channelInfolist);
		parser.GetAttrNode(channelInfoNode, "Freq", freq);
		parser.GetAttrNode(channelInfoNode, "ServicID", serviceId);
		parser.GetAttrNode(channelInfoNode, "ProgramID", programId);
		
		pXMLNODE tempChanInfoNode = temParser.CreateNodePtr(tempCapabilityNode, "ChannelInfo");
		temParser.SetAttrNode("Freq", freq, tempChanInfoNode);
		temParser.SetAttrNode("ServicID", serviceId, tempChanInfoNode);
		temParser.SetAttrNode("ProgramID", programId, tempChanInfoNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"RecordCapabilityQuery\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateHDDefAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="HDDefAudioParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string right, left, leftSurround, rightSurround, center, subWoofer, audioEncode;
	pXMLNODE HDAudioParamSetNode = parser.GetNodeFromPath("Msg/HDDefAudioParamSet");
	parser.GetAttrNode(HDAudioParamSetNode, "Right", right);
	parser.GetAttrNode(HDAudioParamSetNode, "Left", left);
	parser.GetAttrNode(HDAudioParamSetNode, "LeftSurround", leftSurround);
	parser.GetAttrNode(HDAudioParamSetNode, "RightSurround", rightSurround);
	parser.GetAttrNode(HDAudioParamSetNode, "Center", center);
	parser.GetAttrNode(HDAudioParamSetNode, "Subwoofer", subWoofer);
	parser.GetAttrNode(HDAudioParamSetNode, "AudioEncode", audioEncode);

	temParser.CreateNode("Msg", "HDDefAudioParamSet");
	pXMLNODE tempHDAParamSetNode = temParser.GetNodeFromPath("Msg/HDDefAudioParamSet");
	temParser.SetAttrNode("Right", right, tempHDAParamSetNode);
	temParser.SetAttrNode("Left", left, tempHDAParamSetNode);
	temParser.SetAttrNode("LeftSurround", leftSurround, tempHDAParamSetNode);
	temParser.SetAttrNode("RightSurround", rightSurround, tempHDAParamSetNode);
	temParser.SetAttrNode("Center", center, tempHDAParamSetNode);
	temParser.SetAttrNode("Subwoofer", subWoofer, tempHDAParamSetNode);
	temParser.SetAttrNode("AudioEncode", audioEncode, tempHDAParamSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"HDDefAudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="AudioParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string viderPid, audioPid, freq, right, left, leftSurround, rightSurround, center, subWoofer, serviceId;
	pXMLNODE audioParamSetNode = parser.GetNodeFromPath("Msg/AudioParamSet");
	parser.GetAttrNode(audioParamSetNode, "VideoPID", viderPid);
	parser.GetAttrNode(audioParamSetNode, "AudioPID", audioPid);
	parser.GetAttrNode(audioParamSetNode, "Freq", freq);
	parser.GetAttrNode(audioParamSetNode, "Right", right);
	parser.GetAttrNode(audioParamSetNode, "Left", left);
	parser.GetAttrNode(audioParamSetNode, "LeftSurround", leftSurround);
	parser.GetAttrNode(audioParamSetNode, "RightSurround", rightSurround);
	parser.GetAttrNode(audioParamSetNode, "Center", center);
	parser.GetAttrNode(audioParamSetNode, "Subwoofer", subWoofer);
	parser.GetAttrNode(audioParamSetNode, "ServiceID", serviceId);

	temParser.CreateNode("Msg", "AudioParamSet");
	pXMLNODE tempAParamSetNode = temParser.GetNodeFromPath("Msg/AudioParamSet");
	temParser.SetAttrNode("VideoPID", viderPid,tempAParamSetNode);
	temParser.SetAttrNode("AudioPID", audioPid,tempAParamSetNode);
	temParser.SetAttrNode("Freq", freq,tempAParamSetNode);
	temParser.SetAttrNode("Right", right, tempAParamSetNode);
	temParser.SetAttrNode("Left", left, tempAParamSetNode);
	temParser.SetAttrNode("LeftSurround", leftSurround, tempAParamSetNode);
	temParser.SetAttrNode("RightSurround", rightSurround, tempAParamSetNode);
	temParser.SetAttrNode("Center", center, tempAParamSetNode);
	temParser.SetAttrNode("Subwoofer", subWoofer, tempAParamSetNode);
	temParser.SetAttrNode("ServiceID", serviceId, tempAParamSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"AudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateClearAlarmState(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ClearAlarmState";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	// <ClearAlarmState> 
	//   <Target Freq="482000"/>
	// </ClearAlarmState>

	temParser.CreateNode("Msg", "ClearAlarmState");
	pXMLNODE tempClearAlarmStateNode = temParser.GetNodeFromPath("Msg/ClearAlarmState");

	string freq;
	pXMLNODE cleanAlarmStatNode = parser.GetNodeFromPath("Msg/ClearAlarmState");
	pXMLNODELIST cleanAlarmStatList = parser.GetNodeList(cleanAlarmStatNode);
	int targetCount = parser.GetChildCount(cleanAlarmStatNode);
	for (int i=0; i!=targetCount; i++)
	{
		pXMLNODE targetNode = parser.GetNextNode(cleanAlarmStatList);
		parser.GetAttrNode(targetNode, "Freq", freq);

		pXMLNODE tempTargerNode = temParser.CreateNodePtr(tempClearAlarmStateNode, "Target");
		temParser.SetAttrNode("Freq", freq, tempTargerNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateMatrixQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="MatrixQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	//<MatrixQuery >		
	//  <Matrix Type="Set" Index1="1" Index2="14"/>
	//	<Matrix Type="Set" Index1="4" Index2="5"/>
	//</MatrixQuery>

	temParser.CreateNode("Msg", "MatrixQuery");
	pXMLNODE tempMatrixQueryNode = temParser.GetNodeFromPath("Msg/MatrixQuery");

	string matrixType, index1, index2;
	pXMLNODE MatrixQueryNode = parser.GetNodeFromPath("Msg/MatrixQuery");
	pXMLNODELIST MatrixQueryList = parser.GetNodeList(MatrixQueryNode);
	int matrixCount = parser.GetChildCount(MatrixQueryNode);
	for (int i=0; i!=matrixCount; i++)
	{
		pXMLNODE matrixNode = parser.GetNextNode(MatrixQueryList);
		parser.GetAttrNode(matrixNode, "type", matrixType);
		parser.GetAttrNode(matrixNode, "Index1", index1);
		parser.GetAttrNode(matrixNode, "Index2", index2);

		pXMLNODE tempMatrixNode = temParser.CreateNodePtr(tempMatrixQueryNode, "Matrix");
		temParser.SetAttrNode("Type", matrixType, tempMatrixNode);
		temParser.SetAttrNode("Index1", index1, tempMatrixNode);
		temParser.SetAttrNode("Index2", index2, tempMatrixNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateChangeQAMQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="ChangeQAMQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	// <ChangeQAMQuery>
	//		<ChangeQAM Index="1" QAM="16" SymbolRate="1" />
	//		<ChangeQAM Index="5" QAM="128" SymbolRate="4" />
	// </ChangeQAMQuery>

	temParser.CreateNode("Msg", "ChangeQAMQuery");
	pXMLNODE tempChangeQAMNode = temParser.GetNodeFromPath("Msg/ChangeQAMQuery");

	string changeIndex, qam, symbolRate;
	pXMLNODE ChangeQAMNode = parser.GetNodeFromPath("Msg/ChangeQAMQuery");
	pXMLNODELIST ChangeQAMList = parser.GetNodeList(ChangeQAMNode);
	int changeCount = parser.GetChildCount(ChangeQAMNode);
	for (int i=0; i!=changeCount; i++)
	{
		pXMLNODE changeNode = parser.GetNextNode(ChangeQAMList);
		parser.GetAttrNode(changeNode, "Index", changeIndex);
		parser.GetAttrNode(changeNode, "QAM", qam);
		parser.GetAttrNode(changeNode, "SymbolRate", symbolRate);

		pXMLNODE tempChangeNode = temParser.CreateNodePtr(tempChangeQAMNode, "ChangeQAM");
		temParser.SetAttrNode("Index", changeIndex, tempChangeNode);
		temParser.SetAttrNode("QAM", qam, tempChangeNode);
		temParser.SetAttrNode("SymbolRate", symbolRate, tempChangeNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForTHREED::TranslateLoopAlarmInfo(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="LoopAlaInf";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	//	<LoopAlaInf>
	//		<RoundStream Index="1" Switch="1" DelayTime="15" AlaType="1">
	//			<Channel Freq="530000" SymbolRate="6875" QAM="QAM64"/>
	//			<Channel Freq="538000" SymbolRate="6875" QAM="QAM64"/>
	//			<!--SymbolRate="" QAM=""表示默认值-->   	
	//		</RoundStream>
	//		<RoundStream Index="1" Switch="1" DelayTime="15" AlaType="1">
	//			<Channel Freq="520000" SymbolRate="6875" QAM="QAM64"/>
	//			<Channel Freq="528000" SymbolRate="6875" QAM="QAM64"/>
	//			<!--SymbolRate="" QAM=""表示默认值-->   	
	//		</RoundStream>
	//	</LoopAlaInf>

	string streamIndex, streamSwitch, delayTime, alarmType;
	string freq, symbolRate, qam;
	pXMLNODE tempLoopNode = temParser.CreateNodePtr(stdRootNode, "LoopAlaInf");
	pXMLNODE loopNode = parser.GetNodeFromPath("Msg/LoopAlaInf");
	pXMLNODELIST roundStreamList = parser.GetNodeList(loopNode);
	int streamCount = parser.GetChildCount(loopNode);
	for (int n=0; n!=streamCount; n++)
	{
		pXMLNODE RoundStreamNode = parser.GetNextNode(roundStreamList);
		parser.GetAttrNode(RoundStreamNode, "Index", streamIndex);
		parser.GetAttrNode(RoundStreamNode, "Switch", streamSwitch);
		parser.GetAttrNode(RoundStreamNode, "DelayTime", delayTime);
		parser.GetAttrNode(RoundStreamNode, "AlaType", alarmType);

		pXMLNODE tempRoundStreamNode = temParser.CreateNodePtr(tempLoopNode, "RoundStream");
		temParser.SetAttrNode("Index", streamIndex, tempRoundStreamNode);
		temParser.SetAttrNode("Switch", streamSwitch, tempRoundStreamNode);
		temParser.SetAttrNode("DelayTime", delayTime, tempRoundStreamNode);
		temParser.SetAttrNode("AlaType", alarmType, tempRoundStreamNode);

		pXMLNODELIST channelList = parser.GetNodeList(RoundStreamNode);
		int channelCount = parser.GetChildCount(RoundStreamNode);
		for (int i=0; i!=channelCount; i++)
		{
			pXMLNODE channelNode = parser.GetNextNode(channelList);
			parser.GetAttrNode(channelNode, "Freq", freq);
			parser.GetAttrNode(channelNode, "SymbolRate", symbolRate);
			parser.GetAttrNode(channelNode, "QAM", qam);

			pXMLNODE tempChannelNode = temParser.CreateNodePtr(tempRoundStreamNode, "Channel");
			temParser.SetAttrNode("Freq", freq, tempChannelNode);
			temParser.SetAttrNode("SymbolRate", symbolRate, tempChannelNode);
			temParser.SetAttrNode("QAM", qam, tempChannelNode);
		}
	}

	temParser.SaveToString(strStandardXML);
	return true;
}



bool TranslateDownXMLForTHREED::TranslateRecordParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="RecordParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	//  <RecordParamSet>
	//   <RecordParam Type="HD" Width="1" Height="2" Fps="3" Bps="4" />
	//	 <RecordParam Type="SD" Width="5" Height="6" Fps="7" Bps="8" />
	//	</RecordParamSet>


	temParser.CreateNode("Msg", "RecordParamSet");
	pXMLNODE tempRecordParamSetNode = temParser.GetNodeFromPath("Msg/RecordParamSet");

	string paramType, width, height, fps, bps;
	pXMLNODE RecordParamSetNode = parser.GetNodeFromPath("Msg/RecordParamSet");
	pXMLNODELIST RecordParamList = parser.GetNodeList(RecordParamSetNode);
	int RecordParamCount = parser.GetChildCount(RecordParamSetNode);
	for (int i=0; i!=RecordParamCount; i++)
	{
		pXMLNODE paramNode = parser.GetNextNode(RecordParamList);
		parser.GetAttrNode(paramNode, "Type", paramType);
		parser.GetAttrNode(paramNode, "Width", width);
		parser.GetAttrNode(paramNode, "Height", height);
		parser.GetAttrNode(paramNode, "Fps", fps);
		parser.GetAttrNode(paramNode, "Bps", bps);

		pXMLNODE tempParamNode = temParser.CreateNodePtr(tempRecordParamSetNode, "RecordParam");
		temParser.SetAttrNode("Type", paramType, tempParamNode);
		temParser.SetAttrNode("Width", width, tempParamNode);
		temParser.SetAttrNode("Height", height, tempParamNode);
		temParser.SetAttrNode("Fps", fps, tempParamNode);
		temParser.SetAttrNode("Bps", bps, tempParamNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

// <?xml version="1.0" encoding="GB2312" standalone="yes"?>
// <Msg Version="1" MsgID="2" Type="RadioDown" DateTime="2006-08-17 15:30:00" SrcCode="110000X01" 
// DstCode="110000R01" Priority="1">
// <VideoClientStop EquCode="110000N00001" URL=""/>
// </Msg>


bool TranslateDownXMLForTHREED::TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser retparser(source);

	XmlParser parser(strOriginalXML.c_str());
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();					//获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //获取Msg节点的protocol属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值

	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //设置Msg节点的Version属性值
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //设置Msg节点的MsgID属性值
	retparser.SetAttrNode("DVBType",string("THREED"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("StopClient"),retRootNode);			//设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,URL;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"URl",URL);

	pXMLNODE retNode =retparser.CreateNodePtr(retRootNode,"StopClient");

	retparser.SetAttrNode("EquCode",EquCode,retNode);
	retparser.SetAttrNode("URL",URL,retNode);

	retparser.SaveToString(strStandardXML);
	return true;
}