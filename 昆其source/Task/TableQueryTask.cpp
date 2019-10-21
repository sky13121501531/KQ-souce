#include "TableQueryTask.h"
#include "ace/Task.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"
TableQueryTask::TableQueryTask(void)
{
}

TableQueryTask::TableQueryTask( std::string strXML ) : DeviceIndependentTask(strXML)
{
}
TableQueryTask::~TableQueryTask(void)
{
}

void TableQueryTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]表查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning();
	
	std::string strFileName;
	std::string STD,Freq,SymbolRate;
	std::string strScanTime,Devid;
    XmlParser parser(strStandardXML.c_str());
	if(this->GetVersion() == "3.0")
	{
		pXMLNODE orgRootNode=parser.GetRootNode();
		pXMLNODELIST nodeList=parser.GetNodeList(orgRootNode);
		for(int i=0;i<nodeList->Size();++i)
		{
			pXMLNODE orgTableNode=parser.GetNextNode(nodeList);
			parser.GetAttrNode(orgTableNode,"ScanDate",strScanTime);
			pXMLNODELIST chilenodeList=parser.GetNodeList(orgTableNode);
			for(int j=0;j<chilenodeList->Size();j++)
			{
				pXMLNODE subTableNode=parser.GetNextNode(chilenodeList);
				std::string strNodeName = parser.GetNodeName(subTableNode);
				if(strNodeName == "DemodulateInfo")
				{
					string strChassisID,strPosition,strChanId;
					parser.GetAttrNode( subTableNode,"ChassisID",strChassisID );
					parser.GetAttrNode( subTableNode,"Position",strPosition );
					parser.GetAttrNode( subTableNode,"Demodulate",strChanId );
					Devid = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
				}
				else if(strNodeName == "FreqInfo")
				{
					parser.GetAttrNode( subTableNode,"Freq",Freq );
				}
			}
		}
		//
		if(strScanTime.size() > 0)
		{
			//
			string Path;
			PROPMANAGER::instance()->GetXmlTablePath(DVBC,Path);
			string tablequeryfilename = Path + string("TableQuery.xml");
			//
			string strRtnXml;
			XmlParser parser;
			parser.LoadFromFile(tablequeryfilename.c_str());
			parser.Get_xml(strRtnXml); //读取频道扫描结果
			if (strRtnXml.empty())
			{
				SetRetValue(RUN_FAILED);
				cout<<"操作文件异常:"<<strFileName.c_str()<<endl;
			}
			else
			{
				pXMLNODE headRootNode = parser.GetNodeFromPath( "Msg" );//根节点msg
				parser.SetAttrNode( "Version",this->GetVersion(),headRootNode);//目标URL
				parser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode);//目标URL
				parser.SetAttrNode( "Type",string("MonUp"),headRootNode);//目标URL
				parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode);//目标URL
				parser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode);//目标URL
				parser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode);//目标URL
				parser.SetAttrNode( "ReplyID",this->GetMsgID(),headRootNode);//目标URL
				parser.SetAttrNode( "DstURL",this->GetSrcURL(),headRootNode);//目标URL
				parser.Get_xml(strRtnXml); //读取频道扫描结果
				SetRetValue(RUN_SUCCESS);
			}
			SendXML(strRtnXml);
		}
		else
		{
			if(this->GetVersion() == "3.0")
			{
				CHANSCANTHREAD::instance()->SetChanScanNow(strStandardXML,Freq,Devid);
			}
		}
	}
	else
	{
		pXMLNODE tableNode = parser.GetNodeFromPath("Msg/TableQuery/Table");
		parser.GetAttrNode( tableNode,"STD",STD );						//获取STD属性值
		parser.GetAttrNode( tableNode,"Freq",Freq );					//获取Freq属性值
		parser.GetAttrNode( tableNode,"SymbolRate",SymbolRate );		//获取SymbolRate属性值
		//获取TABLE的路径
		OSFunction::GetFileName(DVBType,GetObjectName(),strFileName);
		RetValue = RUN_SUCCESS;

		SendXML(TranslateUpXML::TranslateTableQuery(this,strFileName)); //发送到前端
	}
	
	SetFinised();

	bRun = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]表查询任务停止 !\n",DeviceID));
}
bool TableQueryTask::TableQuery(const std::string STD, const std::string Freq, const std::string SymbolRate, std::string &strFileName)
{
	strFileName = "Table.rar";
	return true;
}

std::string TableQueryTask::GetObjectName()
{
	return"TableQueryTask";
}

std::string TableQueryTask::GetTaskName()
{
	return "表查询任务";
}

string TableQueryTask::CreatePreTableQueryRet()
{
	string retXml;
	if(this->GetVersion() == "3.0")
	{
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser headParser(cheader);
		pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg
		headParser.SetAttrNode( "Version",this->GetVersion(),headRootNode );//版本
		headParser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode );//消息id属性
		headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//消息类型
		headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//当前时间
		headParser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode );//本机标识，可通过接口获得
		headParser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode );//目标机器标识
		headParser.SetAttrNode( "DstURL",this->GetSrcURL(),headRootNode);//目标URL
		headParser.SetAttrNode( "ReplyID",this->GetMsgID(),headRootNode );//回复的消息id
		headParser.SaveToString( retXml );	
		//
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(parser.GetRootNode(),"TableScanQueryNoticeReport");//为返回的xml创建ChannelScanReport节点
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("Type",string("1"),channelScanQueryNode);
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
	}
	return retXml;
}