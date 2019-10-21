///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForAM.cpp
// 创建者：gaoxd
// 创建时间：2010-04-09
// 内容描述：将下发XML转化为原子任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#include "TranslateDownXMLForAM.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"

//返回标准的实时视频任务XML
bool TranslateDownXMLForAM::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRealtimeQuery></StreamRealtimeQuery></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,SrcURL;//头信息
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"SrcURL",SrcURL);

	if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
	}
	else
	{
		return false;
	}

	//设置头信息
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("StreamRealtimeQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",SrcURL,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	pXMLNODE childNode=parser.GetNodeFirstChild(rootNode);
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( childNode );
	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/StreamRealtimeQuery");
	//初始化属性值
	string Index,EquCode,LastURL,Freq,Bps;
	try
	{	
		//获取实时音频各属性
		parser.GetAttrNode( realStreamNode,"Index",Index );
		parser.GetAttrNode( realStreamNode,"EquCode",EquCode );
		parser.GetAttrNode( realStreamNode,"LastURL",LastURL );
		parser.GetAttrNode( realStreamNode,"Freq",Freq );
		parser.GetAttrNode( realStreamNode,"Bps",Bps );
		//设置音频属性
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"RealtimeQueryTask");
		tempParser.SetAttrNode( "Index",Index,chScanNode );
		tempParser.SetAttrNode( "EquCode",EquCode,chScanNode );
		tempParser.SetAttrNode( "LastURL",LastURL,chScanNode );
		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "Bps",Bps,chScanNode );

	}
	catch(...)
	{
		string error = "设置标准XML文档元素的属性过程失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}	
	tempParser.SaveToString(strStandardXML);//保存到字符串
	return true;
}
//返回标准的录像设置任务XML
bool TranslateDownXMLForAM::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (protocol.length()<1)
	{
		protocol="HTTP";
	}
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"AMDown") )
	{
		downType = "AM";
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

	if(strOriginalXML.find("SetAutoRecordChannel")!=string::npos)	//自动录音设置
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
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,SubPriority;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				string Index,ExpireDays;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"Code",Code);
				parser.GetAttrNode(childNode,"Bps",Bps);
				parser.GetAttrNode(childNode,"Priority",SubPriority);
				parser.GetAttrNode(childNode,"ExpireDays",ExpireDays);
				
				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"AM");
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
				CHANNELINFOMGR::instance()->GetFreqByChannelCode(AM,Code,Freq);
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
				tempParser.SetAttrNode("Priority",SubPriority,recordNode);
				tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);
			}
		}
	}
	else		//任务录音设置
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		pXMLNODE taskRecord=parser.GetNodeFromPath("Msg/TaskSet/Task");
		string EquCode,TaskID,Action, ChCode,Freq, DeviceID, Width, Height, Fps, Bps,Index;
		parser.GetAttrNode(taskRecord,"EquCode",EquCode);
		parser.GetAttrNode(taskRecord,"TaskID",TaskID);
		parser.GetAttrNode(taskRecord,"Action",Action);
		parser.GetAttrNode(taskRecord,"ChannelCODE",ChCode);
		parser.GetAttrNode(taskRecord,"Freq",Freq);
		parser.GetAttrNode(taskRecord,"DeviceID",DeviceID);
		parser.GetAttrNode(taskRecord,"Width",Width);
		parser.GetAttrNode(taskRecord,"Height",Height);
		parser.GetAttrNode(taskRecord,"Fps",Fps);
		parser.GetAttrNode(taskRecord,"Bps",Bps);
		PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"AM");

		if ("Del"==Action||"del"==Action)		//录音任务删除
		{
			pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
			tempParser.SetAttrNode("TaskID",TaskID,recordNode);
			tempParser.SetAttrNode("Action",Action,recordNode);
			tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
			tempParser.SetAttrNode("Freq",Freq,recordNode);
			tempParser.SetAttrNode("Code",ChCode,recordNode);
			tempParser.SetAttrNode("OrgNetID",string(""),recordNode);
			tempParser.SetAttrNode("TsID",string(""),recordNode);
			tempParser.SetAttrNode("ServiceID",string(""),recordNode);
			tempParser.SetAttrNode("VideoPID",string(""),recordNode);
			tempParser.SetAttrNode("AudioPID",string(""),recordNode);
			tempParser.SetAttrNode("DataType",string(""),recordNode);
			tempParser.SetAttrNode("Width",Width,recordNode);
			tempParser.SetAttrNode("Height",Height,recordNode);
			tempParser.SetAttrNode("Fps",Fps,recordNode);
			tempParser.SetAttrNode("Bps",Bps,recordNode);

			tempParser.SetAttrNode("DayofWeek",string(""),recordNode);
			tempParser.SetAttrNode("StartTime",string(""),recordNode);
			tempParser.SetAttrNode("EndTime",string(""),recordNode);
			tempParser.SetAttrNode("StartDateTime",string(""),recordNode);
			tempParser.SetAttrNode("EndDateTime",string(""),recordNode);
		}
		else		//录音任务设置
		{
			pXMLNODELIST nodelist=parser.GetNodeList(taskRecord);
			for (int i=0;i!=nodelist->Size();++i)
			{
				pXMLNODE childnode=parser.GetNextNode(nodelist);
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				string   DayOfWeek, StartTime, EndTime, StartDateTime,EndDateTime, ExpireDays;
				parser.GetAttrNode(childnode,"DayOfWeek",DayOfWeek);
				parser.GetAttrNode(childnode,"StartTime",StartTime);
				parser.GetAttrNode(childnode,"EndTime",EndTime);
				parser.GetAttrNode(childnode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childnode,"EndDateTime",EndDateTime);
				parser.GetAttrNode(childnode,"ExpireDays",ExpireDays);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
				if (Freq.length()<1)
				{
					CHANNELINFOMGR::instance()->GetFreqByChannelCode(AM,ChCode,Freq);
				}

				tempParser.SetAttrNode("Freq",Freq,recordNode);
				tempParser.SetAttrNode("Code",ChCode,recordNode);
				tempParser.SetAttrNode("OrgNetID",string(""),recordNode);
				tempParser.SetAttrNode("TsID",string(""),recordNode);
				tempParser.SetAttrNode("ServiceID",string(""),recordNode);
				tempParser.SetAttrNode("VideoPID",string(""),recordNode);
				tempParser.SetAttrNode("AudioPID",string(""),recordNode);
				tempParser.SetAttrNode("DataType",string(""),recordNode);
				tempParser.SetAttrNode("Width",Width,recordNode);
				tempParser.SetAttrNode("Height",Height,recordNode);
				tempParser.SetAttrNode("Fps",Fps,recordNode);
				tempParser.SetAttrNode("Bps",Bps,recordNode);
				tempParser.SetAttrNode("DayofWeek",DayOfWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
				tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);
			}
		}
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}
//返回标准的录像执行任务XML
bool TranslateDownXMLForAM::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	//录音标准的XML结构
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
		string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime,ExpireDays;
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
		parser.GetAttrNode(childNode,"Priority",priority);
		parser.GetAttrNode(childNode,"ExpireDays",ExpireDays);

		tempParser.SetAttrNode("Priority",priority,tempRootNode);
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
		tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);

		tempParser.SaveToString(xml);
		tempParser.DeleteNode(MyRecordNode,recordNode);
		strStandardXMLVec.push_back(xml);
	}

	return true;
}
//返回标准的录像查询、下载XML
bool TranslateDownXMLForAM::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//初始化属性值
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string Freq,TaskID, OrgNetID, tsID, serviceID,videoID,audioID,code, StartDateTime, EndDateTime, DeviceID;

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

	std::string  taskType;
	if (nodeName=="StreamSimpleQuery")//不同的录像查询任务类型
	{
		taskType="AutoRecordQuery";
	}
	else if (nodeName=="TaskRecordQuery")
	{
		taskType="TaskRecordQuery";
	}
	else if(nodeName=="TaskRecordFileQuery")
	{
		taskType="TaskRecordFileQuery";
	}
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if (strType =="AMDown")
	{
		tempParser.SetAttrNode("DVBType",string("AM"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("RecordQuery"),tempRootNode);   //设置Msg节点的TaskType属性值
	if (strProtocol.length()<1)
	{
		strProtocol="HTTP";
	}
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);             //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);          //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);               //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);               //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                 //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);             //设置Msg节点的Priority属性值

	//创建查询类型节点

	pXMLNODE query= tempParser.CreateNodePtr(tempRootNode,(char*)taskType.c_str());

	pXMLNODE recordTaskNode = tempParser.CreateNodePtr(query,"RecordQueryTask"); //创建RecordQueryTask节点

	//获取录像任务各属性
	if ( (taskType.compare("AutoRecordFileQuery") == 0)||(taskType.compare("AutoRecordQuery") == 0) )
	{
		parser.GetAttrNode( recordNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( recordNode,"TsID",tsID );
		parser.GetAttrNode( recordNode,"ServiceID",serviceID );
		if ( nodeName.compare("AutoRecordQuery") == 0)
		{
			parser.GetAttrNode( recordNode,"DeviceID",DeviceID );
		}
	}
	else
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );
	}
	parser.GetAttrNode(recordNode,"ChannelCode",code);
	parser.GetAttrNode(recordNode,"ChCode",code);
	parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );
	parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );

	//设置录像任务属性
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );
		CHANNELINFOMGR::instance()->GetFreqByChannelCode(AM,code,Freq);
		//应该通过ChCode获得相应的Freq
		tempParser.SetAttrNode( "Freq",Freq,recordTaskNode);
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
//返回标准的频谱扫描XML
bool TranslateDownXMLForAM::TranslateSpectrumScanQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,strSrcURL;//头信息
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);

	if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
	}
	else
	{
		return false;
	}

	//设置头信息
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("SpectrumScanQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",strSrcURL,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( rootNode );
	//初始化属性值
	string EquCode,StartFreq,EndFreq,StepFreq,Steps,Action;
	try
	{	
		//获取频谱扫描各属性
		parser.GetAttrNode( realStreamNode,"EquCode",EquCode );
		parser.GetAttrNode( realStreamNode,"StartFreq",StartFreq);
		parser.GetAttrNode( realStreamNode,"EndFreq",EndFreq );
		parser.GetAttrNode( realStreamNode,"StepFreq",StepFreq );
		parser.GetAttrNode( realStreamNode,"Steps",Steps);
		parser.GetAttrNode( realStreamNode,"Action",Action );

		//设置频谱扫描属性
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempRootNode,"SpectrumScanQuery");
		tempParser.SetAttrNode( "EquCode",EquCode,chScanNode );
		tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
		tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
		tempParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		tempParser.SetAttrNode( "Steps",Steps,chScanNode );
		tempParser.SetAttrNode("Action",Action,chScanNode);

	}
	catch(...)
	{
		string error = "设置标准XML文档元素的属性过程失败";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}	
	tempParser.SaveToString(strStandardXML);//保存到字符串
	return true;
}
//返回标准的实时指标查询XML
bool TranslateDownXMLForAM::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
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
	string periodicity="Cycle";
	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="AMDown")
	{
		downType="AM";
	}
	else
	{
		return false;
	}
	if(protocol.length()<1)
		protocol="HTTP";
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

