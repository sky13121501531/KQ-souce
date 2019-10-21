///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateDownXMLForDVBC.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将下发XML转化为原子任务所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#include "TranslateDownXMLForDVBC.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"

#ifdef ZONG_JU_ZHAO_BIAO
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../Foundation/StrUtil.h"
#endif


// #ifdef ZONG_JU_ZHAO_BIAO
// #include "../Foundation/Config.h"
// #endif


//返回标准的频道扫描任务XML
bool TranslateDownXMLForDVBC::TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
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
	string ScanTime, ScanType;
	string STD, Freq, SymbolRate, StartFreq, EndFreq, StepFreq,QAM;

	if(version == "3.0")
	{
		parser.GetAttrNode( childNode,"ScanDate",ScanTime );
		parser.GetAttrNode( childNode,"ScanType",ScanType );
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
		tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
		tempParser.SetAttrNode( "ScanType",ScanType,chScanNode );
		tempParser.SetAttrNode( "STD",STD,chScanNode );
		tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
		tempParser.SetAttrNode( "QAM",QAM,chScanNode );
		tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
		tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
		tempParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		for (int i=0;i!=count;++i)
		{
			pXMLNODE subchildnode=nodelist->GetNextNode();
			string NodeName = parser.GetNodeName(subchildnode);
			if(NodeName == "DemodulateInfo")
			{
				string strChassisID,strPosition,TunerDeviceID,strChanId;
				parser.GetAttrNode( subchildnode,"ChassisID",strChassisID );
				parser.GetAttrNode( subchildnode,"Position",strPosition );
				parser.GetAttrNode( subchildnode,"Demodulate",strChanId );
				TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
				tempParser.SetAttrNode( "DeviceID",TunerDeviceID,chScanNode );
			}
			else if(NodeName == "FreqInfo")
			{
				string strFreq;
				parser.GetAttrNode( subchildnode,"Freq",strFreq );
				tempParser.SetAttrNode( "Freq",strFreq,chScanNode );
			}
		}
		tempParser.SaveToString(strStandardXML);//保存到字符串
	}
	else
	{
		parser.GetAttrNode( childNode,"ScanTime",ScanTime );
		parser.GetAttrNode( childNode,"ScanType",ScanType );
		if (count==0)
		{
			
			//获取频道扫描各属性
		
			parser.GetAttrNode( childNode,"SymbolRate",SymbolRate );
			parser.GetAttrNode( childNode,"QAM",QAM );
			//设置频道扫描属性
			pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
			tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
			tempParser.SetAttrNode( "ScanType",ScanType,chScanNode );
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
				tempParser.SetAttrNode( "ScanType",ScanType,chScanNode );
				tempParser.SetAttrNode( "STD",STD,chScanNode );
				tempParser.SetAttrNode( "Freq",Freq,chScanNode );
				tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
				tempParser.SetAttrNode( "QAM",QAM,chScanNode );
				tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
				tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
				tempParser.SaveToString(strStandardXML);//保存到字符串
			}
		}
	}

	return true;
}
//返回标准的实时视频任务XML
bool TranslateDownXMLForDVBC::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRealtimeQuery><RealtimeQueryTask/></StreamRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//头信息
	eDVBType edvbType = UNKNOWN;
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

	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
		edvbType = CTTB;
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
		edvbType = DVBS;
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
		edvbType = RADIO;
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
		edvbType = AM;
	}
	else
	{
		return false;
	}
	//

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
	//获取子属性
	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	pXMLNODE realTimeNode = tempParser.GetNodeFirstChild( tempChildNode );

	//
	std::string strRunTime;
	//初始化属性值
	string TunerDeviceID,CoderDeviceID,freq, OrgNetID,tsID, serviceID, videoPID, audioPID;
	string width, height,fps,bps,isosd;
	string dataType;
	string strCodingFormat,strIsAudiotrans,strencoder,strWindowNO;
	string strChassisID,strPosition,strModuleID,strChanId;
	if(version == "3.0")
	{
		pXMLNODE childNode = parser.GetNodeFromPath("Msg/RealStreamQuery");
		parser.GetAttrNode(childNode, "RunTime", strRunTime);
		parser.GetAttrNode(childNode, "WindowNO", strWindowNO);
		parser.GetAttrNode( childNode,"OSDEnable",isosd);
		parser.GetAttrNode( childNode,"CodingFormat",strCodingFormat);
		parser.GetAttrNode( childNode,"Width",width);
		parser.GetAttrNode( childNode,"Height",height);
		parser.GetAttrNode( childNode,"Fps",fps);
		parser.GetAttrNode( childNode,"Bps",bps);
		bps+="000";
		parser.GetAttrNode(childNode, "Encoder", strencoder);
		//
		pXMLNODE subchildNode = parser.GetNodeFromPath("Msg/RealStreamQuery/ChannelInfo");
		parser.GetAttrNode( subchildNode,"Freq",freq );
		parser.GetAttrNode( subchildNode,"ServiceID",serviceID );
		parser.GetAttrNode( subchildNode,"Audio",strIsAudiotrans );
		pXMLNODE subsubchildNode = parser.GetNodeFromPath("Msg/RealStreamQuery/ChannelInfo/DemodulateInfo");
		parser.GetAttrNode( subsubchildNode,"ChassisID",strChassisID );
		parser.GetAttrNode( subsubchildNode,"Position",strPosition );
		parser.GetAttrNode( subsubchildNode,"Demodulate",strChanId );
		TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(edvbType,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
		pXMLNODE transsubchildNode = parser.GetNodeFromPath("Msg/RealStreamQuery/TranscodeInfo");
		if(TunerDeviceID == "-1")
		{
			CoderDeviceID = string("-1");
		}
		else
		{
			CoderDeviceID = "";
			pXMLNODELIST CodchildNodeList=parser.GetNodeList(transsubchildNode);
			for (int iij=0;iij<CodchildNodeList->Size();iij++)
			{
				pXMLNODE CodchildNode=parser.GetNextNode(CodchildNodeList);
				parser.GetAttrNode( CodchildNode,"ChassisID",strChassisID );
				parser.GetAttrNode( CodchildNode,"Position",strPosition );
				parser.GetAttrNode( CodchildNode,"ModuleID",strModuleID );
				parser.GetAttrNode( CodchildNode,"Transcode",strChanId);
				if(CoderDeviceID.size() == 0)
				{
					CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
				}
				else
				{
					CoderDeviceID += ";";
					CoderDeviceID += StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
				}
			}
		}
		string Channelid = "";
		CHANNELINFOMGR::instance()->GetChannelID(edvbType,freq,serviceID,Channelid);
		string proname;
		CHANNELINFOMGR::instance()->GetChannelInfo(edvbType,Channelid,videoPID,audioPID,serviceID,proname);
	}
	else
	{
		pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );
	#ifdef ZONG_JU_ZHAO_BIAO
		parser.GetAttrNode(childNode, "RunTime", strRunTime);
	#endif
		//获取视频查询元素
		pXMLNODE realStreamNode = parser.GetNodeFirstChild( childNode );
		//获取视频查询各属性
		parser.GetAttrNode( realStreamNode,"Freq",freq );
		parser.GetAttrNode( realStreamNode,"ServiceID",serviceID );
		parser.GetAttrNode( realStreamNode,"VideoPID",videoPID );
		parser.GetAttrNode( realStreamNode,"AudioPID",audioPID);
	#ifdef ZONG_JU_ZHAO_BIAO
		parser.GetAttrNode( realStreamNode,"CodingFormat",strCodingFormat);
		parser.GetAttrNode( realStreamNode,"Width",width);
		parser.GetAttrNode( realStreamNode,"Height",height);
		parser.GetAttrNode( realStreamNode,"Fps",fps);
		parser.GetAttrNode( realStreamNode,"Bps",bps);
		parser.GetAttrNode( realStreamNode,"OSDEnable",isosd);
	#endif
		if (freq != strHDMIFreq)
		{
			//获取OrgNetID,tsID
			CHANNELINFOMGR::instance()->GetOrgNetID(DVBC,freq,audioPID,videoPID,serviceID,OrgNetID );
			CHANNELINFOMGR::instance()->GetTsID(DVBC,freq,audioPID,videoPID,serviceID,tsID);
		}
	}
	tempParser.SetAttrNode( "WindowNO",strWindowNO,realTimeNode );
	tempParser.SetAttrNode( "Audio",strIsAudiotrans,realTimeNode );
	tempParser.SetAttrNode( "Encoder",strencoder,realTimeNode );
	tempParser.SetAttrNode( "CDeviceID",CoderDeviceID,realTimeNode );
	tempParser.SetAttrNode( "TDeviceID",TunerDeviceID,realTimeNode );
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
	tempParser.SetAttrNode( "OSDEnable",isosd,realTimeNode );

#ifdef ZONG_JU_ZHAO_BIAO
	tempParser.SetAttrNode("RunTime", strRunTime, realTimeNode);
	tempParser.SetAttrNode("CodingFormat", strCodingFormat, realTimeNode);
#endif

	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的视频轮播任务XML
bool TranslateDownXMLForDVBC::TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi)
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
	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
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
	string width, height,fps,bps,isosd;
	string dataType;
	string strChassisID,strPosition,strModuleID,strChanId;
	int RunTime;

	pXMLNODE RoundStreamNode=parser.GetNodeFromPath("Msg/StreamRoundInfoQuery/RoundStream");
	parser.GetAttrNode(RoundStreamNode,"RoundTime",RoundTime);
	parser.GetAttrNode(RoundStreamNode,"RunTime",RunTime);

#ifdef ZONG_JU_ZHAO_BIAO
	std::string strWindowNum, strCodingFormat;
	parser.GetAttrNode(RoundStreamNode,"WindowNumber",strWindowNum);
	parser.GetAttrNode(RoundStreamNode,"CodingFormat",strCodingFormat);
	parser.GetAttrNode(RoundStreamNode,"Width",width);
	parser.GetAttrNode(RoundStreamNode,"Height",height);
	parser.GetAttrNode(RoundStreamNode,"Fps",fps);
	parser.GetAttrNode(RoundStreamNode,"Bps",bps);
	parser.GetAttrNode(RoundStreamNode,"OSDEnable",isosd);
#endif

	pXMLNODE tempNode=tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	tempParser.SetAttrNode("DeviceID",deviceID,tempNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempNode);
	tempParser.SetAttrNode("RunTime",RunTime,tempNode);

#ifdef ZONG_JU_ZHAO_BIAO
	tempParser.SetAttrNode("WindowNumber",strWindowNum,tempNode);
	tempParser.SetAttrNode("CodingFormat",strCodingFormat,tempNode);
