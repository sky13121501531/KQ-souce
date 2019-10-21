#include "TranslateDownXMLForCTTB.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"	//wz_110319

//返回标准的频道扫描任务XML
bool TranslateDownXMLForCTTB::TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	if(strOriginalXML.empty())
		return "";
	//频道扫描标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery></ChannelScanQuery></Msg>";
	XmlParser orgParser( strOriginalXML.c_str());
	XmlParser retParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	//获取根元素
	pXMLNODE rootNode= orgParser.GetNodeFromPath( "Msg" );
	orgParser.GetAttrNode( rootNode,"Type",downType );
	orgParser.GetAttrNode( rootNode,"MsgID",msgid );
	orgParser.GetAttrNode( rootNode,"Version",version );
	orgParser.GetAttrNode( rootNode,"Protocol",protocol );
	orgParser.GetAttrNode( rootNode,"SrcCode",srccode );
	orgParser.GetAttrNode( rootNode,"DstCode",dstcode );
	orgParser.GetAttrNode( rootNode,"DateTime",dateTime );
	orgParser.GetAttrNode( rootNode,"Priority",priority );
	orgParser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
	}
	else 
	{
		return false;
	}
	//获取根元素
	pXMLNODE childNode = NULL;
	
	childNode=orgParser.GetNodeFirstChild(rootNode);
	
	pXMLNODE retRootNode = retParser.GetNodeFromPath( "Msg" );
	
	retParser.SetAttrNode( "Version",version,retRootNode );
	retParser.SetAttrNode( "MsgID",msgid,retRootNode );
	retParser.SetAttrNode( "DVBType",downType,retRootNode );
	retParser.SetAttrNode( "TaskType",string("ChannelScanQuery"),retRootNode );
	retParser.SetAttrNode( "Protocol",protocol,retRootNode );
	retParser.SetAttrNode( "DateTime",dateTime,retRootNode );
	retParser.SetAttrNode( "SrcCode",srccode,retRootNode );
	retParser.SetAttrNode( "DstCode",dstcode,retRootNode );
	retParser.SetAttrNode( "SrcURL",srcurl,retRootNode );
	retParser.SetAttrNode( "Priority",priority,retRootNode );

	//获取子元素个数
	int count = orgParser.GetChildCount( childNode );
	//获取元素列表
	pXMLNODELIST nodeList = orgParser.GetNodeList( childNode );
	pXMLNODE node = NULL; //列表元素
	for ( int i = 0; i< count; i++ )
	{ 
		//初始化属性值
		string ScanTime, STD, Freq, SymbolRate, StartFreq, EndFreq, StepFreq;
		try
		{	
			//获取频道扫描各属性
			node=orgParser.GetNextNode(nodeList);
			orgParser.GetAttrNode( node,"ScanTime",ScanTime );
			orgParser.GetAttrNode( node,"STD",STD );
			orgParser.GetAttrNode( node,"Freq",Freq );
			orgParser.GetAttrNode( node,"SymbolRate",SymbolRate );
			//设置频道扫描属性
			pXMLNODE chScanNode=retParser.CreateNodePtr(retParser.GetNodeFromPath("Msg/ChannelScanQuery"),"ChannelScan");
			retParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
			retParser.SetAttrNode( "STD",STD,chScanNode );
			retParser.SetAttrNode( "Freq",Freq,chScanNode );
			retParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
			retParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
			retParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
			retParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		}
		catch(...)
		{
			string error = "设置标准XML文档元素的属性过程失败";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
			return false;
		}	
	}
	retParser.SaveToString(strStandardXML);//保存到字符串
	return true;
}
//返回标准的实时视频任务XML
bool TranslateDownXMLForCTTB::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
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
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
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
	string width, height,fps,bps;
	string dataType;

	//获取子属性
	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	pXMLNODE realTimeNode = tempParser.GetNodeFirstChild( tempChildNode );


	//获取视频查询各属性
	parser.GetAttrNode( realStreamNode,"OrgNetID",OrgNetID );
	parser.GetAttrNode( realStreamNode,"TsID",tsID );
	parser.GetAttrNode( realStreamNode,"ServiceID",serviceID );
	parser.GetAttrNode( realStreamNode,"Width",width);
	parser.GetAttrNode( realStreamNode,"Height",height);
	parser.GetAttrNode(realStreamNode,"Bps",bps);
	parser.GetAttrNode(realStreamNode,"Fps",fps);

	//获取VideoID、 AudioID、 Freq
	CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,tsID,serviceID,audioPID );
	CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,tsID,serviceID,videoPID );
	CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,tsID,serviceID,freq );

	tempParser.SetAttrNode( "DeviceID",deviceID,realTimeNode );
	tempParser.SetAttrNode( "Freq",freq,realTimeNode );
	tempParser.SetAttrNode( "OrgNetID",OrgNetID,realTimeNode );
	tempParser.SetAttrNode( "TsID",tsID,realTimeNode );
	tempParser.SetAttrNode( "ServiceID",serviceID,realTimeNode );
	tempParser.SetAttrNode( "VideoPID",videoPID,realTimeNode );
	tempParser.SetAttrNode( "AudioPID",audioPID,realTimeNode );
	tempParser.SetAttrNode( "DataType",dataType,realTimeNode );
	tempParser.SetAttrNode( "Width",width,realTimeNode );
	tempParser.SetAttrNode( "Height",height,realTimeNode );
	tempParser.SetAttrNode( "Fps",fps,realTimeNode );
	tempParser.SetAttrNode( "Bps",bps,realTimeNode );

	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的视频轮播任务XML