bool TranslateDownXMLForAM::TranslateChannelScanQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	string downType;//定义检测类型

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery/></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取根元素
	pXMLNODE rootNode ;
	pXMLNODE childNode ;
	string nodeName;
	rootNode=parser.GetRootNode();
	childNode=parser.GetNodeFromPath("Msg/ChannelScanQuery/ChannelScan");

	//初始化属性值
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,version,msgid,protocol,srccode,dstcode,srcurl;
	string StartFreq,EndFreq,StepFreq;
	string Type,Desc,Value;
	int count = 0;
	string tasktype="ChannelScanQuery";

	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFromPath( "Msg/ChannelScanQuery" );
	pXMLNODE channelscannode=tempParser.CreateNodePtr(tempChildNode,"Channel");
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="AMDown")
	{
		downType="AM";
	}
	else
	{
		return false;
	}
	if(protocol.length()<1)
		protocol="HTTP";
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

	parser.GetAttrNode(childNode,"StartFreq",StartFreq);
	parser.GetAttrNode(childNode,"EndFreq",EndFreq);
	parser.GetAttrNode(childNode,"StepFreq",StepFreq);

	tempParser.SetAttrNode("StartFreq",StartFreq,channelscannode);
	tempParser.SetAttrNode("EndFreq",EndFreq,channelscannode);
	tempParser.SetAttrNode("StepFreq",StepFreq,channelscannode);

	tempParser.SaveToString(strStandardXML);
	return true;

}