#endif
	

	pXMLNODELIST roundStreamList=parser.GetNodeList(RoundStreamNode);
	for(int i=0;i<roundStreamList->Size();++i)
	{
		pXMLNODE childNode=parser.GetNextNode(roundStreamList);
		string NodeName = parser.GetNodeName(childNode);
		if((NodeName == "ChannelInfo")||(NodeName == "Channel"))
		{
			pXMLNODE tempChannelNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundChannel");
			if(NodeName == "Channel")
			{
				parser.GetAttrNode( childNode,"Freq",freq );
				parser.GetAttrNode( childNode,"ServiceID",serviceID );
				parser.GetAttrNode( childNode,"VideoPID",videoPID );
				parser.GetAttrNode( childNode,"AudioPID",audioPID);
			}
			else
			{
				parser.GetAttrNode( childNode,"Freq",freq );
				parser.GetAttrNode( childNode,"ServiceID",serviceID );
				string Channelid = "";
				CHANNELINFOMGR::instance()->GetChannelID(DVBC,freq,serviceID,Channelid);
				string proname;
				CHANNELINFOMGR::instance()->GetChannelInfo(DVBC,Channelid,videoPID,audioPID,serviceID,proname);
			}
			//获取OrgNetID,tsID
			CHANNELINFOMGR::instance()->GetOrgNetID(DVBC,freq,audioPID,videoPID,serviceID,OrgNetID );
			CHANNELINFOMGR::instance()->GetTsID(DVBC,freq,audioPID,videoPID,serviceID,tsID);


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
			tempParser.SetAttrNode( "OSDEnable",isosd,tempChannelNode );
		}
		else if(NodeName == "DemodulateInfo")
		{
			pXMLNODELIST TunNodeList=parser.GetNodeList(childNode);
			for(int i=0;i<TunNodeList->Size();++i)
			{
				pXMLNODE TunchildNode=parser.GetNextNode(TunNodeList);
				pXMLNODE tempTunerNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundTuner");
				parser.GetAttrNode( TunchildNode,"ChassisID",strChassisID );
				parser.GetAttrNode( TunchildNode,"Position",strPosition );
				parser.GetAttrNode( TunchildNode,"Demodulate",strChanId );
				string 	TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
				tempParser.SetAttrNode( "Info",TunerDeviceID,tempTunerNode );
			}
		}
		else if(NodeName == "TranscodeInfo")
		{
			pXMLNODELIST CodNodeList=parser.GetNodeList(childNode);
			for(int i=0;i<CodNodeList->Size();++i)
			{
				pXMLNODE CodchildNode=parser.GetNextNode(CodNodeList);
				pXMLNODE tempCoderNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundCoder");
				parser.GetAttrNode( CodchildNode,"ChassisID",strChassisID );
				parser.GetAttrNode( CodchildNode,"Position",strPosition );
				parser.GetAttrNode( CodchildNode,"ModuleID",strModuleID );
				parser.GetAttrNode( CodchildNode,"Transcode",strChanId);
				string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
				tempParser.SetAttrNode( "Info",CoderDeviceID,tempCoderNode );
			}
		}

	}
	//保存到字符串
	tempParser.SaveToString( strStandardXML );
	return true;
}
//返回标准的录像设置任务XML
bool TranslateDownXMLForDVBC::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime,srccode,dstcode,srcurl;//头信息
	string strCChanIdlist,strTChassisID,strTPosition,strTChanId,strCChassisID,strCPosition,strCModuleID,strCChanId;
	string TunerDeviceID,CoderDeviceID;
	int priority;
	eDVBType edvbType = UNKNOWN;
	//获取根元素
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version);
	parser.GetAttrNode( rootNode,"Protocol",protocol);
	parser.GetAttrNode( rootNode,"SrcCode",srccode);
	parser.GetAttrNode( rootNode,"DstCode",dstcode);
	parser.GetAttrNode( rootNode,"DateTime",dateTime);
	parser.GetAttrNode( rootNode,"Priority",priority);
	parser.GetAttrNode( rootNode,"SrcURL",srcurl);
	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
		edvbType = CTTB;
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
		edvbType = DVBS;
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
		edvbType = RADIO;
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
		edvbType = AM;
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
		if(version == "3.0")
		{
			pXMLNODE autoRecordSetNode=tempParser.CreateNodePtr("Msg","AutoRecordSet");
			pXMLNODE autoRecordNode=parser.GetNodeFromPath("Msg/SetAutoRecordChannel");
			pXMLNODELIST autoRecordList=parser.GetNodeList(autoRecordNode);
			string action = "Set";
			for (int i=0;i<autoRecordList->Size();i++)
			{
				pXMLNODE node=parser.GetNextNode(autoRecordList);
				//parser.GetAttrNode(node,"Action",action);

				std::string Freq;
				std::string IndexSet = "",Step = "";
	#ifdef ZONG_JU_ZHAO_BIAO
				std::string strCodingFormat;
	#endif
				parser.GetAttrNode(node,"Freq",Freq);
				parser.GetAttrNode(node,"IndexSet",IndexSet);

				pXMLNODELIST nodeList=parser.GetNodeList(node);
				for (int j=0;j<nodeList->Size();j++)
				{
					string strBps, isosd,ServiceType,Encoder,strFps, strWidth, strHeight, DeviceID,OrgNetID,TsID,ServiceID,Code,TaskID="0",VideoPID,AudioPID,DataType,Index;
					string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
					pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
					pXMLNODE childNode=parser.GetNextNode(nodeList);

					parser.GetAttrNode(childNode,"OSDEnable", isosd);
					parser.GetAttrNode(childNode, "CodingFormat",strCodingFormat);
					parser.GetAttrNode(childNode,"ServiceType", ServiceType);
					parser.GetAttrNode(childNode,"Encoder", Encoder);
					parser.GetAttrNode(childNode,"Bps",strBps);
					strBps+="000";
					parser.GetAttrNode(childNode, "Fps", strFps);
					parser.GetAttrNode(childNode, "Width", strWidth);
					parser.GetAttrNode(childNode, "Height", strHeight);
					parser.GetAttrNode(childNode,"Index",Index);
					parser.GetAttrNode(childNode,"VideoPID",VideoPID);
					parser.GetAttrNode(childNode,"AudioPID",AudioPID);
					parser.GetAttrNode(childNode,"ServiceID",ServiceID);
					parser.GetAttrNode(childNode,"Step",Step);
					//
					pXMLNODELIST childnodeList=parser.GetNodeList(childNode);
					for (int ij=0;ij<childnodeList->Size();ij++)
					{
						pXMLNODE TunCodchildNode=parser.GetNextNode(childnodeList);
						string NodeName = parser.GetNodeName(TunCodchildNode);
						if(NodeName == "DemodulateInfo")
						{
							parser.GetAttrNode( TunCodchildNode,"ChassisID",strTChassisID );
							parser.GetAttrNode( TunCodchildNode,"Position",strTPosition );
							parser.GetAttrNode( TunCodchildNode,"Demodulate",strTChanId );
							TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(edvbType,StrUtil::Str2Int(strTChassisID),StrUtil::Str2Int(strTPosition),StrUtil::Str2Int(strTChanId)));
						}
						else if(NodeName == "TranscodeInfo")
						{
							CoderDeviceID = "";
							strCChanIdlist = "";
							pXMLNODELIST CodchildNodeList=parser.GetNodeList(TunCodchildNode);
							for (int iij=0;iij<CodchildNodeList->Size();iij++)
							{
								pXMLNODE CodchildNode=parser.GetNextNode(CodchildNodeList);
								parser.GetAttrNode( CodchildNode,"ChassisID",strCChassisID );
								parser.GetAttrNode( CodchildNode,"Position",strCPosition );
								parser.GetAttrNode( CodchildNode,"ModuleID",strCModuleID );
								parser.GetAttrNode( CodchildNode,"Transcode",strCChanId);
								if(strCChanIdlist.size() == 0)
								{
									strCChanIdlist = strCChanId;
								}
								else
								{
									strCChanIdlist += ";";
									strCChanIdlist += strCChanId;
								}
								if(CoderDeviceID.size() == 0)
								{
									CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),StrUtil::Str2Int(strCChanId)));
								}
								else
								{
									CoderDeviceID += ";";
									CoderDeviceID += StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),StrUtil::Str2Int(strCChanId)));
								}
							}
						}
					}
					//
					PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"DVBC");
					if (Freq != strHDMIFreq)
					{
						CHANNELINFOMGR::instance()->GetTsID(edvbType,Freq,AudioPID,VideoPID,ServiceID,TsID);
						CHANNELINFOMGR::instance()->GetOrgNetID(edvbType,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
					}

					tempParser.SetAttrNode( "CDeviceID",CoderDeviceID,recordNode );
					tempParser.SetAttrNode( "TDeviceID",TunerDeviceID,recordNode );
					tempParser.SetAttrNode( "CChassisID",strCChassisID,recordNode );
					tempParser.SetAttrNode( "CPosition",strCPosition,recordNode );
					tempParser.SetAttrNode( "CModuleID",strCModuleID,recordNode );
					tempParser.SetAttrNode( "CTranscode",strCChanIdlist,recordNode );
					tempParser.SetAttrNode( "TChassisID",strTChassisID,recordNode );
					tempParser.SetAttrNode( "TPosition",strTPosition,recordNode );
					tempParser.SetAttrNode( "TDemodulate",strTChanId,recordNode );
					tempParser.SetAttrNode("TaskID",TaskID,recordNode);
					tempParser.SetAttrNode("Action",action,recordNode);
					tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
					tempParser.SetAttrNode("Freq",Freq,recordNode);
					tempParser.SetAttrNode("Code",Code,recordNode);
					tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
					tempParser.SetAttrNode("TsID",TsID,recordNode);
					tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
					if (VideoPID.size() <= 0 || AudioPID.size() <= 0)
					{
						string ChanId, sProgramName;
						CHANNELINFOMGR::instance()->GetChanIdByFreqServId(edvbType, Freq, ServiceID, ChanId);
						CHANNELINFOMGR::instance()->GetChannelInfo(edvbType, ChanId, VideoPID, AudioPID, ServiceID, sProgramName);
					}
					tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
					tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
					tempParser.SetAttrNode("DataType",DataType,recordNode);
					tempParser.SetAttrNode("Width",strWidth,recordNode);
					tempParser.SetAttrNode("Height",strHeight,recordNode);
					tempParser.SetAttrNode("Fps",strFps,recordNode);
					tempParser.SetAttrNode("Bps",strBps,recordNode);
					tempParser.SetAttrNode( "OSDEnable", isosd, recordNode );
					tempParser.SetAttrNode("IndexSet",IndexSet,recordNode);
					tempParser.SetAttrNode("Step",Step,recordNode);
					tempParser.SetAttrNode("ServiceType",ServiceType,recordNode);
					tempParser.SetAttrNode("Encoder",Encoder,recordNode);

	#ifdef ZONG_JU_ZHAO_BIAO
					tempParser.SetAttrNode("CodingFormat",strCodingFormat,recordNode);
	#endif

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
			pXMLNODE autoRecordSetNode=tempParser.CreateNodePtr("Msg","AutoRecordSet");
			pXMLNODE autoRecordNode=parser.GetNodeFromPath("Msg/SetAutoRecordChannel");
			pXMLNODELIST autoRecordList=parser.GetNodeList(autoRecordNode);
			string action;
			for (int i=0;i<autoRecordList->Size();++i)
			{
				pXMLNODE node=parser.GetNextNode(autoRecordList);
				parser.GetAttrNode(node,"Action",action);

				std::string strWidth, strHeight, strBps, strFps, isosd;
				std::string IndexSet = "",Step = "";
	#ifdef ZONG_JU_ZHAO_BIAO
				std::string strCodingFormat;
				parser.GetAttrNode(node, "CodingFormat", strCodingFormat);
				parser.GetAttrNode(node, "Width", strWidth);
				parser.GetAttrNode(node, "Height", strHeight);
				parser.GetAttrNode(node, "Bps", strBps);
				parser.GetAttrNode(node, "Fps", strFps);
	#endif
				parser.GetAttrNode(node,"OSDEnable", isosd);

				pXMLNODELIST nodeList=parser.GetNodeList(node);
				for (int j=0;j<nodeList->Size();++j)
				{
					string DeviceID,OrgNetID,TsID,ServiceID,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,Index,strQAM,strSymbolRate;
					string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
					pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
					pXMLNODE childNode=parser.GetNextNode(nodeList);

					parser.GetAttrNode(childNode,"Index",Index);
					parser.GetAttrNode(childNode,"Freq",Freq);
					parser.GetAttrNode(childNode,"VideoPID",VideoPID);
					parser.GetAttrNode(childNode,"AudioPID",AudioPID);
					parser.GetAttrNode(childNode,"ServiceID",ServiceID);
					parser.GetAttrNode(childNode,"IndexSet",IndexSet);
					parser.GetAttrNode(childNode,"Step",Step);
					parser.GetAttrNode(childNode,"QAM",strQAM);
					parser.GetAttrNode(childNode,"SymbolRate",strSymbolRate);

					PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"DVBC");
					if (Freq != strHDMIFreq)
					{
						CHANNELINFOMGR::instance()->GetTsID(DVBC,Freq,AudioPID,VideoPID,ServiceID,TsID);
						CHANNELINFOMGR::instance()->GetOrgNetID(DVBC,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
					}

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
					tempParser.SetAttrNode("Width",strWidth,recordNode);
					tempParser.SetAttrNode("QAM",strQAM,recordNode);
					tempParser.SetAttrNode("SymbolRate",strSymbolRate,recordNode);
					tempParser.SetAttrNode("Height",strHeight,recordNode);
					tempParser.SetAttrNode("Fps",strFps,recordNode);
					tempParser.SetAttrNode("Bps",strBps,recordNode);
					tempParser.SetAttrNode( "OSDEnable", isosd, recordNode );
					tempParser.SetAttrNode("IndexSet",IndexSet,recordNode);
					tempParser.SetAttrNode("Step",Step,recordNode);

	#ifdef ZONG_JU_ZHAO_BIAO
					tempParser.SetAttrNode("CodingFormat",strCodingFormat,recordNode);
	#endif

					tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
					tempParser.SetAttrNode("StartTime",StartTime,recordNode);
					tempParser.SetAttrNode("EndTime",EndTime,recordNode);
					tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
					tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
				}

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
			string DeviceID,Width,Height,Fps,Bps,TaskID,Index;
			pXMLNODE taskRecordNode=parser.GetNextNode(proRecordTaskSetList);
			parser.GetAttrNode(taskRecordNode,"TaskID",TaskID);
			parser.GetAttrNode(taskRecordNode,"Action",Action);
			parser.GetAttrNode(taskRecordNode,"Index",Index);
			parser.GetAttrNode(taskRecordNode,"Width",Width);
			parser.GetAttrNode(taskRecordNode,"Height",Height);
			parser.GetAttrNode(taskRecordNode,"Fps",Fps);
			parser.GetAttrNode(taskRecordNode,"Bps",Bps);

#ifdef ZONG_JU_ZHAO_BIAO
			std::string strLifeCycle, strCodingFormat;
			parser.GetAttrNode(taskRecordNode, "CodingFormat", strCodingFormat);
			parser.GetAttrNode(taskRecordNode, "Width", Width);
			parser.GetAttrNode(taskRecordNode, "Height", Height);
			parser.GetAttrNode(taskRecordNode, "Bps", Bps);
			parser.GetAttrNode(taskRecordNode, "Fps", Fps);
			parser.GetAttrNode(taskRecordNode, "Lifecycle", strLifeCycle);
#endif

			PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"DVBC");

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

				CHANNELINFOMGR::instance()->GetTsID(DVBC,Freq,AudioPID,VideoPID,ServiceID,TsID);
				CHANNELINFOMGR::instance()->GetOrgNetID(DVBC,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
				parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
				parser.GetAttrNode(childNode,"StartTime",StartTime);
				parser.GetAttrNode(childNode,"EndTime",EndTime);
				parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);

#ifdef ZONG_JU_ZHAO_BIAO
				parser.GetAttrNode(childNode, "SetIndex", DeviceID);	//任务录像有可能设置通道
				
				//任务录像设置通道的情况，需要支持任务录象替换功能************FRee_fanrong_20131125
				if ( DeviceID !="")
				{					
					priority=priority+1;
					tempParser.SetAttrNode( "Priority",priority,tempRootNode );
					int cardindex = StrUtil::Str2Int(DeviceID);
					int idevid = -1;
					PROPMANAGER::instance()->GetDeviceIndexbyCardIndex(cardindex,idevid);
					DeviceID = StrUtil::Int2Str(idevid);
				}
#endif
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

#ifdef ZONG_JU_ZHAO_BIAO
				tempParser.SetAttrNode("Lifecycle",strLifeCycle,recordNode);
				tempParser.SetAttrNode("CodingFormat",strCodingFormat,recordNode);
#endif

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
bool TranslateDownXMLForDVBC::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		string Action,DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID,VideoPID,AudioPID,Freq,DataType,Encoder,ServiceType,strQAM,strSymbolRate;
		string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime, isosd;
		string IndexSet="",Step="";
		string TunerDeviceID="",CoderDeviceID="",CChanIdList="",strTChassisID="",strTPosition="",strTChanId="",strCChassisID="",strCPosition="",strCModuleID="",strCChanId="";
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
		parser.GetAttrNode(childNode,"QAM",strQAM);
		parser.GetAttrNode(childNode,"SymbolRate",strSymbolRate);
		parser.GetAttrNode(childNode,"Width",Width);
		parser.GetAttrNode(childNode,"Height",Height);
		parser.GetAttrNode(childNode,"Fps",Fps);
		parser.GetAttrNode(childNode,"Bps",Bps);
		parser.GetAttrNode(childNode,"OSDEnable", isosd);
		parser.GetAttrNode(childNode,"TDeviceID",TunerDeviceID);
		parser.GetAttrNode(childNode,"CDeviceID", CoderDeviceID);
		parser.GetAttrNode(childNode,"CChassisID",strCChassisID);
		parser.GetAttrNode(childNode,"CPosition",strCPosition);
		parser.GetAttrNode(childNode,"CModuleID",strCModuleID);
		parser.GetAttrNode(childNode,"CTranscode",CChanIdList);
		parser.GetAttrNode(childNode,"TChassisID",strTChassisID);
		parser.GetAttrNode(childNode,"TPosition",strTPosition);
		parser.GetAttrNode(childNode,"TDemodulate",strTChanId);

		parser.GetAttrNode(childNode,"IndexSet",IndexSet);
		parser.GetAttrNode(childNode,"Step",Step);
		parser.GetAttrNode(childNode,"ServiceType", ServiceType);
		parser.GetAttrNode(childNode,"Encoder", Encoder);
#ifdef ZONG_JU_ZHAO_BIAO
		std::string strLifeCycle,strCodingFormat;
		parser.GetAttrNode(childNode,"Lifecycle",strLifeCycle);
		parser.GetAttrNode(childNode,"CodingFormat",strCodingFormat);
		parser.GetAttrNode(childNode,"SetIndex",DeviceID);	//任务录像可能会下发通道
#endif
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
		tempParser.SetAttrNode("QAM",strQAM,recordNode);
		tempParser.SetAttrNode("SymbolRate",strSymbolRate,recordNode);
		tempParser.SetAttrNode( "OSDEnable", isosd, recordNode );
		tempParser.SetAttrNode("IndexSet",IndexSet,recordNode);
		tempParser.SetAttrNode("Step",Step,recordNode);
		tempParser.SetAttrNode("TDeviceID",TunerDeviceID,recordNode);
		tempParser.SetAttrNode("CDeviceID", CoderDeviceID,recordNode);
		tempParser.SetAttrNode( "CChassisID",strCChassisID,recordNode );
		tempParser.SetAttrNode( "CPosition",strCPosition,recordNode );
		tempParser.SetAttrNode( "CModuleID",strCModuleID,recordNode );
		tempParser.SetAttrNode( "CTranscode",CChanIdList,recordNode );
		tempParser.SetAttrNode( "TChassisID",strTChassisID,recordNode );
		tempParser.SetAttrNode( "TPosition",strTPosition,recordNode );
		tempParser.SetAttrNode( "TDemodulate",strTChanId,recordNode );
		tempParser.SetAttrNode("ServiceType", ServiceType,recordNode);
		tempParser.SetAttrNode("Encoder", Encoder,recordNode);
#ifdef ZONG_JU_ZHAO_BIAO
		tempParser.SetAttrNode("Lifecycle",strLifeCycle,recordNode);
		tempParser.SetAttrNode("CodingFormat",strCodingFormat,recordNode);
#endif
		tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
		tempParser.SetAttrNode("StartTime",StartTime,recordNode);
		tempParser.SetAttrNode("EndTime",EndTime,recordNode);
		tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
		tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);

		tempParser.SaveToString(xml);
		tempParser.DeleteNode(MyRecordNode,recordNode);
		if((TunerDeviceID != "-1")&&(CoderDeviceID != "-1"))
		{
			strStandardXMLVec.push_back(xml);
		}
		else
		{
			cout<<"任务设置失败,通道和信号不对应!"<<endl;
		}
	}
	return true;
}
//返回标准的录像查询、下载XML
bool TranslateDownXMLForDVBC::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//初始化属性值
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, OrgNetID, tsID, serviceID,VideoID,AudioID,code, StartDateTime, EndDateTime, DeviceID,ProgramID;
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
	if ((nodeName =="NVRVideoHistoryDownInquiry")||(nodeName =="AutoRecordDownload"))
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
	else if ((nodeName =="NVRVideoHistoryLostInquiry")||(nodeName =="AutoRecordIntegraity"))
	{
		nodeName = "RecordFileLostQuery";                        //修改查询类型节点名称
	}
	char * taskType = const_cast<char *>(nodeName.c_str());
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );
	//
	string strUseChassisID,strUsePosition,strUseModuleID,strUseChanId;
	if(strVersion == "3.0")
	{
		pXMLNODE TranscodeNode = parser.GetNodeFirstChild( recordNode );
		pXMLNODELIST CodNodeList=parser.GetNodeList(TranscodeNode);
		string strCoderList;
		for(int i=0;i<CodNodeList->Size();++i)
		{
			string strChassisID,strPosition,strModuleID,strChanId;
			pXMLNODE CodchildNode=parser.GetNextNode(CodNodeList);
			parser.GetAttrNode( CodchildNode,"ChassisID",strChassisID );
			parser.GetAttrNode( CodchildNode,"Position",strPosition );
			parser.GetAttrNode( CodchildNode,"ModuleID",strModuleID );
			parser.GetAttrNode( CodchildNode,"Transcode",strChanId);
			if(i == 0)
			{
				strUseChassisID = strChassisID;
				strUsePosition = strPosition;
				strUseModuleID = strModuleID;
				strUseChanId = strChanId;
			}
			string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
			if(strCoderList.size()>0)
			{
				strCoderList += ";";
				strCoderList += CoderDeviceID;
			}
			else
			{
				strCoderList = CoderDeviceID;
			}
		}
		DeviceID = 	strCoderList;
	}
	//设置根属性
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //设置Msg节点的Version属性值
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //设置Msg节点的MsgID属性值
	if(!strcmp(strType.c_str(),"MonDown"))
	{
		strType="DVBC";
	}
	else if(!strcmp(strType.c_str(),"DVBCDown"))
	{
		strType="DVBC";
	}
	else if(!strcmp(strType.c_str(),"DTMBDown"))
	{
		strType="CTTB";
	}
	else if(!strcmp(strType.c_str(),"DVBSDown"))
	{
		strType="DVBS";
	}
	else if(!strcmp(strType.c_str(),"RADIODown"))
	{
		strType="RADIO";
	}
	else if(!strcmp(strType.c_str(),"AMDown"))
	{
		strType="AM";
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("DVBType",strType,tempRootNode);           //设置Msg节点的DVBType属性值
	tempParser.SetAttrNode("TaskType",string("RecordQuery"),tempRootNode);   //设置Msg节点的TaskType属性值
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);             //设置Msg节点的Protocol属性值
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);          //设置Msg节点的DateTime属性值
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);               //设置Msg节点的StrCode属性值
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);               //设置Msg节点的DstCode属性值
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                 //设置Msg节点的SrcUrl属性值
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);             //设置Msg节点的Priority属性值

	pXMLNODE query = tempParser.CreateNodePtr(tempRootNode,taskType);	         //创建查询类型节点
	pXMLNODE recordTaskNode = tempParser.CreateNodePtr(query,"RecordQueryTask"); //创建RecordQueryTask节点

