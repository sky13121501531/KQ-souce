
#include "ChannelScanQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "Scheduler.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DBAccess/DBManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../Foundation/XmlParser.h"
#include "./TranslateXMLForDevice.h"
#include "../Foundation/OSFunction.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"
using namespace std;

ChannelScanQueryTask::ChannelScanQueryTask() : DeviceIndependentTask()
{

}
//ScanTime为空表示即时采集,需回复频道扫描预告,再来采集(补采)
//ScanTime不为空表示补集
ChannelScanQueryTask::ChannelScanQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/ChannelScanQuery/ChannelScan");
	parser.GetAttrNode(queryNode,"ScanTime",m_strScanTime);   
	//***多信号源仅支持获取现有频道信息***
	m_strScanTime = "2013-08-17 15:30:00";
	//************************************
	if(this->GetVersion() == "3.0")
	{
		string strScanType;
		parser.GetAttrNode(queryNode,"ScanType",strScanType);           
		//
		parser.GetAttrNode(queryNode,"DeviceID",m_strDevID);           
		parser.GetAttrNode(queryNode,"Freq",m_strFreq);  
		DeviceID = StrUtil::Str2Int(m_strDevID);
		m_strFreq = StrUtil::Long2Str(StrUtil::Str2Long(m_strFreq)/1000);//为了和配置文件配置单位M一致,在ChanScanThread中*1000

		if(strScanType != "3")
		{
			m_strFreq = "All";
		}
	}
}

ChannelScanQueryTask::~ChannelScanQueryTask()
{

}

void ChannelScanQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道扫描任务执行 !\n",DeviceID));
	bRun = true;
	SetRunning();
	string strRtnXml; //XML文件内容
	//数字频道扫描
	//******************************
	if(m_strScanTime.size() > 0)
	{
		string strFileName;  //频道扫描结果文件路径
		OSFunction::GetFileName(DVBType,GetObjectName(),strFileName);
		//strFileName = "C:\\table\\dvbc\\ChannelScanQuery.xml";
		XmlParser parser;
		parser.LoadFromFile(strFileName.c_str());
		parser.Get_xml(strRtnXml); //读取频道扫描结果
		if (strRtnXml.empty())
		{
			SetRetValue(RUN_FAILED);
			cout<<"操作文件异常:"<<strFileName.c_str()<<endl;
		}
		else
		{
			SetRetValue(RUN_SUCCESS);
			if(strRtnXml.size()>0)
			{
				parser.Set_xml(strRtnXml);
				pXMLNODE rtnChannelScanReportNode=parser.GetNodeFromPath("Msg/ChannelScanReport");//硬件返回xml的ChannelScanReport节点
				pXMLNODELIST nodeList=parser.GetNodeList(rtnChannelScanReportNode);//硬件返回xml的ChannelScanReport节点的子节点列表
				int serviceIDNum = 100;
				for (int j=0;j!=nodeList->Size();++j)
				{
					pXMLNODE childNode=parser.GetNextNode(nodeList);//硬件返回xml的ChannelScan节点的子节点列表
					pXMLNODELIST childNodeList=parser.GetNodeList(childNode);//硬件返回xml的ChannelScan节点的子节点列表
					for (int k=0;k!=childNodeList->Size();++k)
					{
						string ServiceID = "",ProgramID = "";
						pXMLNODE subChildNode=parser.GetNextNode(childNodeList);//硬件返回xml的ChannelScan节点的子节点channel
						parser.GetAttrNode(subChildNode,"ServiceID",ServiceID);//获得ServiceID属性值
						parser.GetAttrNode(subChildNode,"ProgramID",ProgramID);//获得ServiceID属性值
						serviceIDNum++;
						string TmpServiceID = StrUtil::Int2Str(serviceIDNum) + ServiceID;
						parser.SetAttrNode("ServiceID",TmpServiceID,subChildNode);//设置返回xml的Channel节点的ServiceID属性值
						parser.SetAttrNode("ProgramID",TmpServiceID,subChildNode);//设置返回xml的Channel节点的ServiceID属性值
					}
				}
				parser.SaveToString(strRtnXml);
			}
		}
		SendXML(TranslateUpXML::TranslateChannelScan(this,strRtnXml));//发送xml,上报的频道进行权限校正,数据库存的和目录存的都是标识清流和扰流得
		//更新频道扫描结果
		CHANNELINFOMGR::instance()->UpdateChannelInfo(GetDVBType(), strRtnXml);
	}
	else
	{
		if(this->GetVersion() == "3.0")
		{
			CHANSCANTHREAD::instance()->SetChanScanNow(CreatePreChanScanRet(),m_strFreq,m_strDevID);
		}
		else
		{
			CHANSCANTHREAD::instance()->SetChanScanNow(CreatePreChanScanRet());
		}
		SetRetValue(RUN_SUCCESS);
	}
	bRun = false;
	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

    SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]频道扫描任务停止 !\n",DeviceID));
}
string ChannelScanQueryTask::GetTaskName()
{
	return std::string("频道扫描任务");
}
std::string ChannelScanQueryTask::GetObjectName()
{
	return std::string("ChannelScanQueryTask");
}
string ChannelScanQueryTask::CreatePreChanScanRet()
{
	string retXml;
	if(this->GetVersion() == "3.0")
	{
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
		parser.SetAttrNode("Type",string("3"),channelScanQueryNode);
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
		cout<<"ChannelScanQueryTask::CreatePreChanScanRet():"<<retXml<<endl;
	}
	else
	{
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";
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
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("ChannelScanNotice"),retNode );
		headParser.SetAttrNode( "Value",string("0"),retNode );//return节点的value属性
		headParser.SetAttrNode( "Desc",string("成功"),retNode );//return节点的Desc属性
		headParser.SaveToString( retXml );	
		//
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(returnInfoNode,"ChannelScanNotice");//为返回的xml创建ChannelScanReport节点
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
		cout<<"ChannelScanQueryTask::CreatePreChanScanRet():"<<retXml<<endl;
	}
	return retXml;
}