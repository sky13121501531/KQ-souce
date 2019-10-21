


#include "./ChanEncryptQueryTask.h"
#include "./TranslateUpXML.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/ChanEncryptCheckMgr.h"
#include "../Foundation/ParamSetManager.h"
#include "./EncryptCheckTask.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"



ChanEncryptQueryTask::ChanEncryptQueryTask() : DeviceIndependentTask()
{

}


ChanEncryptQueryTask::ChanEncryptQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}


ChanEncryptQueryTask::~ChanEncryptQueryTask()
{

}


void ChanEncryptQueryTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡节目信息授权查询任务执行 !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	//
	CHANENCRYPTCHECKMGR::instance()->ClearEncryptInfo();
	//转化任务xml
	std::vector<std::string> vecEncryptXml;
	TranslateEncryptXml( vecEncryptXml );

	RetValue = RUN_SUCCESS;
	for (int n=0; n<vecEncryptXml.size(); n++)
	{
		EncryptCheckTask* pTask = new EncryptCheckTask( vecEncryptXml[n] );
		if (pTask != NULL)
		{
			bool bRet = BUSINESSLAYOUTMGR::instance()->AddTask( pTask );
			if (!bRet)
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 添加授权检测任务失败 !\n"));
				RetValue = RUN_FAILED;	//如果有一个任务没有添加成功，就认为小卡授权查询任务失败
				break;
			}
		}
	}

	//先发送返回信息，确认与页面系统交互过程成功
	SendXML( TranslateUpXML::TranslateChanEncryptQuery(this) );//发送xml

	if (RetValue == RUN_FAILED)
	{
		SetFinised(); 

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡节目信息授权查询任务停止 !\n",DeviceID));
		return;
	}

	//等待小卡授权检测完成
	std::string strStartTime = TimeUtil::GetCurDateTime();
	while (true)
	{
		std::string strCurTime = TimeUtil::GetCurDateTime();
		if (TimeUtil::DiffMinute(strCurTime,strStartTime) >= 120)  //FRee_fanrong_20131224 原来取的90
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡节目信息授权查询任务超时退出 !\n",DeviceID));
			break;
		}
		//如果已经检测完了所有小卡的授权
		if ( CHANENCRYPTCHECKMGR::instance()->GetMapSize() == vecEncryptXml.size() )
		{
			break;
		}

		OSFunction::Sleep( 10 );	//10s 检测一次
	}

	//返回小卡授权的检测结果
	SendXML( TranslateUpXML::TranslateEncryptResult(this) );//发送xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]小卡节目信息授权查询任务停止 !\n",DeviceID));
	return;
}


std::string ChanEncryptQueryTask::GetObjectName()
{
	return std::string("ChanEncryptQueryTask");
}


std::string ChanEncryptQueryTask::GetTaskName()
{
	return std::string("小卡节目信息授权查询任务");
}

bool ChanEncryptQueryTask::TranslateEncryptXml( std::vector<std::string>& vecTaskXml )
{
	//录像标准的XML结构
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";

	XmlParser parser( strStandardXML.c_str());
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
	tempParser.SetAttrNode( "TaskType",string("EncryptCheckTask"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",std::string("9999"),tempRootNode );	//将这个任务的优先级调到最大

	pXMLNODE tempQueryNode = tempParser.CreateNodePtr(tempRootNode, "EncryptCheckTask");
	
	int nListenPort = 10086;
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/ICInfoChannelEncryptQuery");
	pXMLNODELIST infoNodeList = parser.GetNodeList( queryNode );
	int nInfoCount = parser.GetChildCount( queryNode );
	for (int nIndex=0; nIndex<nInfoCount; nIndex++)
	{
		pXMLNODE infoNode = parser.GetNextNode( infoNodeList );

		std::string strCardNo;
		parser.GetAttrNode(infoNode, "CardNo", strCardNo);

		//获取对应小卡的相关信息
		CACardInfo cardinfo;
		if(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0")
		{
			PARAMSETMANAGER::instance()->GetCACardInfoByCardNo(strCardNo, cardinfo);
		}
		else
		{
			if(!(PROPMANAGER::instance()->GetCACardInfoByCardNo(strCardNo, cardinfo)))
			{
				cout<<"小卡授权查询卡号不存在:"<<strCardNo<<endl;
			}
		}

		pXMLNODE tempInfoNode = tempParser.CreateNodePtr(tempQueryNode, "ICInfo");
		tempParser.SetAttrNode("CardNo", strCardNo, tempInfoNode);
		tempParser.SetAttrNode("Position", cardinfo.position, tempInfoNode);
		tempParser.SetAttrNode("Desc", cardinfo.desc, tempInfoNode);
		tempParser.SetAttrNode("Ip", cardinfo.ip, tempInfoNode);
		tempParser.SetAttrNode("ListenPort", nListenPort++, tempInfoNode);
		//
		int devnum = PROPMANAGER::instance()->GetCoderDevNum();
		devnum += PROPMANAGER::instance()->GetQuaDevInfoNum()*2;
		tempParser.SetAttrNode("DeviceId", StrUtil::Int2Str(devnum), tempInfoNode);//使用管理线程的最后一个线程

		std::string strTempXml;
		tempParser.SaveToString( strTempXml );
		vecTaskXml.push_back( strTempXml );
		tempParser.DeleteNode(tempQueryNode, tempInfoNode);
	}

	return true;
}