#ifdef ZONG_JU_ZHAO_BIAO
	std::string tempFreq,rate, indexSet;
#endif
	//获取录像任务各属性
	if ( (nodeName.compare("AutoRecordFileQuery") == 0)||(nodeName.compare("AutoRecordQuery") == 0))
	{
		if(strVersion != "3.0")
		{
			parser.GetAttrNode( recordNode,"Index",DeviceID);                    //获取Index属性值
			PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"DVBC");	//将逻辑通道转化为实际通道
		}
#ifdef ZONG_JU_ZHAO_BIAO
		//if (DeviceID.length()<1)
		{
			parser.GetAttrNode( recordNode,"ServiceID",serviceID );
			parser.GetAttrNode( recordNode,"Rate",rate );
			parser.GetAttrNode( recordNode,"Freq", tempFreq);
		}
#endif

		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //获取StartDateTime属性值
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //获取EndDateTime属性值
		parser.GetAttrNode( recordNode, "IndexSet", indexSet);
	}
	else if (nodeName.compare("TaskRecordQuery") == 0)
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );                    //获取TaskID属性值
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //获取StartDateTime属性值
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //获取EndDateTime属性值
	}
	else if (nodeName.compare("RecordFileLostQuery") == 0)
	{
		if(strVersion != "3.0")
		{
			parser.GetAttrNode( recordNode,"Index",DeviceID);                    //获取Index属性值
			PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"DVBC");	//将逻辑通道转化为实际通道
		}
		parser.GetAttrNode( recordNode,"ServiceID",serviceID );
		parser.GetAttrNode( recordNode,"Freq", tempFreq);
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //获取StartDateTime属性值
		if(StartDateTime.size()==0)//兼容3.0测试平台
		{
			parser.GetAttrNode( recordNode,"startDateTime",StartDateTime );//StartDateTime属性值
		}
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
#ifdef ZONG_JU_ZHAO_BIAO
		tempParser.SetAttrNode( "Freq",tempFreq,recordTaskNode );