bool TranslateDownXMLForCTTB::TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi)
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
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
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
	string width, height,fps,bps;
	string dataType;

	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundQuery/RoundStream");
	parser.GetAttrNode(childNode,"RoundTime",RoundTime);

	pXMLNODE tempNode=tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	tempParser.SetAttrNode("DeviceID",deviceID,tempNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempNode);

	pXMLNODELIST roundStreamList=parser.GetNodeList(childNode);
	for(int i=0;i<roundStreamList->Size();++i)
	{
		pXMLNODE tempChannelNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundChannel");
		pXMLNODE channelNode=parser.GetNextNode(roundStreamList);
		parser.GetAttrNode( channelNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( channelNode,"TsID",tsID );
		parser.GetAttrNode( channelNode,"ServiceID",serviceID );
		parser.GetAttrNode( channelNode,"Width",width);
		parser.GetAttrNode( channelNode,"Height",height);
		parser.GetAttrNode(channelNode,"Bps",bps);
		parser.GetAttrNode(channelNode,"Fps",fps);

		CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,tsID,serviceID,audioPID );
		CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,tsID,serviceID,videoPID );
		CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,tsID,serviceID,freq );


		tempParser.SetAttrNode( "Freq",freq,tempChannelNode );
		tempParser.SetAttrNode("Code",code,tempChannelNode);
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,tempChannelNode );
		tempParser.SetAttrNode( "TsID",tsID,tempChannelNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,tempChannelNode );
		tempParser.SetAttrNode( "VideoPID",videoPID,tempChannelNode );
		tempParser.SetAttrNode( "AudioPID",audioPID,tempChannelNode );
		tempParser.SetAttrNode( "DataType",dataType,tempChannelNode );
		tempParser.SetAttrNode( "Width",width,tempChannelNode );
		tempParser.SetAttrNode( "Height",height,tempChannelNode );
		tempParser.SetAttrNode( "Fps",fps,tempChannelNode );
		tempParser.SetAttrNode( "Bps",bps,tempChannelNode );

	}
	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的录像设置任务XML