bool TranslateDownXMLForAM::TranslateStreamRound( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRoundQuery></StreamRoundQuery></Msg>";
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

	if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
	}
	else
	{
		return false;
	}

	//设置头信息
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
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
	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundQuery/RoundStream");

	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	//初始化属性值
	string Index,EquCode,LastURL,Freq,Width,Height,Fps,Bps,RoundTime,DeviceID;

	//获取实时音视频各属性
	parser.GetAttrNode( childNode,"Index",Index );
	parser.GetAttrNode( childNode,"EquCode",EquCode );
	parser.GetAttrNode( childNode,"LastURL",LastURL );
	parser.GetAttrNode( childNode,"Freq",Freq );
	parser.GetAttrNode( childNode,"Width",Width );
	parser.GetAttrNode( childNode,"RoundTime",RoundTime );
	parser.GetAttrNode( childNode,"Height",Height );
	parser.GetAttrNode( childNode,"Fps",Fps );
	parser.GetAttrNode( childNode,"Bps",Bps );
	//设置实时音视频属性
	tempParser.SetAttrNode("DeviceID",DeviceID,tempQueryNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempQueryNode);

	pXMLNODELIST nodelist=parser.GetNodeList(childNode);
	for (int k=0;k!=nodelist->Size();++k)
	{
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"RoundChannel");
		pXMLNODE child=parser.GetNextNode(nodelist);
		parser.GetAttrNode(child,"Freq",Freq);

		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "Code",string(""),chScanNode );
		tempParser.SetAttrNode( "OrgNetID",string(""),chScanNode );
		tempParser.SetAttrNode( "TsID",string(""),chScanNode );
		tempParser.SetAttrNode( "ServiceID",string(""),chScanNode );
		tempParser.SetAttrNode( "VideoPID",string(""),chScanNode );
		tempParser.SetAttrNode( "AudioPID",string(""),chScanNode );
		tempParser.SetAttrNode( "DataType",string(""),chScanNode );
		tempParser.SetAttrNode( "Width",Width,chScanNode );
		tempParser.SetAttrNode( "Height",Height,chScanNode );
		tempParser.SetAttrNode( "Fps",Fps,chScanNode );
		tempParser.SetAttrNode( "Bps",Bps,chScanNode );
	}
	tempParser.SaveToString(strStandardXML);//保存到字符串
	return true;
}