#endif
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,recordTaskNode );           //设置RecordQueryTask节点的OrgNetID属性值
		tempParser.SetAttrNode( "TsID",tsID,recordTaskNode );                   //设置RecordQueryTask节点的TsID属性值
		tempParser.SetAttrNode( "ServiceID",serviceID,recordTaskNode );         //设置RecordQueryTask节点的ServiceID属性值
		tempParser.SetAttrNode( "VideoPID",VideoID,recordTaskNode );             //设置RecordQueryTask节点的VideoID属性值
		tempParser.SetAttrNode( "AudioPID",AudioID,recordTaskNode);              //设置RecordQueryTask节点的AudioID属性值
		tempParser.SetAttrNode( "Code",code,recordTaskNode);                    //设置RecordQueryTask节点的Code属性值
		tempParser.SetAttrNode( "DeviceID",DeviceID,recordTaskNode );           //设置RecordQueryTask节点的DeviceID属性值
		tempParser.SetAttrNode( "Rate",rate,recordTaskNode );           //设置RecordQueryTask节点的DeviceID属性值
		tempParser.SetAttrNode( "StartDateTime",StartDateTime,recordTaskNode ); //设置RecordQueryTask节点的StartDateTime属性值
		tempParser.SetAttrNode( "EndDateTime",EndDateTime,recordTaskNode );     //设置RecordQueryTask节点的EndDateTime属性值
		tempParser.SetAttrNode( "IndexSet", indexSet, recordTaskNode );
		if(strVersion == "3.0")
		{
			tempParser.SetAttrNode( "ChassisID",strUseChassisID, recordTaskNode);
			tempParser.SetAttrNode( "Position",strUsePosition, recordTaskNode);
			tempParser.SetAttrNode( "ModuleID",strUseModuleID, recordTaskNode);
			tempParser.SetAttrNode( "Transcode",strUseChanId, recordTaskNode);
		}
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
bool TranslateDownXMLForDVBC::TranslateRoundRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
bool TranslateDownXMLForDVBC::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	string downType;//定义检测类型

	//视频查询标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><QualityRealtimeQuery><QualityQueryTask/><QualityParam></QualityParam></QualityRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//获取根元素
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Type",downType);

	//初始化属性值
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,msgid,version,protocol,srccode,dstcode,srcurl;
	string dayOfWeek,startTime,endTime,startDateTime,endDateTime,checkInterval;
	string Type,Desc,Value;
	int count = 0;
	string tasktype="QualityRealtimeQuery";	
	string periodicity="Once";
	eDVBType edvbType = UNKNOWN;
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
	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
		edvbType = CTTB;
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
		edvbType = DVBS;
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
		edvbType = RADIO;
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
		edvbType = AM;
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
	pXMLNODE childNode;
	if(version == "3.0")
	{
		pXMLNODE IndexQuerychildNode = parser.GetNodeFirstChild(rootNode);;
		childNode = parser.GetNodeFirstChild(IndexQuerychildNode);;
		parser.GetAttrNode( childNode,"Freq",freq );
	}
	else
	{
		childNode = parser.GetNodeFirstChild(rootNode);;
		parser.GetAttrNode( childNode,"Freq",freq );
		parser.GetAttrNode(childNode,"SymbolRate",symbolRate);
	}


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
			string NodeName = parser.GetNodeName(listNode);
			if(NodeName == "DemodulateInfo")
			{
				string strChassisID,strPosition,strChanId,TunerDeviceID;
				parser.GetAttrNode( listNode,"ChassisID",strChassisID );
				parser.GetAttrNode( listNode,"Position",strPosition );
				parser.GetAttrNode( listNode,"Demodulate",strChanId );
				TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(edvbType,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
				tempParser.SetAttrNode( "DeviceID",TunerDeviceID,qualityTaskNode );
			}
			else
			{
				parser.GetAttrNode( listNode,"Type",Type );
				parser.GetAttrNode( listNode,"Desc",Desc );
				tempParser.SetAttrNode( "Type",Type,tempListNode );
				tempParser.SetAttrNode( "Desc",Desc,tempListNode );
				tempParser.SetAttrNode( "Value",Value,tempListNode );
			}
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
bool TranslateDownXMLForDVBC::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (strType =="MonDown")
	{
		tempParser.SetAttrNode("DVBType",string("DVBC"),tempRootNode);           //设置Msg节点的DVBType属性值
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
bool TranslateDownXMLForDVBC::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		std::string TaskID, Action,DevideID,TunerIndex;

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
		tempParser.SetAttrNode("DVBType",string("DVBC"),tempRootNode);               //设置Msg节点的DVBType属性值
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
		parser.GetAttrNode(childNode,"TunerIndex",TunerIndex);                                //获取原始XML中QualityTaskSet节点的Action属性
		tempParser.SetAttrNode("TaskID",TaskID,qualityTaskNode);                              //设置QualityTask节点的TaskID属性
		tempParser.SetAttrNode("Action",Action,qualityTaskNode);                              //设置QualityTask节点的Action属性
		tempParser.SetAttrNode("DeviceID",DevideID,qualityTaskNode);                          //设置QualityTask节点的DevieceID属性
		tempParser.SetAttrNode("TunerIndex",TunerIndex,qualityTaskNode);                          //设置QualityTask节点的DevieceID属性

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
bool TranslateDownXMLForDVBC::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, EquCode,STD,Freq,StartDateTime,EndDateTime,ReportTime, sIndex;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //获取原始XML根元素
	pXMLNODE childNode = parser.GetNodeFirstChild(rootNode);		//根节点的第一个子节点
	std::string TaskName = parser.GetNodeName(childNode);				//获得任务名称
	
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
	if (strType =="MonDown")
	{
		tempParser.SetAttrNode("DVBType",string("DVBC"),tempRootNode);           //设置Msg节点的DVBType属性值
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

	if ((TaskName == "GetIndexSet")||(TaskName == "IndexQuery"))
	{
		parser.GetAttrNode(childNode, "Index", sIndex);
		parser.GetAttrNode(childNode, "Freq", Freq);
		string strXml = strOriginalXML;
		if ((strXml.find("beginTime") != std::string::npos)&& (strXml.find("endTime") != std::string::npos))
		{
			parser.GetAttrNode(childNode, "beginTime", StartDateTime);
			parser.GetAttrNode(childNode, "endTime", EndDateTime);
		}
		else if((strXml.find("BeginTime") != std::string::npos)&& (strXml.find("EndTime") != std::string::npos))
		{
			parser.GetAttrNode(childNode, "BeginTime", StartDateTime);
			parser.GetAttrNode(childNode, "EndTime", EndDateTime);
		}
		else if((strXml.find("StartDateTime") != std::string::npos)&& (strXml.find("EndDateTime") != std::string::npos))
		{
			pXMLNODE timechildNode = parser.GetNodeFirstChild(childNode);		
			parser.GetAttrNode(timechildNode, "StartDateTime", StartDateTime);
			parser.GetAttrNode(timechildNode, "EndDateTime", EndDateTime);
			parser.GetAttrNode(timechildNode, "Freq", Freq);
		}
		ReportTime = TimeUtil::GetCurDateTime();
		TaskID = Freq;

	}
	else if (TaskName == "QualityQuery")
	{
		pXMLNODE qualityQueryNode = parser.GetNodeFromPath("Msg/QualityQuery");
		pXMLNODE qualityNode = parser.GetNodeFirstChild(qualityQueryNode);

		parser.GetAttrNode(qualityNode,"TaskID",TaskID);                             //获取原始XML中Quality节点的TaskID属性值
		parser.GetAttrNode(qualityNode,"EquCode",EquCode);                           //获取原始XML中Quality节点的EquCode属性值
		parser.GetAttrNode(qualityNode,"STD",STD);                                   //获取原始XML中Quality节点的STD属性值
		parser.GetAttrNode(qualityNode,"Freq",Freq);                                 //获取原始XML中Quality节点的Freq属性值
		parser.GetAttrNode(qualityNode,"StartDateTime",StartDateTime);               //获取原始XML中Quality节点的StartDateTime属性值
		parser.GetAttrNode(qualityNode,"EndDateTime",EndDateTime);                   //获取原始XML中Quality节点的EndDateTime属性值
		parser.GetAttrNode(qualityNode,"ReportTime",ReportTime);                     //获取原始XML中Quality节点的ReportTime属性值
	}
	else 
	{
		return false;
	}

	pXMLNODE tempQualityQueryNode =tempParser.CreateNodePtr(tempRootNode,"QualityQuery");  //创建QualityQuery节点
	pXMLNODE tempQualityNode = tempParser.CreateNodePtr(tempQualityQueryNode,"Quality");   //创建Quality节点


	tempParser.SetAttrNode("Index", sIndex, tempQualityNode);
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
bool TranslateDownXMLForDVBC::TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (strType =="MonDown")
	{
		 strType = "DVBC";          //设置Msg节点的DVBType属性值
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
bool TranslateDownXMLForDVBC::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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


	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TableQuery></TableQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime,strScanDate;
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
		if(version=="3.0")
		{
			parser.GetAttrNode(orgTableNode,"ScanDate",strScanDate);
			pXMLNODELIST chilenodeList=parser.GetNodeList(orgTableNode);
			for(int j=0;j<chilenodeList->Size();j++)
			{
				pXMLNODE subTableNode=parser.GetNextNode(chilenodeList);
				std::string strNodeName = parser.GetNodeName(subTableNode);
				if(strNodeName == "DemodulateInfo")
				{
					string strChassisID,strPosition,TunerDeviceID,strChanId;
					parser.GetAttrNode( subTableNode,"ChassisID",strChassisID );
					parser.GetAttrNode( subTableNode,"Position",strPosition );
					parser.GetAttrNode( subTableNode,"Demodulate",strChanId );
					TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
					temParser.SetAttrNode( "DeviceID",TunerDeviceID,stdTabelNode );
				}
				else if(strNodeName == "FreqInfo")
				{
					string strFreq;
					parser.GetAttrNode( subTableNode,"Freq",strFreq );
					temParser.SetAttrNode( "Freq",strFreq,stdTabelNode );
				}
			}
			temParser.SetAttrNode( "ScanTime",strScanDate,stdTabelNode );
		}
		else
		{
			parser.GetAttrNode(orgTableNode,"EquCode",equcode);
			parser.GetAttrNode(orgTableNode,"STD",stdtype);
			parser.GetAttrNode(orgTableNode,"Freq",freq);
			parser.GetAttrNode(orgTableNode,"SymbolRate",rate);
			parser.GetAttrNode(orgTableNode,"QueryTime",querytime);
			//
			temParser.SetAttrNode("EquCode",equcode,stdTabelNode);
			temParser.SetAttrNode("STD",stdtype,stdTabelNode);
			temParser.SetAttrNode("Freq",freq,stdTabelNode);
			temParser.SetAttrNode("SymbolRate",rate,stdTabelNode);
			temParser.SetAttrNode("QueryTime",querytime,stdTabelNode);
		}
	}

	//保存到字符串
	if(version=="3.0")
	{
		strStandardXML = strOriginalXML;
	}
	else
	{
		temParser.SaveToString( strStandardXML);
	}
	return true;
}

//返回标准的运行图设置任务xml
bool TranslateDownXMLForDVBC::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if(!strcmp(downtype.c_str(),"MonDown"))
	{
		downtype="DVBC";
	}
	else if(!strcmp(downtype.c_str(),"DVBCDown"))
	{
		downtype="DVBC";
	}
	else if(!strcmp(downtype.c_str(),"DTMBDown"))
	{
		downtype="CTTB";
	}
	else if(!strcmp(downtype.c_str(),"DVBSDown"))
	{
		downtype="DVBS";
	}
	else if(!strcmp(downtype.c_str(),"RADIODown"))
	{
		downtype="RADIO";
	}
	else if(!strcmp(downtype.c_str(),"AMDown"))
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

	pXMLNODE stdRunSetNode=temParser.GetNodeFromPath("Msg/RunPlanSet");
	try
	{
		//jsp2017.7.25
		int runPlanCount = -1;
		pXMLNODELIST orgRunPlanList;
		if (version == "3.0")
		{
			pXMLNODE runningChartNode=parser.GetNodeFromPath("Msg/RunningChartSet");
			orgRunPlanList = parser.GetNodeList( runningChartNode );
			runPlanCount = parser.GetChildCount( runningChartNode );	
		}
		else
		{
			orgRunPlanList = parser.GetNodeList( orgRootNode );
			runPlanCount = parser.GetChildCount( orgRootNode );	//how many RunPlanSet node?
		}

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

				string Month,Day,DayOfWeek,StartTime,EndTime,StartDateTime,EndDateTime,RunPlanType,ValidStartDateTime,ValidEndDateTime,AlarmEndTime;

				if (version == "3.0")
				{
					if(parser.GetNodeName(timeParamNode)=="MonthInfo")
					{
						parser.GetAttrNode(timeParamNode,"Month",Month);
						parser.GetAttrNode(timeParamNode,"Day",Day);
						parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
						parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
					}
					else if(parser.GetNodeName(timeParamNode)=="WeekInfo")
					{
						parser.GetAttrNode(timeParamNode,"Week",DayOfWeek);
						parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
						parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
					}
					else if(parser.GetNodeName(timeParamNode)=="DayInfo")
					{
						parser.GetAttrNode(timeParamNode,"StartDateTime",StartDateTime);
						parser.GetAttrNode(timeParamNode,"EndDateTime",EndDateTime);
					}
					parser.GetAttrNode(timeParamNode, "ValidityBegin", ValidStartDateTime);
					parser.GetAttrNode(timeParamNode, "ValidityEnd", ValidEndDateTime);
				}
				else
				{
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
					parser.GetAttrNode(timeParamNode, "AlarmEndTime", ValidEndDateTime);
				}

				parser.GetAttrNode(timeParamNode, "Type", RunPlanType);
				pXMLNODE stdListNode = temParser.CreateNodePtr(stdRunSetNode, "RunPlanChannel");
				temParser.SetAttrNode("EquCode",EquCode,stdListNode);
				temParser.SetAttrNode("Freq",Freq,stdListNode);
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
				temParser.SetAttrNode("ValidEndDateTime",ValidEndDateTime,stdListNode);
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
//返回标准的运行图查询任务xml
bool TranslateDownXMLForDVBC::TranslateRunPlanCheck(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RunPlanCheckRequest/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="RunPlanCheckRequest";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

//返回标准的自动录像设置查询任务xml
bool TranslateDownXMLForDVBC::TranslateRecordSetQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AutoRecordChannelQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="RecordSetQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

//返回标准的手动录制任务xml
bool TranslateDownXMLForDVBC::TranslateManualRecordSet(const std::string &strOriginalXML, std::string &strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
		
#ifdef ZONG_JU_ZHAO_BIAO
		//*****************************总局入网
		std::string deviceid,time,path,freq,serviceid,lifecycle;
		parser.GetAttrNode(recordNode,"Index",deviceid);
		parser.GetAttrNode(recordNode,"Freq",freq);
		parser.GetAttrNode(recordNode,"ServiceID",serviceid);
		parser.GetAttrNode(recordNode,"Path",path);
		parser.GetAttrNode(recordNode,"Time",time);
		parser.GetAttrNode(recordNode,"Lifecycle",lifecycle);

		//总局时间格式为 00:00:00 需要计算为分钟
		long minute = 0;
		if (time.length() == 8)
		{
			minute = TimeUtil::StrToSecondTime(time) / 60;
		}

		//页面不下发index，靠freq和serviceId来寻找正在实时的通道
		if (!PROPMANAGER::instance()->IsDeviceAvaiable(StrUtil::Str2Int(deviceid),"StreamRealtimeQueryTask",DVBC)
			&& freq != "" && serviceid != "")
		{
			std::string channelId;
			CHANNELINFOMGR::instance()->GetChanIdByFreqServId(DVBC, freq, serviceid, channelId);

			std::list<int>taskList;
			PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeQueryTask", DVBC, taskList);
			std::list<int>::iterator taskIter = taskList.begin();
			for (; taskIter!=taskList.end(); taskIter++)
			{
				sTaskInfo tempTask;
				bool ret = BUSINESSLAYOUTMGR::instance()->QueryRunTaskInfo((*taskIter), tempTask);
				if (!ret)
					continue;

				if (tempTask.channelid == channelId)
				{
					deviceid = StrUtil::Int2Str(*taskIter);
					break;
				}
			}
		}

		temParser.SetAttrNode("DeviceID",deviceid,tempRecordNode);
		temParser.SetAttrNode("Time",minute,tempRecordNode);
		temParser.SetAttrNode("Path",path,tempRecordNode);
		temParser.SetAttrNode("Lifecycle",lifecycle,tempRecordNode);
		//***********************************
#else
		std::string deviceid,time,path;
		parser.GetAttrNode(recordNode,"Index",deviceid);
		parser.GetAttrNode(recordNode,"Time",time);
		parser.GetAttrNode(recordNode,"Path",path);
		temParser.SetAttrNode("DeviceID",deviceid,tempRecordNode);
		temParser.SetAttrNode("Time",time,tempRecordNode);
		temParser.SetAttrNode("Path",path,tempRecordNode);
#endif
		
	}
	temParser.SaveToString(strStandardXML);
	return true;

}
//返回标准的数据业务分析时间设置任务xml
bool TranslateDownXMLForDVBC::TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	string strNodePath = "Msg/AutoAnalysisTimeQuery";
	if(version == "3.0")
	{
		strNodePath = "Msg/AutoAnalysisTimeSet";
	}
	pXMLNODE  timeQueryNode = parser.GetNodeFromPath((char*)strNodePath.c_str());
	pXMLNODE  tempTimeQueryNode = temParser.GetNodeFromPath("Msg/AutoAnalysisTimeSet");
	pXMLNODELIST queryNodelist = parser.GetNodeList(timeQueryNode);     //获取AutoAnalysisTimeQuery的子节点列表
	int count = parser.GetChildCount(timeQueryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE timeNode = parser.GetNextNode(queryNodelist);
		pXMLNODE tempTimeNode = temParser.CreateNodePtr(tempTimeQueryNode,"AutoAnalysisTime");  //创建AutoAnalysisTime节点
		std::string starttime,type, scantype;
		parser.GetAttrNode(timeNode,"StartTime",starttime);
		parser.GetAttrNode(timeNode,"Type",type);
		parser.GetAttrNode(timeNode, "ScanType", scantype);
		temParser.SetAttrNode("StartTime",starttime,tempTimeNode);
		temParser.SetAttrNode("Type",type,tempTimeNode);
		temParser.SetAttrNode("ScanType", scantype, tempTimeNode);
		string strDevid;
		if(version == "3.0")
		{
			pXMLNODELIST devidNodelist = parser.GetNodeList(timeNode);     //获取AutoAnalysisTimeQuery的子节点列表
			int count = parser.GetChildCount(timeNode);
			if(count > 0)
			{
				string strChassisID,strPosition,strChanId;
				for (int i = 0;i<count;i++)
				{
					pXMLNODE devNode = parser.GetNextNode(devidNodelist);
					parser.GetAttrNode( devNode,"ChassisID",strChassisID );
					parser.GetAttrNode( devNode,"Position",strPosition );
					parser.GetAttrNode( devNode,"Demodulate",strChanId );
				}
				temParser.SetAttrNode("DeviceID", strChassisID+"-"+strPosition+"-"+strChanId, tempTimeNode);
			}
		}
	}
	temParser.SaveToString(strStandardXML);
	return true;
}

//返回标准的数据业务分析时间设置任务xml
bool TranslateDownXMLForDVBC::TranslateAutoAnalysisTimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AutoAnalysisTimeQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="AutoAnalysisTimeQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}

bool TranslateDownXMLForDVBC::TranslateEPGQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><EPGQuery></EPGQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downType,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="EPGQuery";
	string ScanTime, ScanType,Freq;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downType);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downType,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	string strChassisID,strPosition,TunerDeviceID,strChanId;
	if(version == "3.0")
	{
		pXMLNODE queryNode=parser.GetNodeFromPath("Msg/EPGScanQuery");
		parser.GetAttrNode( queryNode,"ScanDate",ScanTime );
		parser.GetAttrNode( queryNode,"ScanType",ScanType );
		//
		pXMLNODE desubchildnode=parser.GetNodeFromPath("Msg/EPGScanQuery/DemodulateInfo");
		parser.GetAttrNode( desubchildnode,"ChassisID",strChassisID );
		parser.GetAttrNode( desubchildnode,"Position",strPosition );
		parser.GetAttrNode( desubchildnode,"Demodulate",strChanId );
		TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
		//
		//pXMLNODE frsubchildnode=parser.GetNodeFromPath("Msg/EPGScanQuery/FreqInfo");
		//parser.GetAttrNode( frsubchildnode,"Freq",Freq );
	}
	else
	{
		pXMLNODE queryNode=parser.GetNodeFromPath("Msg/EPGQuery");
		parser.GetAttrNode( queryNode,"ScanTime",ScanTime );
	}
	//pXMLNODE retQueryNode=temParser.CreateNodePtr(stdRootNode,"EPGQuery");
	pXMLNODE retQueryNode=temParser.GetNodeFromPath("Msg/EPGQuery");
	temParser.SetAttrNode("ScanTime", ScanTime, retQueryNode);
	if(version == "3.0")
	{
		temParser.SetAttrNode( "DeviceID",TunerDeviceID,retQueryNode );
		temParser.SetAttrNode( "ScanType",ScanType,retQueryNode );
		temParser.SetAttrNode( "Freq",Freq,retQueryNode );
	}
	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForDVBC::TranslateMHPQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateSpectrumScan(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	string BandWidth, ReportInterval, ExpireTime, Action;

	pXMLNODE orgchildNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdchildNode=temParser.GetNodeFromPath("Msg/SpectrumScanQuery");
	//parser.GetAttrNode(orgchildNode,"ScanTime",ScanTime);
	//parser.GetAttrNode(orgchildNode,"STD",STD);
	//parser.GetAttrNode(orgchildNode,"SymbolRate",SymbolRate);
	//parser.GetAttrNode(orgchildNode,"QAM",QAM);
	parser.GetAttrNode(orgchildNode,"StartFreq",StartFreq);
	parser.GetAttrNode(orgchildNode,"EndFreq",EndFreq);
	parser.GetAttrNode(orgchildNode,"StepFreq",StepFreq);

	parser.GetAttrNode(orgchildNode,"BandWidth",BandWidth);
	parser.GetAttrNode(orgchildNode,"ReportInterval",ReportInterval);
	parser.GetAttrNode(orgchildNode,"ExpireTime",ExpireTime);
	parser.GetAttrNode(orgchildNode,"Action",Action);

	//temParser.SetAttrNode("ScanTime",ScanTime,stdchildNode);
	//temParser.SetAttrNode("STD",STD,stdchildNode);
	//temParser.SetAttrNode("SymbolRate",SymbolRate,stdchildNode);
	//temParser.SetAttrNode("QAM",QAM,stdchildNode);
	temParser.SetAttrNode("StartFreq",StartFreq,stdchildNode);
	temParser.SetAttrNode("EndFreq",EndFreq,stdchildNode);
	temParser.SetAttrNode("StepFreq",StepFreq,stdchildNode);

	temParser.SetAttrNode("BandWidth",BandWidth,stdchildNode);
	temParser.SetAttrNode("ReportInterval",ReportInterval,stdchildNode);
	temParser.SetAttrNode("ExpireTime",ExpireTime,stdchildNode);
	temParser.SetAttrNode("Action",Action,stdchildNode);

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回指标补偿标准xml
bool TranslateDownXMLForDVBC::TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downType,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="AlarmParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downType);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	eDVBType edvbType = UNKNOWN;
	if(!strcmp(downType.c_str(),"MonDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DVBCDown"))
	{
		downType="DVBC";
		edvbType = DVBC;
	}
	else if(!strcmp(downType.c_str(),"DTMBDown"))
	{
		downType="CTTB";
		edvbType = CTTB;
	}
	else if(!strcmp(downType.c_str(),"DVBSDown"))
	{
		downType="DVBS";
		edvbType = DVBS;
	}
	else if(!strcmp(downType.c_str(),"RADIODown"))
	{
		downType="RADIO";
		edvbType = RADIO;
	}
	else if(!strcmp(downType.c_str(),"AMDown"))
	{
		downType="AM";
		edvbType = AM;
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downType,stdRootNode);
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

	if (version == "3.0")
	{
		for(int n=0;n<count;n++)
		{
			pXMLNODE orgFreqNode =parser.GetNextNode(HoldList);
			pXMLNODE stdSetNode =temParser.GetNodeFirstChild(stdRootNode);
			string sTaskName=parser.GetNodeName(orgFreqNode);
			temParser.SetAttrNode("ReturnTaskName",sTaskName,FreqNode);
			int count1 = parser.GetChildCount(orgFreqNode);
			pXMLNODELIST FreqList = parser.GetNodeList(orgFreqNode);
			for(int m=0; m < count1; m++)
			{
				pXMLNODE orgSetNode = parser.GetNextNode(FreqList);
				string SetName=parser.GetNodeName(orgSetNode);
				pXMLNODELIST SetList=parser.GetNodeList(orgSetNode);
				parser.GetAttrNode(orgSetNode,"Freq",Freq);
				int isize = SetList->Size();
				for(int i=0;i<isize;i++)
				{
					string ServiceID,VideoPID,AudioPID,Tsid,Netid,Channelid;
					pXMLNODE HoldNode = parser.GetNextNode(SetList);
					parser.GetAttrNode(HoldNode,"ServiceID",ServiceID);
					CHANNELINFOMGR::instance()->GetOrgNetID(edvbType,Freq,AudioPID,VideoPID,ServiceID,Netid);
					CHANNELINFOMGR::instance()->GetTsID(edvbType,Freq,AudioPID,VideoPID,ServiceID,Tsid);
					CHANNELINFOMGR::instance()->GetChannelID(edvbType,Freq,ServiceID,Channelid);
					pXMLNODE stdHoldNode = temParser.CreateNodePtr(FreqNode,"AlarmParam");

					temParser.SetAttrNode("Freq",Freq,stdHoldNode);
					temParser.SetAttrNode("ChannelID",Channelid,stdHoldNode);

					string Type,Desc;
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

					string Switch,Duration;
					parser.GetAttrNode(HoldNode,string("Duration"),Duration);
					temParser.SetAttrNode("Duration",Duration,stdHoldNode);
					parser.GetAttrNode(HoldNode,string("Switch"),Switch);
					temParser.SetAttrNode("Switch",Switch,stdHoldNode);
					
				}
			}
		}
	}
	else
	{
		for(int n=0;n<count;n++)
		{
			pXMLNODE orgSetNode =parser.GetNextNode(HoldList);
			pXMLNODE stdSetNode =temParser.GetNodeFirstChild(stdRootNode);
			string sTaskName=parser.GetNodeName(orgSetNode);
			temParser.SetAttrNode("ReturnTaskName",sTaskName,FreqNode);
			parser.GetAttrNode(orgSetNode,"Freq",Freq);
			int count1 = parser.GetChildCount(orgSetNode);
			pXMLNODELIST SetList = parser.GetNodeList(orgSetNode);
			for(int m=0; m < count1; m++)
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

					CHANNELINFOMGR::instance()->GetOrgNetID(edvbType,Freq,AudioPID,VideoPID,ServiceID,Netid);
					CHANNELINFOMGR::instance()->GetTsID(edvbType,Freq,AudioPID,VideoPID,ServiceID,Tsid);
					CHANNELINFOMGR::instance()->GetChannelID(edvbType,Netid,Tsid,ServiceID,VideoPID,AudioPID,"",Channelid);
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

#ifdef ZONG_JU_ZHAO_BIAO
					if (Freq=="ALL" || Freq.length()<1)
					{
						Freq = "All";
					}
#else
					if(Freq.length()<1||Freq=="All")
					{
						Freq="All";
						Switch="1";
					}
#endif
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

#ifdef ZONG_JU_ZHAO_BIAO
					//总局入网测试，失锁不下发duration，给它默认值5秒
					if (Type == "1" && Duration.length()<1)
						Duration = "5";
#endif
					temParser.SetAttrNode("Duration",Duration,stdHoldNode);


					temParser.SetAttrNode("Switch",Switch,stdHoldNode);
				}

			}
		}
	}
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//返回报警参数查询标准xml
bool TranslateDownXMLForDVBC::TranslateAlarmParamQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamQuery></AlarmParamQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="AlarmParamQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE firstChileNode = parser.GetNodeFirstChild(orgRootNode);
	string sTaskName=parser.GetNodeName(firstChileNode);
	pXMLNODE alarmQueryNode=temParser.GetNodeFromPath("Msg/AlarmParamQuery");
	temParser.SetAttrNode("ReturnTaskName",sTaskName,alarmQueryNode);

	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}