bool TranslateDownXMLForCTTB::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
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
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,Index;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
				parser.GetAttrNode(childNode,"TsID",TsID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				parser.GetAttrNode(childNode,"Width",Width);
				parser.GetAttrNode(childNode,"Height",Height);
				parser.GetAttrNode(childNode,"Fps",Fps);
				parser.GetAttrNode(childNode,"Bps",Bps);

				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"CTTB");
				CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,TsID,ServiceID,Freq);
				CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioPID);
				CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoPID);
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
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

				tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
			}

		}
	}
	else if(strOriginalXML.find("TaskRecordSet")!=string::npos)
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		string Action;

		pXMLNODE TaskRecordSetNode=parser.GetNodeFromPath("Msg/TaskRecordSet");
		pXMLNODELIST TaskRecordList=parser.GetNodeList(TaskRecordSetNode);
		for (int i=0;i<TaskRecordList->Size();++i)
		{
			string Width,Height,Fps,Bps,Code,TaskID="0";
			pXMLNODE taskRecordNode=parser.GetNextNode(TaskRecordList);
			parser.GetAttrNode(taskRecordNode,"TaskID",TaskID);
			parser.GetAttrNode(taskRecordNode,"Action",Action);
			parser.GetAttrNode(taskRecordNode,"Width",Width);
			parser.GetAttrNode(taskRecordNode,"Height",Height);
			parser.GetAttrNode(taskRecordNode,"Fps",Fps);
			parser.GetAttrNode(taskRecordNode,"Bps",Bps);
			pXMLNODELIST taskRecordList=parser.GetNodeList(taskRecordNode);
			for (int k=0;k<taskRecordList->Size();++k)
			{
				string DeviceID,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Freq,DataType,Index;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(taskRecordList);
				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
				parser.GetAttrNode(childNode,"TsID",TsID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				
				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"CTTB");
				CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,TsID,ServiceID,Freq);
				CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioPID);
				CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoPID);
				parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
				if (DayofWeek.length()<1)
				{
					parser.GetAttrNode(childNode,"DayOfWeek",DayofWeek);
				}
				parser.GetAttrNode(childNode,"StartTime",StartTime);
				parser.GetAttrNode(childNode,"EndTime",EndTime);
				parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
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
bool TranslateDownXMLForCTTB::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		string Action,DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID,VideoPID,AudioPID,Freq,DataType;
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
bool TranslateDownXMLForCTTB::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//初始化属性值
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, OrgNetID, tsID, serviceID,videoID,audioID,code, StartDateTime, EndDateTime, DeviceID;

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取原始XML根元素
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //获取Msg节点的protocol属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //获取Msg节点的SrcUrl属性值	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	std::string nodeName = parser.GetNodeName( childNode );    //获取Msg的子节点名称
	char * taskType = const_cast<char *>(nodeName.c_str());
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //设置Msg节点的DVBType属性值
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
		parser.GetAttrNode( recordNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( recordNode,"TsID",tsID );
		parser.GetAttrNode( recordNode,"ServiceID",serviceID );
		parser.GetAttrNode(recordNode,"Index",DeviceID);
		if ( nodeName.compare("AutoRecordQuery") == 0)
		{
			parser.GetAttrNode( recordNode,"DeviceID",DeviceID );
		}
		if(DeviceID!="")
		PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"CTTB");	//将逻辑通道转化为实际通道
	}
	else
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );
	}

	parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );
	parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );

	//设置录像任务属性
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,recordTaskNode );
		tempParser.SetAttrNode( "TsID",tsID,recordTaskNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,recordTaskNode );
        tempParser.SetAttrNode( "VideoPID",videoID,recordTaskNode );
		tempParser.SetAttrNode( "AudioPID",audioID,recordTaskNode );
		tempParser.SetAttrNode( "Code",code,recordTaskNode );
		tempParser.SetAttrNode( "DeviceID",DeviceID,recordTaskNode );
		tempParser.SetAttrNode( "StartDateTime",StartDateTime,recordTaskNode );
		tempParser.SetAttrNode( "EndDateTime",EndDateTime,recordTaskNode );
	}
	catch(...)
	{
		string error ="设置标准XML文档元素属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}
	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的实时指标查询XML
bool TranslateDownXMLForCTTB::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	string downType;//定义检测类型

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><QualityRealtimeQuery><QualityQueryTask/><QualityParam></QualityParam></QualityRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取根元素
	pXMLNODE rootNode ;
	pXMLNODE childNode ;
	string nodeName;
	rootNode=parser.GetRootNode();
	childNode=parser.GetNodeFirstChild(rootNode);

	//初始化属性值
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,version,msgid,protocol,srccode,dstcode,srcurl;
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
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="DVBTHDown")
	{
		downType="CTTB";
	}
	else
	{
		return false;
	}
	if (protocol.length()<1)
	{
		protocol = "HTTP";
	}
	if (protocol == "HTTP")
	{
		periodicity="Once";
	}
	else if(protocol == "TCP")
	{
		periodicity="Cycle";
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
	tempParser.SetAttrNode("Priority",priority,tempRootNode );

	//获取子元素属性
	parser.GetAttrNode( childNode,"Freq",freq );
	parser.GetAttrNode( childNode,"STD",STD );
	parser.GetAttrNode( childNode,"SymbolRate",symbolRate );
	parser.GetAttrNode( childNode,"Action",action );

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
		tempParser.SetAttrNode( "SymbolRate",symbolRate,qualityTaskNode) ;
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
	tempParser.SaveToString( strStandardXML);
	return true;
}
//返回标准的定时指标设置任务xml
bool TranslateDownXMLForCTTB::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //设置Msg节点的DVBType属性值
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
	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"QualityTaskSet");  //创建QualityTaskSet节点

	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //获取原始XML中QualityReport节点的TaskID属性
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //获取原始XML中QualityReport节点的Action属性
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //设置QualityQuerySet节点的TaskID属性
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //设置QualityQuerySet节点的Action属性
	if(Action=="Del"||Action=="del")
	{
		
	}
	else if(Action=="Set"||Action=="set")
	{
		pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/Channel");      //获取原始XML中Channel节点
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/QualityParam"); //获取原始XML中QualityParam节点


		pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //创建TaskParm节点
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //创建QualityParam节点
		pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //创建TimeParam节点

		std::string STD,Freq,SymbolRate;
		pXMLNODELIST channelNodeList=parser.GetNodeList(channelNode);
		for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
		{  
			pXMLNODE channelInfo = parser.GetNextNode(channelNodeList);
			parser.GetAttrNode(channelInfo,"STD",STD);                                     //获取Channel节点的STD属性
			parser.GetAttrNode(channelInfo,"Freq",Freq);								   //获取Channel节点的Freq属性
			parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //获取Channel节点的SymbolRate属性
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodeList=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodeList);
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
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}
//返回标准的定时指标执行任务xml
bool TranslateDownXMLForCTTB::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);               //设置Msg节点的DVBType属性值
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
		pXMLNODELIST taskNodelist=parser.GetNodeList(taskNode);
		for ( int i = 0;i<parser.GetChildCount(taskNode);i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskNodelist);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //获取TaskParm节点的STD属性
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //获取TaskParm节点的Freq属性
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //获取TaskParm节点的SymbolRate属性
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodelist=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodelist);
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
bool TranslateDownXMLForCTTB::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //设置Msg节点的DVBType属性值
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
bool TranslateDownXMLForCTTB::TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);

	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("ChannelSet"),tempRootNode);   //设置Msg节点的TaskType属性值
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
bool TranslateDownXMLForCTTB::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
bool TranslateDownXMLForCTTB::TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><DeviceStatusQuery></DeviceStatusQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string index,equcode;
	string tasktype="DeviceStatusQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
		temParser.CreateNode("Msg/DeviceStatusQuery","GetDeviceIndexStatus");
	}
	//获取StatusIndex子元素列表
	pXMLNODE    StatusInfo=temParser.GetNodeFromPath("Msg/DeviceStatusQuery");

	parser.GetAttrNode(childNode,"Index",index);
	parser.GetAttrNode(childNode,"EquCode",equcode);
	temParser.SetAttrNode("Index",index,StatusInfo);
	temParser.SetAttrNode("EquCode",equcode,StatusInfo);

	pXMLNODELIST stdStatusIndexList = temParser.GetNodeList( StatusInfo );
	pXMLNODELIST orgStatusIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string deviceid;

	try
	{
		//设置返回XML的StatusIndex元素属性
		for ( int j = 0;j< count;j++ )
		{
			orglistNode = parser.GetNextNode( orgStatusIndexList );
			stdListNode = temParser.GetNextNode( stdStatusIndexList );

			parser.GetAttrNode( orglistNode,"DeviceID",deviceid );
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
//返回标准的PSI/SI查询任务xml
bool TranslateDownXMLForCTTB::TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TableQuery><Table/></TableQuery></Msg>";
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
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
	pXMLNODE orgTableNode=parser.GetNodeFirstChild(childNode);
	pXMLNODE stdTabelNode=temParser.GetNodeFromPath("Msg/TableQuery/Table");
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

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回标准的运行图设置任务xml
bool TranslateDownXMLForCTTB::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
	pXMLNODE stdRunSetNode=temParser.GetNodeFromPath("Msg/RunPlanSet");

	int count = 0;
	count=parser.GetChildCount( childNode );

	pXMLNODELIST stdChannelIndexList = temParser.GetNodeList( stdRunSetNode );
	pXMLNODELIST orgChannelIndexList = parser.GetNodeList( childNode );

	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string EquCode,OrgNetID,TsID,ServiceID,VideoID,AudioID;
	int childcount=0;
	for ( int j = 0;j< count;j++ )
	{
		orglistNode = parser.GetNextNode( orgChannelIndexList );
		childcount=parser.GetChildCount(orglistNode);
		for(int i=0;i<childcount;i++)
		{
			temParser.CreateNode("Msg/RunPlanSet","RunPlanChannel");
		}
	}
	try
	{
		//设置返回XML的ChannelIndex元素属性
		stdChannelIndexList = temParser.GetNodeList( stdRunSetNode );
		for ( int j = 0;j< count;j++ )
		{	
			orglistNode = parser.GetNextNode( orgChannelIndexList );
		
			parser.GetAttrNode(orglistNode,"EquCode",EquCode);
			parser.GetAttrNode(orglistNode,"OrgNetID",OrgNetID);
			parser.GetAttrNode(orglistNode,"TsID",TsID);
			parser.GetAttrNode(orglistNode,"ServiceID",ServiceID);

			CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoID);
			CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioID);
			childcount=parser.GetChildCount(orglistNode);

			pXMLNODE childListNode;
			pXMLNODELIST childChannelIndexList = parser.GetNodeList( orglistNode );
			for(int k=0;k<childcount;k++)
			{
				string DayOfWeek,StartTime,EndTime,ValidStartDateTime,ValidEndDateTime,StartDateTime,EndDateTime;

				if ( k == 0 )
				{
					childListNode = parser.GetFirstNode( childChannelIndexList );
					if(j==0)
					{
						stdListNode = temParser.GetFirstNode( stdChannelIndexList );
					}
					else
					{
						stdListNode = temParser.GetNextNode( stdChannelIndexList );
					}	

				}
				else
				{
					childListNode = parser.GetNextNode( childChannelIndexList );
					stdListNode = temParser.GetNextNode( stdChannelIndexList );
				}
				if(parser.GetNodeName(childListNode)=="CycleWorkTime")
				{
					parser.GetAttrNode(childListNode,"DayOfWeek",DayOfWeek);
					parser.GetAttrNode(childListNode,"StartTime",StartTime);
					parser.GetAttrNode(childListNode,"EndTime",EndTime);
					parser.GetAttrNode(childListNode,"ValidStartDateTime",ValidStartDateTime);
					parser.GetAttrNode(childListNode,"ValidEndDateTime",ValidEndDateTime);

				}
				else if(parser.GetNodeName(childListNode)=="SingleWorkTime")
				{
					parser.GetAttrNode(childListNode,"StartDateTime",StartDateTime);
					parser.GetAttrNode(childListNode,"EndDateTime",EndDateTime);
				}
				temParser.SetAttrNode("EquCode",EquCode,stdListNode);
				temParser.SetAttrNode("OrgNetID",OrgNetID,stdListNode);
				temParser.SetAttrNode("TsID",TsID,stdListNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdListNode);
                temParser.SetAttrNode("VideoPID",VideoID,stdListNode);
				temParser.SetAttrNode("AudioPID",AudioID,stdListNode);
				temParser.SetAttrNode("DayOfWeek",DayOfWeek,stdListNode);
				temParser.SetAttrNode("StartTime",StartTime,stdListNode);
				temParser.SetAttrNode("EndTime",EndTime,stdListNode);
				temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,stdListNode);
				temParser.SetAttrNode("ValidEndDateTime",ValidEndDateTime,stdListNode);
				temParser.SetAttrNode("StartDateTime",StartDateTime,stdListNode);
				temParser.SetAttrNode("EndDateTime",EndDateTime,stdListNode);
				temParser.SaveToString( strStandardXML);
			}


		}
	}
	catch(...)
	{
		string error = "设置运行图属性失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}

	//保存到字符串
	temParser.SaveToString( strStandardXML);

	return true;
}
//返回标准的手动录制任务xml
bool TranslateDownXMLForCTTB::TranslateManualRecordSet(const std::string &strOriginalXML, std::string &strStandardXML)
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
//返回标准的环境指标查询任务xml
bool TranslateDownXMLForCTTB::TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
bool TranslateDownXMLForCTTB::TranslateMHPQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
//报警参数设置	( jidushan 110321 ) 
bool TranslateDownXMLForCTTB::TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	
	string tasktype="AlarmParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE ParamNode=parser.GetNodeFirstChild(orgRootNode);
	//AlarmParamSet节点
	pXMLNODE AlarmParamSetNode=temParser.GetNodeFromPath("Msg/AlarmParamSet");
	string std,freq,symbolrate;
	//AlarmParamSet节点 建立std、freq、symbolrate属性,默认为空
	temParser.SetAttrNode("STD",		std,		AlarmParamSetNode); 
	temParser.SetAttrNode("Freq",		freq,		AlarmParamSetNode);
	temParser.SetAttrNode("SymbolRate", symbolrate, AlarmParamSetNode);

	int  count = parser.GetChildCount(ParamNode);
	pXMLNODELIST NodeList = parser.GetNodeList(ParamNode);
	for(int i=0;i<count;i++)
	{
		pXMLNODE childNode=parser.GetNextNode(NodeList);	//AlarmProgram \AlarmFreq \AlarmEnvironment节点
		string nodename = parser.GetNodeName(childNode);
		if (nodename == "AlarmProgram")		//program报警类型
		{
			//先把freq和channelID获取出来，建立子节点的时候直接赋值
			string EquCode,OrgNetID,TsID,ServiceID;
			parser.GetAttrNode(childNode,"EquCode",		EquCode);
			parser.GetAttrNode(childNode,"OrgNetID",	OrgNetID);
			parser.GetAttrNode(childNode,"TsID",		TsID);
			parser.GetAttrNode(childNode,"ServiceID",	ServiceID);

			//获取freq
			bool ret = CHANNELINFOMGR::instance()->GetFreq(OSFunction::GetEnumDVBType(downtype), OrgNetID, TsID, ServiceID, freq);
			if (freq.length()<1)
			{
				freq = "All";
			}
			//获取channelid
			string VideoPID,AudioPID,Channelid;
			CHANNELINFOMGR::instance()->GetVideoID(OSFunction::GetEnumDVBType(downtype),OrgNetID, TsID,ServiceID,VideoPID);
			CHANNELINFOMGR::instance()->GetAudioID(OSFunction::GetEnumDVBType(downtype),OrgNetID, TsID,ServiceID,AudioPID);
			CHANNELINFOMGR::instance()->GetChannelID(CTTB,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,"",Channelid);
			
			pXMLNODELIST childSetList=parser.GetNodeList(childNode);
			for(int k=0;k<childSetList->Size();k++)
			{
				pXMLNODE childHoldNode = parser.GetNextNode(childSetList);
				pXMLNODE AlarmParamNode = temParser.CreateNodePtr(AlarmParamSetNode,"AlarmParam");

				temParser.SetAttrNode("Freq",freq,AlarmParamNode);
				temParser.SetAttrNode("ChannelID",Channelid,AlarmParamNode);

				//节目报警应该有的属性
				string type,desc,duration,alarmswitch;
				parser.GetAttrNode(childHoldNode,string("Type"),type);//获得Type属性
				parser.GetAttrNode(childHoldNode,string("Desc"),desc);//获得Desc属性
				parser.GetAttrNode(childHoldNode,string("Duration"),duration);
				parser.GetAttrNode(childHoldNode,string("Switch"),alarmswitch);

				temParser.SetAttrNode("Type",type,AlarmParamNode);
				temParser.SetAttrNode("Desc",desc,AlarmParamNode);
				temParser.SetAttrNode("Duration",duration,AlarmParamNode);
				temParser.SetAttrNode("Switch",alarmswitch,AlarmParamNode);

				//节目报警应该获取不到的属性
				string num,timeinterval,downthreshold,upthreshold;
				parser.GetAttrNode(childHoldNode,string("Num"),num);//错误次数
				parser.GetAttrNode(childHoldNode,string("TimeInterval"),timeinterval);//时间间隔
				parser.GetAttrNode(childHoldNode,string("DownThreshold"),downthreshold);//下限
				parser.GetAttrNode(childHoldNode,string("UpThreshold"),upthreshold);//上限

				temParser.SetAttrNode("Num",num,AlarmParamNode);
				temParser.SetAttrNode("TimeInterval",timeinterval,AlarmParamNode);
				temParser.SetAttrNode("DownThreshold",downthreshold,AlarmParamNode);
				temParser.SetAttrNode("UpThreshold",upthreshold,AlarmParamNode);
			}
		}
		else		//freq类型或者environment类型
		{
			//freq类型alarmfreq节点自带std，freq和symbolrate属性
			//environment类型没有，默认赋值为空
			parser.GetAttrNode(childNode, "STD",		std);
			parser.GetAttrNode(childNode, "Freq",		freq);
			parser.GetAttrNode(childNode, "SymbolRate", symbolrate);
			
			temParser.SetAttrNode("STD",		std,		AlarmParamSetNode);
			temParser.SetAttrNode("Freq",		freq,		AlarmParamSetNode);
			temParser.SetAttrNode("SymbolRate", symbolrate, AlarmParamSetNode);
			
			int lastcount = parser.GetChildCount(childNode);	
			pXMLNODELIST lastNodeList = parser.GetNodeList(childNode);
			for (int j=0;j<lastcount;++j)
			{
				pXMLNODE lastNode = parser.GetNextNode(lastNodeList);
				pXMLNODE AlarmParamNode = temParser.CreateNodePtr(AlarmParamSetNode, "AlarmParam");
				if (freq.length()<1)
				{
					freq = "All";
				}
				temParser.SetAttrNode("Freq",	freq,	AlarmParamNode);

				//freq类型以及Environment类型特有属性
				string alarmSwitch,type,desc,alarmNum, TimeInterval,DownThreshold,UpThreshold;
				parser.GetAttrNode(lastNode, "Type", type);
				parser.GetAttrNode(lastNode, "Desc", desc);
				parser.GetAttrNode(lastNode, "Switch", alarmSwitch);
				if (alarmSwitch.length()<1)
				{
					alarmSwitch = "1";
				}
				parser.GetAttrNode(lastNode, "Num", alarmNum);
				parser.GetAttrNode(lastNode, "TimeInterval", TimeInterval);
				parser.GetAttrNode(lastNode, "DownThreshold", DownThreshold);
				parser.GetAttrNode(lastNode, "UpThreshold", UpThreshold);
				
				temParser.SetAttrNode("Type",			type,			AlarmParamNode);
				temParser.SetAttrNode("Desc",			desc,			AlarmParamNode);
				temParser.SetAttrNode("Num",			alarmNum,		AlarmParamNode);
				temParser.SetAttrNode("TimeInterval",	TimeInterval,	AlarmParamNode);
				temParser.SetAttrNode("DownThreshold",	DownThreshold,	AlarmParamNode);
				temParser.SetAttrNode("UpThreshold",	UpThreshold,	AlarmParamNode);
				temParser.SetAttrNode("Switch",			alarmSwitch,	AlarmParamNode);

				//其他默认为空类型
				string serviceId,videoPid,audioPid,duration;
				parser.GetAttrNode(lastNode, "ServiceID", serviceId);
				parser.GetAttrNode(lastNode, "VideoPID", videoPid);
				parser.GetAttrNode(lastNode, "AudioPID", audioPid);
				parser.GetAttrNode(lastNode, "Duration", duration);
				temParser.SetAttrNode("Duration",		duration,		AlarmParamNode);
				temParser.SetAttrNode("ServiceID",		serviceId,		AlarmParamNode);
				temParser.SetAttrNode("VideoPID",		videoPid,		AlarmParamNode);
				temParser.SetAttrNode("AudioPID",		audioPid,		AlarmParamNode);		
			}
		}
	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;

}


//返回指标补偿标准xml
bool TranslateDownXMLForCTTB::TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
//返回标准的数据业务分析时间设置任务xml
bool TranslateDownXMLForCTTB::TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

bool TranslateDownXMLForCTTB::TranslateEPGQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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


	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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