//wz_110107
bool TranslateDownXMLForAM::TranslateChannelSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,
		strSrcCode,strDstCode,strSrcURL,strPriority;

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	parser.GetAttrNode(rootNode,"Version",strVersion);         //获取Msg节点的Version属性值
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //获取Msg节点的MsgID属性值
	parser.GetAttrNode(rootNode,"Type",strType);		       //获取Msg节点的Type属性值
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //获取Msg节点的DateTime属性值码
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //获取Msg节点的StrCode属性值
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //获取Msg节点的DstCode属性值
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //获取Msg节点的Priority属性值
	parser.GetAttrNode( rootNode,"SrcURL",strSrcURL );
	
	if (strType =="AMDown")
	{
		strType = "AM";          //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}
	//设置头信息
	if(strProtocol.length()<1)
	{
		strProtocol="HTTP";
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

	std::string strCode,strFreq,strName;
	for (int i = 0;i<parser.GetChildCount(channelSetNode);i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);

		parser.GetAttrNode(channelNode,"Code",strCode);                                //获取原始XML中Channel节点的STD属性值
		parser.GetAttrNode(channelNode,"Freq",strFreq);                              //获取原始XML中Channel节点的Freq属性值
		parser.GetAttrNode(channelNode,"Name",strName);                                //获取原始XML中Channel节点的QAM属性值
	

		pXMLNODE tempChannelNode = tempParser.CreateNodePtr(tempChannelSetNode,"Channel");//创建标准XML中Channel节点
		tempParser.SetAttrNode("Code",strCode,tempChannelNode);                                //设置标准XML中Channel节点的STD属性值
		tempParser.SetAttrNode("Freq",strFreq,tempChannelNode);                              //设置标准XML中Channel节点的Freq属性值
		tempParser.SetAttrNode("Name",strName,tempChannelNode);                                //设置标准XML中Channel节点的QAM属性值
	}
	tempParser.SaveToString(strStandardXML);   //保存到字符串
	return true;
}