//wz_110107
bool TranslateDownXMLForDVBC::TranslateAlarmReportSet( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
		strStandardXML.replace(firPos+1,7,"DVBC");
		size_t secPos=strStandardXML.find(" ",firPos+1);
		strStandardXML.insert(secPos+1,std::string("TaskType=\"AlarmTypeSet\" "));
	
	return true;
	*/
}

bool TranslateDownXMLForDVBC::TranslateAlarmQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateOSDSet( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateStreamRealtimeRate(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateRecordCapabilityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	std::string strRecordType;
	parser.GetAttrNode(CapabilityNode, "SetRecordType", strRecordType);		//录像任务类型
	temParser.SetAttrNode("SetRecordType", strRecordType, tempCapabilityNode);

	pXMLNODELIST channelInfolist = parser.GetNodeList(CapabilityNode);
	int channelInfoCount = 0;
	channelInfoCount = parser.GetChildCount(CapabilityNode);
	for (int i=0; i!=channelInfoCount; i++)
	{
		pXMLNODE channelInfoNode = parser.GetNextNode(channelInfolist);
		parser.GetAttrNode(channelInfoNode, "Freq", freq);
		parser.GetAttrNode(channelInfoNode, "ServiceID", serviceId);
		parser.GetAttrNode(channelInfoNode, "ProgramID", programId);
		
		pXMLNODE tempChanInfoNode = temParser.CreateNodePtr(tempCapabilityNode, "ChannelInfo");
		temParser.SetAttrNode("Freq", freq, tempChanInfoNode);
		temParser.SetAttrNode("ServiceID", serviceId, tempChanInfoNode);
		temParser.SetAttrNode("ProgramID", programId, tempChanInfoNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"DVBC");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"RecordCapabilityQuery\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForDVBC::TranslateHDDefAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	strStandardXML.replace(firPos+1,7,"DVBC");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"HDDefAudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForDVBC::TranslateAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	strStandardXML.replace(firPos+1,7,"DVBC");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"AudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForDVBC::TranslateClearAlarmState(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateMatrixQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
bool TranslateDownXMLForDVBC::TranslateChangeQAMQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

bool TranslateDownXMLForDVBC::TranslateLoopAlarmInfo(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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



bool TranslateDownXMLForDVBC::TranslateRecordParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

#ifdef ZONG_JU_ZHAO_BIAO
	pXMLNODELIST RecordParamList = parser.GetNodeList(orgRootNode);
	int RecordParamCount = parser.GetChildCount(orgRootNode);
#else
	pXMLNODE RecordParamSetNode = parser.GetNodeFromPath("Msg/RecordParamSet");
	pXMLNODELIST RecordParamList = parser.GetNodeList(RecordParamSetNode);
	int RecordParamCount = parser.GetChildCount(RecordParamSetNode);
#endif

	for (int i=0; i!=RecordParamCount; i++)
	{
		pXMLNODE paramNode = parser.GetNextNode(RecordParamList);
		parser.GetAttrNode(paramNode, "Type", paramType);
		parser.GetAttrNode(paramNode, "Width", width);
		parser.GetAttrNode(paramNode, "Height", height);
		parser.GetAttrNode(paramNode, "Fps", fps);
		parser.GetAttrNode(paramNode, "Bps", bps);

#ifdef ZONG_JU_ZHAO_BIAO
		bps = StrUtil::Int2Str( StrUtil::Str2Int(bps)/1000 );
#endif

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


bool TranslateDownXMLForDVBC::TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML)
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
	retparser.SetAttrNode("DVBType",string("DVBC"),retRootNode);               //设置Msg节点的DVBType属性值
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


#ifdef ZONG_JU_ZHAO_BIAO
bool TranslateDownXMLForDVBC::TranslateStopPlayingVideo( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="StopPlayingVideo";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	temParser.CreateNode(stdRootNode, "StopPlayingVideo");
	if(version == "3.0")
	{
		pXMLNODE tranNode = parser.GetNodeFromPath("Msg/StopPlayingVideo/TranscodeInfo");
		pXMLNODELIST CodNodeList=parser.GetNodeList(tranNode);
		string strCoderList;
		for(int i=0;i<CodNodeList->Size();++i)
		{
			string strChassisID,strPosition,strModuleID,strChanId;
			pXMLNODE CodchildNode=parser.GetNextNode(CodNodeList);
			parser.GetAttrNode( CodchildNode,"ChassisID",strChassisID );
			parser.GetAttrNode( CodchildNode,"Position",strPosition );
			parser.GetAttrNode( CodchildNode,"ModuleID",strModuleID );
			parser.GetAttrNode( CodchildNode,"Transcode",strChanId);
			string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
			if(strCoderList.size()>0)
			{
				strCoderList += ";";
				strCoderList += CoderDeviceID;
			}
			else
			{
				strCoderList = CoderDeviceID;
			}
		}
		temParser.SetAttrNode( "CoderList",strCoderList,stdRootNode );
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//小卡卡号查询任务
bool TranslateDownXMLForDVBC::TranslateICInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="ICInfoQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	temParser.CreateNode(stdRootNode, "ICInfoQuery");

	temParser.SaveToString(strStandardXML);
	return true;
}


//板卡(通道)设置任务
bool TranslateDownXMLForDVBC::TranslateNvrStatusSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="NvrStatusSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE NvrStaSetNode = parser.GetNodeFromPath("Msg/NvrStatusSet");
	int count = parser.GetChildCount(NvrStaSetNode);
	pXMLNODELIST NvrStatusList = parser.GetNodeList(NvrStaSetNode);
	pXMLNODE tempNvrStaSetNode = temParser.CreateNodePtr(stdRootNode, "NvrStatusSet");

	std::string index, indexType;
	for (int i=0; i<count; i++)
	{
		pXMLNODE NvrStatusNode = parser.GetNextNode(NvrStatusList);
		parser.GetAttrNode(NvrStatusNode, "Index",		index);
		parser.GetAttrNode(NvrStatusNode, "IndexType",	indexType);

		pXMLNODE tempNvrStaNode = temParser.CreateNodePtr(tempNvrStaSetNode, "NvrStatus");
		temParser.SetAttrNode("Index", index, tempNvrStaNode);
		temParser.SetAttrNode("IndexType", indexType, tempNvrStaNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}


//前端属性配置任务
bool TranslateDownXMLForDVBC::TranslateAgentInfoSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AgentInfoSet></AgentInfoSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="AgentInfoSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE stdAISNode = temParser.GetNodeFromPath("Msg/AgentInfoSet");

	pXMLNODE orgAISNode = parser.GetNodeFromPath("Msg/AgentInfoSet");
	int count = parser.GetChildCount(orgAISNode);
	pXMLNODELIST orgAISList = parser.GetNodeList(orgAISNode);

	string agenttype,agentdesc,agentvalue;
	for (int i=0; i<count; i++)
	{
		pXMLNODE orgAISRNode = parser.GetNextNode(orgAISList);
		parser.GetAttrNode(orgAISRNode, "Type", agenttype);
		parser.GetAttrNode(orgAISRNode, "Desc", agentdesc);
		parser.GetAttrNode(orgAISRNode, "Value", agentvalue);

		pXMLNODE stdAISRNode = temParser.CreateNodePtr(stdAISNode, "AgentInfo");
		temParser.SetAttrNode("AgentType", agenttype, stdAISRNode);
		temParser.SetAttrNode("AgentDesc", agentdesc, stdAISRNode);
		temParser.SetAttrNode("AgentValue", agentvalue, stdAISRNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}
//前端属性配置查询任务
bool TranslateDownXMLForDVBC::TranslateAgentInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AgentInfoQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="AgentInfoQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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
	//保存到字符串
	temParser.SaveToString( strStandardXML);
	return true;
}


//前端重启任务
bool TranslateDownXMLForDVBC::TranslateRebootSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="RebootSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	if(version == "3.0")
	{
		std::string Report,Type, ChassisID, Position, ModuleID, Index; 
		pXMLNODE pRebootNode=parser.GetNodeFromPath("Msg");
		pXMLNODELIST pRebootList = parser.GetNodeList(pRebootNode);
		for (int i=0; i<pRebootList->Size(); i++)
		{
			pXMLNODE pTypeNode = parser.GetNextNode(pRebootList);
			parser.GetAttrNode(pTypeNode,"Type",Type);
			parser.GetAttrNode(pTypeNode,"Report",Report);
			pXMLNODE pRebootSetNode=temParser.CreateNodePtr(stdRootNode,"RebootSet");
			temParser.SetAttrNode("Type",Type,pRebootSetNode);
			if(Type == "1" || Type == "2")
			{
				temParser.SetAttrNode("Report",Report,pRebootSetNode);
			}
			pXMLNODE pInfoNode = parser.GetNodeFirstChild(pTypeNode);
			parser.GetAttrNode(pInfoNode, "ChassisID", ChassisID);
			parser.GetAttrNode(pInfoNode, "Position", Position);
			parser.GetAttrNode(pInfoNode, "ModuleID", ModuleID);
			parser.GetAttrNode(pInfoNode, "Index", Index);
			temParser.SetAttrNode("ChassisID", ChassisID, pRebootSetNode);
			temParser.SetAttrNode("Position", Position, pRebootSetNode);
			temParser.SetAttrNode("ModuleID", ModuleID, pRebootSetNode);
			temParser.SetAttrNode("Index", Index, pRebootSetNode);
		}
	}
	else
	{
		std::string reboottype, rebootIndex, rebootReport;
		pXMLNODE orgRSNode = parser.GetNodeFromPath("Msg/RebootSet");
		parser.GetAttrNode(orgRSNode, "Type", reboottype);
		parser.GetAttrNode(orgRSNode, "Index", rebootIndex);
		parser.GetAttrNode(orgRSNode, "Report", rebootReport);
		pXMLNODE stdRSNode = temParser.GetNodeFromPath("Msg");
		pXMLNODE pRebootSetNode=temParser.CreateNodePtr(stdRSNode,"RebootSet");
		temParser.SetAttrNode("Type", reboottype, pRebootSetNode);
		temParser.SetAttrNode("Index", rebootIndex, pRebootSetNode);
		temParser.SetAttrNode("Report", rebootReport, pRebootSetNode);
	}
	temParser.SaveToString(strStandardXML);
	return true;
}

//板卡(通道)状态查看任务
bool TranslateDownXMLForDVBC::TranslateNvrStatusQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="NvrStatusQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	temParser.CreateNode(stdRootNode, "NvrStatusQuery");

	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForDVBC::TranslateGetRecVideoTime( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="GetRecordVideoTime";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE tempTimeNode = temParser.CreateNodePtr(stdRootNode, "GetRecordVideoTime");

	std::string strProgramId, strFreq, strServiceId,strDevID;
	string strChassisID,strPosition,strModuleID,strChanId;
	if(version == "3.0")
	{
		pXMLNODE chanNode = parser.GetNodeFromPath( "Msg/AutoRecordExpiryDate/ChannelInfo" );
		parser.GetAttrNode(chanNode, "Freq", strFreq);
		parser.GetAttrNode(chanNode, "ServiceID", strServiceId);
		pXMLNODE TranscodeNode = parser.GetNodeFirstChild( chanNode );
		pXMLNODELIST CodNodeList=parser.GetNodeList(TranscodeNode);
		string strCoderList;
		for(int i=0;i<CodNodeList->Size();++i)
		{
		
			pXMLNODE CodchildNode=parser.GetNextNode(CodNodeList);
			parser.GetAttrNode( CodchildNode,"ChassisID",strChassisID );
			parser.GetAttrNode( CodchildNode,"Position",strPosition );
			parser.GetAttrNode( CodchildNode,"ModuleID",strModuleID );
			parser.GetAttrNode( CodchildNode,"Transcode",strChanId);
			string CoderDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strModuleID),StrUtil::Str2Int(strChanId)));
			if(strCoderList.size()>0)
			{
				strCoderList += ";";
				strCoderList += CoderDeviceID;
			}
			else
			{
				strCoderList = CoderDeviceID;
			}
		}
		strDevID = 	strCoderList;
	}
	else
	{
		pXMLNODE infoNode = parser.GetNodeFromPath( "Msg/GetRecordVideoTime/RecordeVideoInfo" );
		parser.GetAttrNode(infoNode, "ProgramID", strProgramId);
		parser.GetAttrNode(infoNode, "Freq", strFreq);
		parser.GetAttrNode(infoNode, "ServiceID", strServiceId);
	}
	pXMLNODE tempInfoNode = temParser.CreateNodePtr(tempTimeNode, "RecordeVideoInfo");
	temParser.SetAttrNode("ProgramID", strProgramId, tempInfoNode);
	temParser.SetAttrNode("Freq", strFreq, tempInfoNode);
	temParser.SetAttrNode("ServiceID", strServiceId, tempInfoNode);
	temParser.SetAttrNode("ChassisID", strChassisID, tempInfoNode);
	temParser.SetAttrNode("Position", strPosition, tempInfoNode);
	temParser.SetAttrNode("ModuleID", strModuleID, tempInfoNode);
	temParser.SetAttrNode("Transcode", strChanId, tempInfoNode);
	temParser.SetAttrNode("DeviceID", strDevID, tempInfoNode);

	temParser.SaveToString(strStandardXML);
	return true;
}



bool TranslateDownXMLForDVBC::TranslateChanEncryptQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="ICInfoChannelEncryptQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE tempQueryNode = temParser.CreateNodePtr(stdRootNode, "ICInfoChannelEncryptQuery");

	pXMLNODE ICQueryNode;
	if(version == "3.0")
	{
		ICQueryNode = parser.GetNodeFromPath("Msg/ICCardEncryptQuery");
	}
	else
	{
		ICQueryNode = parser.GetNodeFromPath("Msg/ICInfoChannelEncryptQuery");
	}
	int nICInfoCount = parser.GetChildCount(ICQueryNode);
	pXMLNODELIST infoNodeList = parser.GetNodeList(ICQueryNode);
	for (int nIndex=0; nIndex<nICInfoCount; nIndex++)
	{
		pXMLNODE infoNode = parser.GetNextNode( infoNodeList );
		std::string strCardNum;
		if(version == "3.0")
		{
			parser.GetAttrNode(infoNode, "ICCardNO", strCardNum);
		}
		else
		{
			parser.GetAttrNode(infoNode, "CardNO", strCardNum);
		}

		pXMLNODE tempInfoNode = temParser.CreateNodePtr(tempQueryNode, "ICInfo");
		temParser.SetAttrNode("CardNo", strCardNum, tempInfoNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}
#endif
//返回 红外遥控器 虚拟键 标准XML
bool TranslateDownXMLForDVBC::TranslateInfraredRemoteControlQuery(
	const std::string& strOriginalXML, std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="InfraredRemoteControlQuery";
	string sVirtualKey;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	pXMLNODE queryNode;
	if(version == "3.0")
	{
		queryNode=parser.GetNodeFromPath("Msg/STBVirtualController");
	}
	else
	{
		queryNode=parser.GetNodeFromPath("Msg/SendVirtualKey");
	}
	parser.GetAttrNode(queryNode,"Key",sVirtualKey);

	pXMLNODE retQueryNode=temParser.CreateNodePtr(stdRootNode,"InfraredRemoteControlQuery");
	temParser.SetAttrNode("VirtualKey",sVirtualKey,retQueryNode);

	temParser.SaveToString(strStandardXML);

	return true;
}

//返回 OSD设置 标准XML
bool TranslateDownXMLForDVBC::TranslateOSDFormatSetQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string tasktype="OSDFormatSet";
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	string Freq, ServiceID, FontSize, Position, AntiColor, Type, Align; 
	pXMLNODE formatNode=parser.GetNodeFromPath("Msg");
	pXMLNODELIST pformatList = parser.GetNodeList(formatNode);
	for (int i=0; i<pformatList->Size(); i++)
	{
		pXMLNODE queryNode = parser.GetNextNode(pformatList);
		parser.GetAttrNode(queryNode,"Freq",Freq);
		pXMLNODE retQueryNode=temParser.CreateNodePtr(stdRootNode,"OSDFormatSet");
		temParser.SetAttrNode("Freq",Freq,retQueryNode);

		pXMLNODELIST pProList = parser.GetNodeList(queryNode);
		for (int j=0; j<pProList->Size(); j++)
		{
			pXMLNODE pChildNode = parser.GetNextNode(pProList);
			parser.GetAttrNode(pChildNode, "ServiceID", ServiceID);
			parser.GetAttrNode(pChildNode, "FontSize", FontSize);
			parser.GetAttrNode(pChildNode, "Position", Position);
			parser.GetAttrNode(pChildNode, "AntiColor", AntiColor);
			parser.GetAttrNode(pChildNode, "Type", Type);
			parser.GetAttrNode(pChildNode, "Align", Align);

			pXMLNODE stdChildNode = temParser.CreateNodePtr(retQueryNode, "Program");
			temParser.SetAttrNode("ServiceID", ServiceID, stdChildNode);
			temParser.SetAttrNode("FontSize", FontSize, stdChildNode);
			temParser.SetAttrNode("Position", Position, stdChildNode);
			temParser.SetAttrNode("AntiColor", AntiColor, stdChildNode);
			temParser.SetAttrNode("Type", Type, stdChildNode);
			temParser.SetAttrNode("Align", Align, stdChildNode);
		}
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//返回 OSD设置查询 标准XML
bool TranslateDownXMLForDVBC::TranslateOSDFormatQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><OSDFormatQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//获取根元素
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	string tasktype="OSDFormatQuery";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
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

	temParser.SaveToString(strStandardXML);
	return true;
}
//返回 前端信息查询 标准XML
bool TranslateDownXMLForDVBC::TranslateGetAgentInfoQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="GetAgentInfo";
	if((downtype=="MonDown")||(downtype=="MONDown"))
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateCardDevStatusQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="DevStatus";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateSetCardStatusTask(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="SetCardStatus";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateCameraQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="CameraQuery";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateEquipmentHeartBeatQuery(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="EquipmentHeartBeatQuery";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateAutoRecordUploadTask(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="AutoRecordUploadTask";
	if(downtype=="DVBCDown")
	{
		downtype="DVBC";
	}
	else if(downtype=="DVBSDown")
	{
		downtype="DVBS";
	}
	else if(downtype=="DTMBDown")
	{
		downtype="CTTB";
	}
	else if(downtype=="AMDown")
	{
		downtype="AM";
	}
	else if(downtype=="RADIODown")
	{
		downtype="RADIO";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
bool TranslateDownXMLForDVBC::TranslateLimitedBandwidth(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="LimitedBandwidth";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForDVBC::TranslateControlCardTimeQueryTask(const std::string& strOriginalXML, std::string& strStandardXML)
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="ControlCardTimeQuery";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForDVBC::TranslateServerStatusSet( const std::string& strOriginalXML,std::string& strStandardXML )
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
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);   
	retparser.SetAttrNode("DVBType",string("DVBC"),retRootNode);   
	retparser.SetAttrNode("Type",string("DVBCDown"),retRootNode);               //设置Msg节点的DVBType属性值
	retparser.SetAttrNode("TaskType",string("ServerStatusSet"),retRootNode);   //设置Msg节点的TaskType属性值
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //设置Msg节点的Protocol属性值
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //设置Msg节点的DateTime属性值
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //设置Msg节点的StrCode属性值
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //设置Msg节点的DstCode属性值
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //设置Msg节点的SrcUrl属性值
	retparser.SetAttrNode("Priority",strPriority,retRootNode); 


	std::string EquCode,Action;
	pXMLNODE ServerNode = parser.GetNodeFromPath("Msg/ServerStatusSet");
	pXMLNODE TaskNode =parser.findNode(ServerNode,"Task");
	if(TaskNode==NULL)
	{
		strStandardXML=strOriginalXML;	
		return true;
	}
	parser.GetAttrNode(TaskNode,"Devindex",EquCode);
	parser.GetAttrNode(TaskNode,"Action",Action);

	pXMLNODE retNode=retparser.CreateNodePtr(retRootNode,"ServerStatusSet");

	retparser.SetAttrNode("Devindex",EquCode,retNode);
	retparser.SetAttrNode("Action",Action,retNode);

	pXMLNODELIST NodeList=parser.GetNodeList(TaskNode);

	int count=parser.GetChildCount(TaskNode);

	for(int i=0;i<count;i++)
	{
		pXMLNODE ChildNode=parser.GetNextNode(NodeList);

		if(parser.GetNodeName(ChildNode)=="CycleRecordTime")
		{
			std::string DayOfWeek,StartTime;
			parser.GetAttrNode(ChildNode,"DayOfWeek",DayOfWeek);
			parser.GetAttrNode(ChildNode,"StartTime",StartTime);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"CycleRecordTime");

			retparser.SetAttrNode("DayOfWeek",DayOfWeek,ParamNode);
			retparser.SetAttrNode("StartTime",StartTime,ParamNode);
		}
		else if(parser.GetNodeName(ChildNode)=="SingleReportTime")
		{
			std::string StartDateTime;
			parser.GetAttrNode(ChildNode,"StartDateTime",StartDateTime);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"SingleReportTime");

			retparser.SetAttrNode("StartDateTime",StartDateTime,ParamNode);
		}
		else if(parser.GetNodeName(ChildNode)=="UpdateFile")
		{
			std::string URL;
			parser.GetAttrNode(ChildNode,"URL",URL);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"UpdateFile");

			retparser.SetAttrNode("URL",URL,ParamNode);

		}
	}
	retparser.SaveToString(strStandardXML);	
	return true;
}

bool TranslateDownXMLForDVBC::TranslateDevStatusQueryTask( const std::string& strOriginalXML,std::string& strStandardXML )
{
	XmlParser parser(strOriginalXML.c_str());
	pXMLNODE orgRootNode=parser.GetRootNode();
	string downtype;
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	string tasktype="DevStatusQuery";
	if(downtype=="MonDown")
	{
		downtype="DVBC";
	}
	else
	{
		return false;
	}
	parser.SetAttrNode("DVBType",downtype,orgRootNode);
	parser.SetAttrNode("TaskType",tasktype,orgRootNode);
	parser.SaveToString(strStandardXML);
	return true;
}
