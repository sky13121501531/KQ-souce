
#include "EPGQueryTask.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/Config.h"
#include "../Foundation/TypeDef.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"

EPGQuery::EPGQuery() : DeviceIndependentTask()
{
	
}

EPGQuery::EPGQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/EPGQuery");
	parser.GetAttrNode(queryNode,"ScanTime",m_strScanTime);           
}

EPGQuery::~EPGQuery()
{

}

string EPGQuery::GetTaskName()
{
	return std::string("EPG查询任务");
}

void EPGQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]EPG查询任务执行 !\n",DeviceID));

	bRun = true;
	SetRunning();
	std::string strFileName = "EPGQuery.xml";    //EPG文件名

	std::string strTablePath;
	PROPMANAGER::instance()->GetXmlTablePath(GetDVBType(), strTablePath);
	std::string strFullName = strTablePath + strFileName;
	cout<<"EPG Query FullName:"<<strFullName<<endl;
	XmlParser epgParser;
	epgParser.LoadFromFile( strFullName.c_str() );

	pXMLNODE rootNode = epgParser.GetRootNode();
	std::string strEpgXml;
	//从任务中获取MsgId等信息;同时MsgId必须和页面下发的MsgId相同;
	epgParser.SetAttrNode( "Version",this->GetVersion(),rootNode );//版本
	epgParser.SetAttrNode( "MsgID",this->GetMsgID(),rootNode );//消息id属性
	std::string strType;
	if (this->GetDVBType() == DVBC)
		strType = "DVBCUp";
	else if (this->GetDVBType() == CTTB)
		strType = "DTMBUp";
	else if (this->GetDVBType() == DVBS)
		strType = "DVBSUp";
	else
		strType = "UnKnown";
	epgParser.SetAttrNode( "Type",strType,rootNode );//消息类型
	epgParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),rootNode );//当前时间
	epgParser.SetAttrNode( "SrcCode",this->GetDstCode(),rootNode );//本机标识，可通过接口获得
	epgParser.SetAttrNode( "DstCode",this->GetSrcCode(),rootNode );//目标机器标识
	epgParser.SetAttrNode( "DstURL",strSrcURL,rootNode);//目标URL
	epgParser.SetAttrNode( "ReplyID",this->GetMsgID(),rootNode );//回复的消息id

	epgParser.SaveToString( strEpgXml );
	//除掉&和;
	while(true)
	{
		if(strEpgXml.find("&")!=-1||strEpgXml.find(";")!=-1)
		{
			int posdz = strEpgXml.find("&");
			if(posdz != -1)
			{
				strEpgXml.replace(posdz,1,"A");
			}
			int posfh = strEpgXml.find(";");
			if(posfh != -1)
			{
				strEpgXml.replace(posfh,1,"B");
			}
		}
		else
		{
			break;
		}
	}
	//
	XmlParser validParser( strEpgXml.c_str() );
	validParser.SaveAsFile("C:/vscttb/EpgLastReport.xml");
	string temfile = OSFunction::GetStrDVBType(this->GetDVBType())+"_"+"EpgLastReport"+TimeUtil::DateTimeToString(time(0))+".zip";
	string path = "c:\\vscttb\\temp\\";
	OSFunction::CompressFile(path+temfile,"C:/vscttb/EpgLastReport.xml");
	string RequireURL =  "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
			PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + temfile;
	SetRetValue(RUN_SUCCESS);
	SendXML(TranslateUpXML::TranslateEPGQuery(this,RequireURL));
	//
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]EPG查询任务停止 !\n",DeviceID));
}

std::string EPGQuery::GetObjectName()
{
	return std::string("EPGScanQuery");
}

string EPGQuery::CreateEpgScanRet()
{
	string retXml;
	string roundurl;
	PROPMANAGER::instance()->GetUpStreamRoundUrl(DVBC,roundurl);
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg
	headParser.SetAttrNode( "Version",this->GetVersion(),headRootNode );//版本
	headParser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode );//消息id属性
	headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//消息类型
	headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//当前时间
	headParser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode );//本机标识，可通过接口获得
	headParser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode );//目标机器标识
	headParser.SetAttrNode( "DstURL",roundurl,headRootNode);//目标URL
	headParser.SetAttrNode( "ReplyID",string("-1"),headRootNode );//回复的消息id
	headParser.SaveToString( retXml );	
	//
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE channelScanQueryNode=parser.CreateNodePtr(parser.GetRootNode(),"ScanNoticeReport");//为返回的xml创建ChannelScanReport节点
	std::string scantime = TimeUtil::GetCurDateTime();
	parser.SetAttrNode("Type",string("2"),channelScanQueryNode);
	parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
	parser.SaveToString(retXml);
	cout<<"EPGQuery::CreateEpgScanRet():"<<retXml<<endl;
	return retXml;
}