bool TranslateDownXMLForAM::TranslateAlarmParamSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,strSrcURL;//头信息
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",strSrcURL );

	if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
	}
	else
	{
		return false;
	}

	//设置头信息
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("AlarmParamSet"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode); 

	pXMLNODE alarmset=tempParser.GetNodeFromPath("Msg/AlarmParamSet");
	
	pXMLNODE alarmnode=parser.GetNodeFromPath("Msg/QualityAlarmParamSet");
	tempParser.SetAttrNode("STD",string(""),alarmset);
	tempParser.SetAttrNode("Freq",string(""),alarmset);
	tempParser.SetAttrNode("SymbolRate",string(""),alarmset);

	pXMLNODELIST alarmlist=parser.GetNodeList(alarmnode);
	for (int i=0;i!=alarmlist->Size();++i)
	{
		pXMLNODE childnode=parser.GetNextNode(alarmlist);
		std::string channelcode,freq;
		parser.GetAttrNode(childnode,"ChannelCode",channelcode);
		if (channelcode!="all"&&channelcode!="All")
		{
			CHANNELINFOMGR::instance()->GetFreqByChannelCode(AM,channelcode,freq);
		}
		else
		{
			freq="All";
		}
		pXMLNODELIST chilidalarmlist=parser.GetNodeList(childnode);
		for(int k=0;k!=chilidalarmlist->Size();++k)
		{
			pXMLNODE alarmchildnode=parser.GetNextNode(chilidalarmlist);
			std::string Type,Desc,Value,UpThreshold,DownThreshold,TimeThreshold,Num,Switch,AlarmThreshold;
			parser.GetAttrNode(alarmchildnode,"type",Type);
			parser.GetAttrNode(alarmchildnode,"Desc",Desc);
			parser.GetAttrNode(alarmchildnode,"TimeThreshold",TimeThreshold);
			parser.GetAttrNode(alarmchildnode,"UpThreshold",UpThreshold);
			parser.GetAttrNode(alarmchildnode,"AlarmThreshold",AlarmThreshold);
			if (StrUtil::Str2Int(Type)<10)
			{
				parser.GetAttrNode(alarmchildnode,"DownThreshold",DownThreshold);
			}
			else
			{
				parser.GetAttrNode(alarmchildnode,"DownThreshold",Num);
			}
			parser.GetAttrNode(alarmchildnode,"Switch",Switch);
			if(Switch.length()<1)
				Switch="1";
			pXMLNODE AlarmParamNode=tempParser.CreateNodePtr(alarmset,"AlarmParam");
			tempParser.SetAttrNode("Freq",freq,AlarmParamNode);
			tempParser.SetAttrNode("ChannelID",freq,AlarmParamNode);
			tempParser.SetAttrNode("Type",Type,AlarmParamNode);
			tempParser.SetAttrNode("Desc",Desc,AlarmParamNode);
			tempParser.SetAttrNode("Num",Num,AlarmParamNode);
			tempParser.SetAttrNode("Duration",TimeThreshold,AlarmParamNode);

			tempParser.SetAttrNode("AlarmThreshold",AlarmThreshold,AlarmParamNode);
			tempParser.SetAttrNode("DownThreshold",DownThreshold,AlarmParamNode);
			tempParser.SetAttrNode("UpThreshold",UpThreshold,AlarmParamNode);
			tempParser.SetAttrNode("Switch",Switch,AlarmParamNode);
		}	

	}
	tempParser.SaveToString(strStandardXML);
	return true;
}

//返回标准的定时指标设置任务xml
bool TranslateDownXMLForAM::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{  
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action,EquCode,InRunPlan,Code;

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
	if (strType =="AMDown")
	{
		tempParser.SetAttrNode("DVBType",string("AM"),tempRootNode);           //设置Msg节点的DVBType属性值
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
	pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //创建TaskParm节点
	pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //创建QualityParam节点
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //创建TimeParam节点
	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //获取原始XML中QualityReport节点的TaskID属性
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //获取原始XML中QualityReport节点的Action属性
	parser.GetAttrNode(qualityReportNode,"EquCode",EquCode);
	parser.GetAttrNode(qualityReportNode,"InRunPlan",InRunPlan);
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //设置QualityQuerySet节点的TaskID属性
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //设置QualityQuerySet节点的Action属性
	tempParser.SetAttrNode("EquCode",EquCode,querySetNode);
	tempParser.SetAttrNode("InRunPlan",InRunPlan,querySetNode);

	if(Action=="Set"||Action=="set")
	{
		pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityReportTaskSet/QualityReport/Channel");      //获取原始XML中Channel节点
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityReportTaskSet/QualityReport/QualityParam"); //获取原始XML中QualityParam节点
		pXMLNODELIST ChannelList = parser.GetNodeList(channelNode);
		std::string STD,Freq,SymbolRate;
		for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
		{  
			pXMLNODE channelInfo = parser.GetNextNode(ChannelList);
			parser.GetAttrNode(channelInfo,"STD",STD);                                     //获取Channel节点的STD属性
			parser.GetAttrNode(channelInfo,"Freq",Freq);								   //获取Channel节点的Freq属性
			parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //获取Channel节点的SymbolRate属性
			parser.GetAttrNode(channelInfo,"Code",Code);
			CHANNELINFOMGR::instance()->GetFreqByChannelCode(AM,Code,Freq);
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
			tempParser.SetAttrNode("Code",Code,taskNode);
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
bool TranslateDownXMLForAM::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		tempParser.SetAttrNode("DVBType",string("AM"),tempRootNode);               //设置Msg节点的DVBType属性值
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
		pXMLNODELIST taskNodeList=parser.GetNodeList(taskNode);
		for ( int i = 0;i<taskNodeList->Size();i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskNodeList);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //获取TaskParm节点的STD属性
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //获取TaskParm节点的Freq属性
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //获取TaskParm节点的SymbolRate属性
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //创建TaskParm节点的子节点
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //设置QualityQueryTask节点的STD属性
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //设置QualityQueryTask节点的Freq属性
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //设置QualityQueryTask节点的SymbolRate属性
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodeList=parser.GetNodeList(qualityNode);
		for (int j = 0;j<qualityNodeList->Size();j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodeList);
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

//返回标准的定时指标设置任务xml
bool TranslateDownXMLForAM::TranslateSpectrumTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{  
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action,EquCode,IsFreqScan,StartFreq,EndFreq,StepFreq,Code;

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
	if (strType =="AMDown")
	{
		tempParser.SetAttrNode("DVBType",string("AM"),tempRootNode);           //设置Msg节点的DVBType属性值
	}
	else
	{
		return false;
	}

	tempParser.SetAttrNode("TaskType",string("SpectrumTaskSet"),tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	pXMLNODE SpectrumNode = parser.GetNodeFirstChild( childNode );

	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"SpectrumTaskSet");  //创建QualityTaskSet节点
	pXMLNODE TaskNode = tempParser.CreateNodePtr(querySetNode,"TaskParam");
	pXMLNODE SpecqueryNode = tempParser.CreateNodePtr(TaskNode,"SpectrumQueryIndex");
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //创建TimeParam节点
	parser.GetAttrNode(SpectrumNode,"TaskID",TaskID);							   //获取原始XML中QualityReport节点的TaskID属性
	parser.GetAttrNode(SpectrumNode,"Action",Action);                             //获取原始XML中QualityReport节点的Action属性
	parser.GetAttrNode(SpectrumNode,"EquCode",EquCode);
	parser.GetAttrNode(SpectrumNode,"StartFreq",StartFreq);
	parser.GetAttrNode(SpectrumNode,"EndFreq",EndFreq);
	parser.GetAttrNode(SpectrumNode,"StepFreq",StepFreq);
	parser.GetAttrNode(SpectrumNode,"IsFreqScan",IsFreqScan);
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //设置QualityQuerySet节点的TaskID属性
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //设置QualityQuerySet节点的Action属性
	tempParser.SetAttrNode("EquCode",EquCode,SpecqueryNode );
	tempParser.SetAttrNode("StartFreq",StartFreq,SpecqueryNode );
	tempParser.SetAttrNode("EndFreq",EndFreq,SpecqueryNode) ;
	tempParser.SetAttrNode("StepFreq",StepFreq,SpecqueryNode );
	tempParser.SetAttrNode("IsFreqScan",IsFreqScan,SpecqueryNode );

	std::string DayOfWeek,StartTime,StartDateTime;
	pXMLNODELIST paramNodeList = parser.GetNodeList(SpectrumNode);
	for (int p = 0;p<paramNodeList->Size();p++)
	{
		pXMLNODE checkTimeNode = parser.GetNextNode(paramNodeList);
		std::string strNodeName = parser.GetNodeName(checkTimeNode);
		if (strNodeName =="CycleReportTime"||strNodeName =="SingleReportTime")
		{
			if (strNodeName =="CycleReportTime")
			{
				parser.GetAttrNode(checkTimeNode,"DayofWeek",DayOfWeek);                   //获取原始XML中CycleReportTime节点的DayOfWeek属性
				parser.GetAttrNode(checkTimeNode,"StartTime",StartTime);                   //获取原始XML中CycleReportTime节点的StartTime属性
			}
			else if (strNodeName =="SingleReportTime")
			{
				parser.GetAttrNode(checkTimeNode,"StartDateTime",StartDateTime);          //获取原始XML中SingleReportTime节点的StartDateTime属性

			}
			pXMLNODE timeIndexNode = tempParser.CreateNodePtr(timeParamNode,"TimeIndex");   //创建TimeParam节点的子节点

			tempParser.SetAttrNode("DayOfWeek",DayOfWeek,timeIndexNode);                    //设置TimeIndex节点的DayOfWeek属性
			tempParser.SetAttrNode("StartTime",StartTime,timeIndexNode);                    //设置TimeIndex节点的StartTime属性
			tempParser.SetAttrNode("StartDateTime",StartDateTime,timeIndexNode);            //设置TimeIndex节点的StartDateTime属性               //设置TimeIndex节点的EndDateTime属性

			//重新初始化指标设置中与时间相关变量
			DayOfWeek = "";
			StartTime = "";
			StartDateTime ="";

		}
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForAM::TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		std::string TaskID, Action,DevideID,EquCode,IsFreqScan,StartFreq,EndFreq,StepFreq;

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
		tempParser.SetAttrNode("DVBType",string("AM"),tempRootNode);               //设置Msg节点的DVBType属性值
		tempParser.SetAttrNode("TaskType",string("SpectrumTask"),tempRootNode);    //设置Msg节点的TaskType属性值
		tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //设置Msg节点的Protocol属性值
		tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //设置Msg节点的DateTime属性值
		tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //设置Msg节点的StrCode属性值
		tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //设置Msg节点的DstCode属性值
		tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //设置Msg节点的SrcUrl属性值
		tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //设置Msg节点的Priority属性值

		pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
		pXMLNODE SpecNode = parser.GetNodeFromPath("Msg/SpectrumTaskSet/TaskParam/SpectrumQueryIndex");      //获取原始XML中TaskParm节点

		pXMLNODE qualityTaskNode = tempParser.CreateNodePtr(tempRootNode,"SpectrumTask");  //创建QualityTask节点
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(qualityTaskNode,"TaskParam"); //创建QualityParam节点
		pXMLNODE TaskQueryNode = tempParser.CreateNodePtr(qualityParamNode,"SpectrumQueryIndex");
		parser.GetAttrNode(childNode,"TaskID",TaskID);				     			  //获取原始XML中QualityTaskSet节点的TaskID属性
		parser.GetAttrNode(childNode,"Action",Action);                                //获取原始XML中QualityTaskSet节点的Action属性
		parser.GetAttrNode(SpecNode,"EquCode",EquCode);
		parser.GetAttrNode(SpecNode,"StartFreq",StartFreq);
		parser.GetAttrNode(SpecNode,"EndFreq",EndFreq);
		parser.GetAttrNode(SpecNode,"StepFreq",StepFreq);
		parser.GetAttrNode(SpecNode,"IsFreqScan",IsFreqScan);
		tempParser.SetAttrNode("TaskID",TaskID,qualityTaskNode);                              //设置QualityTask节点的TaskID属性
		tempParser.SetAttrNode("Action",Action,qualityTaskNode);                              //设置QualityTask节点的Action属性
		tempParser.SetAttrNode("EquCode",EquCode,TaskQueryNode);  
		tempParser.SetAttrNode("StartFreq",StartFreq,TaskQueryNode);  
		tempParser.SetAttrNode("EndFreq",EndFreq,TaskQueryNode);  
		tempParser.SetAttrNode("StepFreq",StepFreq,TaskQueryNode);  
		tempParser.SetAttrNode("IsFreqScan",IsFreqScan,TaskQueryNode);  

		tempParser.SaveToString(tmpstr);

		//根据timeIndexVct中的TimeIndex节点信息生成任务
		size_t pos4=tmpstr.find("</TaskParam>");
		if (pos4>0)
		{
			std::string timeParam = "<TimeParam>"+ timeIndexVct[p]+"</TimeParam>";
			tmpstr.insert(pos4+12,timeParam);
			strStandardXMLVec.push_back(tmpstr);
		}
	}
	return true;
}

bool TranslateDownXMLForAM::TranslateSpectrumQueryTask(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //获取原始XML根元素
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("SpectrumQuery"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,TaskID,StartDateTime,EndDateTime;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"TaskID",TaskID);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode =retparser.CreateNodePtr(retRootNode,"SpectrumQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("TaskID",TaskID,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForAM::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //获取原始XML根元素
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("QualityQuery"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,TaskID,StartDateTime,EndDateTime,ReportTime,ChannelCode;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"TaskID",TaskID);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);
	parser.GetAttrNode(QueryNode,"ReportTime",ReportTime);
	parser.GetAttrNode(QueryNode,"ChCode",ChannelCode);
	if (ChannelCode.length()<1)
	{
		parser.GetAttrNode(QueryNode, "ChannelCode", ChannelCode);
	}

	pXMLNODE retNode =retparser.CreateNodePtr(retRootNode,"QualityQuery");
	pXMLNODE retQueryNode =retparser.CreateNodePtr(retNode,"Quality");
	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("TaskID",TaskID,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);
	retparser.SetAttrNode("ReportTime",ReportTime,retQueryNode);
	retparser.SetAttrNode("ChannelCode",ChannelCode,retQueryNode);
	retparser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForAM::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //获取原始XML根元素
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("ClientInfoQuery"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  
	std::string Index;
	pXMLNODE ClientNode =parser.GetNodeFromPath("Msg/ClientInfoQuery/HeadendInfo");
	parser.GetAttrNode(ClientNode,"Index",Index);

	pXMLNODE retClientInfoNode = retparser.CreateNodePtr(retRootNode,"ClientInfoQuery");

	pXMLNODE retClientNode = retparser.CreateNodePtr(retClientInfoNode,"HeadendInfo");

	retparser.SetAttrNode("Index",Index,retClientNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}

//返回标准的运行图设置任务xml
bool TranslateDownXMLForAM::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="AMDown")
	{
		downtype="AM";
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
	pXMLNODE retRunNode = temParser.GetNodeFromPath("Msg/RunPlanSet");
	pXMLNODELIST ChannelList = parser.GetNodeList(childNode);
	for(int count=0;count<parser.GetChildCount(childNode);count++)
	{
		pXMLNODE ChannelIndexNode =parser.GetNextNode(ChannelList);
		std::string EquCode,ChannelCode;
		string StartDateTime,EndDateTime;
		parser.GetAttrNode(ChannelIndexNode,"EquCode",EquCode);
		parser.GetAttrNode(ChannelIndexNode,"ChannelCode",ChannelCode);

		std::string DayOfWeek,StartTime,EndTime,ValidStartDateTime,ValidEndDateTime;
		int orgTimeCount = parser.GetChildCount(ChannelIndexNode);
		pXMLNODELIST orgTimeList = parser.GetNodeList(ChannelIndexNode);
		for (int i=0; i<orgTimeCount; i++)
		{
			pXMLNODE ChildIndexNode = parser.GetNextNode(orgTimeList);
			parser.GetAttrNode(ChildIndexNode,"DayOfWeek",DayOfWeek);
			parser.GetAttrNode(ChildIndexNode,"StartTime",StartTime);
			parser.GetAttrNode(ChildIndexNode,"EndTime",EndTime);
			parser.GetAttrNode(ChildIndexNode,"StartDateTime",StartDateTime);
			parser.GetAttrNode(ChildIndexNode,"EndDateTime",EndDateTime);
			parser.GetAttrNode(ChildIndexNode,"ValidStartDateTime",ValidStartDateTime);
			parser.GetAttrNode(ChildIndexNode,"ValidEndDateTime",ValidEndDateTime);

			pXMLNODE retIndexNode = temParser.CreateNodePtr(retRunNode,"RunPlanChannel");
			temParser.SetAttrNode("EquCode",EquCode,retIndexNode);
			temParser.SetAttrNode("ChannelCode",ChannelCode,retIndexNode);
			temParser.SetAttrNode("DayOfWeek",DayOfWeek,retIndexNode);
			temParser.SetAttrNode("StartTime",StartTime,retIndexNode);
			temParser.SetAttrNode("EndTime",EndTime,retIndexNode);
			temParser.SetAttrNode("StartDateTime",StartDateTime,retIndexNode);
			temParser.SetAttrNode("EndDateTime",EndDateTime,retIndexNode);
			temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,retIndexNode);
			temParser.SetAttrNode("ValidEndDateTime",ValidEndDateTime,retIndexNode);
		}
	}

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

bool TranslateDownXMLForAM::TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
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

bool TranslateDownXMLForAM::TranslateTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //获取原始XML根元素
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("TaskSchedulerQuery"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  
	std::string EquCode,StartDateTime,EndDateTime;
	pXMLNODE QueryNode =parser.GetNodeFromPath("Msg/TaskSchedulerQuery");
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode = retparser.CreateNodePtr(retRootNode,"TaskSchedulerQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}

bool TranslateDownXMLForAM::TranslateManualTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //获取原始XML根元素
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
	retparser.SetAttrNode("DVBType",string("AM"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("ManualTaskSchedulerQuery"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  
	std::string EquCode,StartDateTime,EndDateTime;
	pXMLNODE QueryNode =parser.GetNodeFromPath("Msg/ManualTaskSchedulerQuery");
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode = retparser.CreateNodePtr(retRootNode,"ManualTaskSchedulerQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}