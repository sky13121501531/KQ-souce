///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateUpXMLForRADIO.cpp
// 创建者：gaoxd
// 创建时间：2010-04-09
// 内容描述：将原子任务执行结果的XML转化为应用系统所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXMLForRADIO.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Task//StreamRealtimeQueryTask.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/QualityCompensationMgr.h"
#include "../Foundation/OSFunction.h"
#include <fstream>

//返回实时视频XML
std::string TranslateUpXMLForRADIO::TranslateStreamRealtimeQuery(const XMLTask* task)//任务类型需要继续修正
{
	std::string retXml=GetXmlHeader(task,"StreamRealtimeQuery");//标准的返回头xml
	std::string index, url;
	if (task->GetRetValue()==RUN_SUCCESS || task->GetRetValue()==PREFERENTIALTASK_USE)
	{
		index="0";
		string protocol;

		//wz_110309(修改接口)
		PROPMANAGER::instance()->GetStreamProtocol(RADIO,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + PROPMANAGER::instance()->GetRtspVideoPort() +"/"+ StrUtil::Int2Str(task->GetDeviceID());//视频播放地址
		}
		else
		{
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(task->GetDeviceID());//视频播放地址
		}

	}
	XmlParser retParser;
	retParser.Set_xml(retXml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"StreamRealtimeReport");//创建Stream节点
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"MediaStream");//创建Stream节点的子节点MediaStream节点

	retParser.SetAttrNode("Index",index,mediaStreamNode);//设置MediaStream节点的index属性
	retParser.SetAttrNode("URL",url,mediaStreamNode);//设置MediaStream节点的URL属性
	retParser.SaveToString(retXml);
	return retXml;
}
//返回录像设置XML
std::string TranslateUpXMLForRADIO::TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML;

	if(task->GetTaskID() == "0")
	{
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
	}
	else
	{
		retXML=GetXmlHeader(task,"TaskSet");
	}
	return retXML;
}
//自动录像视频流查询XML
std::string TranslateUpXMLForRADIO::TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"StreamSimpleQuery");//标准的返回头xml
	std::string index="0";

	XmlParser retParser;
	retParser.Set_xml(retXml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"StreamSimpleReport");//创建Stream节点

	retParser.SetAttrNode("StartDateTime",TimeUtil::GetCurDateTime(),streamNode);
	retParser.SetAttrNode("EndDateTime",TimeUtil::GetCurDateTime(),streamNode);

	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"MediaStream");//创建Stream节点的子节点MediaStream节点

	retParser.SetAttrNode("Index",index,mediaStreamNode);//设置MediaStream节点的index属性
	retParser.SetAttrNode("url",URL,mediaStreamNode);//设置MediaStream节点的URL属性
	retParser.SaveToString(retXml);
	return retXml;
}
//构造返回XML头信息
std::string TranslateUpXMLForRADIO::GetXmlHeader(const XMLTask* task,std::string retType)
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	string   Desc ="",Comment = "";
	eTaskRetStatus retValue=task->GetRetValue();//任务的执行结果

	switch(retValue)
	{
	case RUN_SUCCESS:
		Desc="成功";
		break;
	case RUN_FAILED:
		Desc="失败";
		break;
	case NOFILE_EXIST:
		Desc="文件不存在";
		break;
	case CHANNELID_UNAVAIABLE:
		Desc="频道非法";
		break;
	case DEVICEID_UNAVAIABLE:
		Desc="通道非法";
		break;
	case NODEVICEIDMATCH:
		Desc="没有通道匹配该任务";
		break;
	case PREFERENTIALTASK_USE:
		Desc="高优先级任务占用通道";
		break;
	case DATABASEERROR:
		Desc="操作数据库错误";
		break;
	case DEVICEERROR:
		Desc="操作硬件错误";
		break;
	case RECORD_DOWN_OUTOFRAND:
		Desc="录像时间超界限";
		break;
	default:
		Desc="失败";
		break;
	}

	std::string msgType;//消息类型
	if (task->GetDVBType()== RADIO)
		msgType="RadioUp";
	else
		msgType="UnKnow";

	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg

	headParser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//版本
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//消息id属性
	headParser.SetAttrNode( "Type",msgType,headRootNode );//消息类型
	headParser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,headRootNode );//当前时间
	headParser.SetAttrNode( "SrcCode",task->GetDstCode(),headRootNode );//本机标识，可通过接口获得
	headParser.SetAttrNode( "DstCode",task->GetSrcCode(),headRootNode );//目标机器标识
	headParser.SetAttrNode( "DstURL",task->GetSrcURL(),headRootNode);//目标URL
	headParser.SetAttrNode( "ReplyID",task->GetMsgID(),headRootNode );//回复的消息id

	pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
	headParser.SetAttrNode( "Type",retType,retNode );

	headParser.SetAttrNode( "Value",retValue,retNode );//return节点的value属性
	headParser.SetAttrNode( "Desc",Desc,retNode );//return节点的Desc属性
	headParser.SetAttrNode( "Comment",Comment,retNode );//return节点的Comment属性
	headParser.SaveToString( retXml );

	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateChannelScanQuery( const XMLTask* task,std::string strxml )
{
	std::string retXml=GetXmlHeader(task,"ChannelScanQuery");//标准的返回头xml
	if (task->GetRetValue() != RUN_SUCCESS  || strxml.empty())
	{
		return retXml;
	}
	XmlParser srcParser(retXml.c_str());
	pXMLNODE root=srcParser.GetRootNode();
	pXMLNODE reportNode=srcParser.CreateNodePtr(root,"ChannelScanReport");
	pXMLNODE srcChannelScanNode=srcParser.CreateNodePtr(reportNode,"ChannelScan");
	srcParser.SetAttrNode("Index",string("0"),srcChannelScanNode);
	XmlParser rtnParser(strxml.c_str());
	pXMLNODE ChannelScanNode=rtnParser.GetNodeFromPath("Msg/ChannelScan");
	pXMLNODELIST ScanNodelist=rtnParser.GetNodeList(ChannelScanNode);
	for (int k=0;k!=rtnParser.GetChildCount(ChannelScanNode);++k)
	{
		pXMLNODE childNode=rtnParser.GetNextNode(ScanNodelist);
		pXMLNODE node=srcParser.CreateNodePtr(srcChannelScanNode,"Channel");
		string freq;
		rtnParser.GetAttrNode(childNode,"Freq",freq);
		srcParser.SetAttrNode("Freq",freq,node);
	}
	srcParser.SaveToString(retXml);

	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateStreamRound( const XMLTask* task,std::string deviceID )
{
	std::string retXml;//返回的xml
	std::string retType="StreamRoundQuery";//硬件返回xml的return节点的类型和值

	std::string headerxml=GetXmlHeader(task,retType);//标准的返回头xml

	std::string index, url;
	if(task->GetRetValue()==5)
	{	
		XmlParser headParser;
		headParser.Set_xml(headerxml);
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("StreamRoundQuery"),retNode );
		headParser.SaveToString(retXml);
		return retXml;
	}
	if (task->GetRetValue()==0)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;
		//wz_110309(修改接口)
		PROPMANAGER::instance()->GetStreamProtocol(RADIO,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() +"/roundstream" ;//视频播放地址
		}
		else
		{
			//wz_0217
			int virtualid = 0;
			PROPMANAGER::instance()->GetVirDeviceId(RADIO, virtualid);
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(virtualid);	//视频播放地址
			//wz_0217
		}
	}
	XmlParser retParser;
	retParser.Set_xml(headerxml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"Stream");//
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"MediaStream");//
	

	retParser.SetAttrNode("Index",index,mediaStreamNode);//设置MediaStream节点的index属性
	retParser.SetAttrNode("URL",url,mediaStreamNode);//设置MediaStream节点的URL属性
	retParser.SaveToString(retXml);

	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateChannelSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChannelSet");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateAlarmParamSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityAlarmParamSet");//标准的返回头xml
	return retXml;
}
std::string TranslateUpXMLForRADIO::TranslateQualitySet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityReportTaskSet");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateSpectrumSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"SpectrumScanTaskSet");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateSpectrumQuery( const XMLTask* task,std::vector<eSpecInfo> vecSpecInfo)
{
	std::string headerxml=GetXmlHeader(task,"FreqScanQuery");
	XmlParser parser(headerxml.c_str());
	pXMLNODE rootNode = parser.GetRootNode();
	int count=vecSpecInfo.size();
	if(count>1000)
	{
		string strURL,sharepath;
		sharepath=PROPMANAGER::instance()->GetTempSharePath();
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort() + sharepath+"Spec_";
		time_t curtime;
		float freq;
		int status=0;
		float valu=0.0;
		string strtime=TimeUtil::DateTimeToString(time(0));
		string dstZipPath=PROPMANAGER::instance()->GetTempFileLoac()+"Spec_"+strtime+".zip";
		string srcFilePath=PROPMANAGER::instance()->GetTempFileLoac()+"Spec_"+strtime+".txt";
		strURL +=strtime+".zip";
		ofstream ofile(srcFilePath.c_str());

		for(vector<eSpecInfo>::iterator itr=vecSpecInfo.begin();itr!=vecSpecInfo.end();itr++)
		{
			freq=StrUtil::Str2Float(itr->freq);
			curtime=TimeUtil::StrToDateTime(itr->checktime);
			valu = StrUtil::Str2Float(itr->valu);
			status=StrUtil::Str2Int(itr->status);
			ofile<<curtime<<" "<<freq<<" "<<valu<<" "<<status<<" "<<endl;
		}

		ofile.close();
		OSFunction::CompressFile(dstZipPath,srcFilePath);
		pXMLNODE filenode=parser.CreateNodePtr(parser.GetRootNode(),"FreqScanReportFile");
		parser.SetAttrNode(string("URL"),strURL,filenode); //设置压缩数据存放地址
		parser.SetAttrNode(string("Size"),(int)count,filenode); //压缩数据文件大小
	}
	else
	{
		pXMLNODE ReportNode =parser.CreateNodePtr(rootNode,"FreqScanReport");

		for(int i=0;i<vecSpecInfo.size();i++)
		{
			pXMLNODE childNode =parser.CreateNodePtr(ReportNode,"ScanResult");
			parser.SetAttrNode("Freq",vecSpecInfo[i].freq,childNode);
			parser.SetAttrNode("Level",vecSpecInfo[i].valu,childNode);
			parser.SetAttrNode("Status",vecSpecInfo[i].status,childNode);
			parser.SetAttrNode("CheckTime",vecSpecInfo[i].checktime,childNode);
		}
	}
	string retXml;
	parser.SaveToString(retXml);
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string headerxml=GetXmlHeader(task,"TaskRecordQuery");
	XmlParser parser(headerxml.c_str());
	pXMLNODE report=parser.CreateNodePtr(parser.GetRootNode(),"TaskRecordReport");
	pXMLNODE TaskRecord =parser.CreateNodePtr(report,"TaskRecord");
	parser.SetAttrNode("Index",string("0"),TaskRecord);
	parser.SetAttrNode("EquCODE",string(""),TaskRecord);

	//新安播要求返回channelCode
	std::string strChannelId = task->GetChannelID();
	std::string strChannelCode;
	CHANNELINFOMGR::instance()->GetChanCodeByChanId(RADIO, strChannelId, strChannelCode);
	if (strChannelCode.length()<1)
	{
		strChannelCode = strChannelId;
	}
	parser.SetAttrNode("ChCode",strChannelCode,TaskRecord);

	parser.SetAttrNode("ChannelCode",string(""),TaskRecord);	//gxd 2011.02.22 适应不用的页面系统接口
	parser.SetAttrNode("TaskID",task->GetTaskID(),TaskRecord);
	pXMLNODE record=parser.CreateNodePtr(TaskRecord,"Record");
	parser.SetAttrNode("RecordID",100,record);
	parser.SetAttrNode("StartDateTime",startDateTime,record);
	parser.SetAttrNode("EndDateTime",endDateTime,record);
	parser.SetAttrNode("Size",100,record);
	parser.SetAttrNode("ExpireDays",PROPMANAGER::instance()->GetRecordExpiredays(task->GetDVBType()),record);
	
	parser.SetAttrNode("URL",URL,record);

	string retxml;
	parser.SaveToString(retxml);
	return retxml;
}

std::string TranslateUpXMLForRADIO::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string headxml = GetXmlHeader(task, "TaskRecordFileQuery");

	XmlParser UpXmlParser( headxml.c_str() );
	pXMLNODE ReportNode = UpXmlParser.CreateNodePtr(UpXmlParser.GetRootNode(), "TaskRecordFileQuery");
	pXMLNODE RecNode = UpXmlParser.CreateNodePtr(ReportNode, "TaskRecord");
	UpXmlParser.SetAttrNode("EquCode", std::string(""),RecNode);

	//新安播要求返回channelCode
	std::string strChannelId = task->GetChannelID();
	std::string strChannelCode;
	CHANNELINFOMGR::instance()->GetChanCodeByChanId(RADIO, strChannelId, strChannelCode);
	if (strChannelCode.length()<1)
	{
		strChannelCode = strChannelId;
	}
	UpXmlParser.SetAttrNode("ChCode", strChannelCode,RecNode);

	UpXmlParser.SetAttrNode("TaskID", task->GetTaskID(),RecNode);

	pXMLNODE UrlNode = UpXmlParser.CreateNodePtr(RecNode, "Record");
	UpXmlParser.SetAttrNode("Number", std::string("0"), UrlNode);
	UpXmlParser.SetAttrNode("RecordID", 100, UrlNode);
	UpXmlParser.SetAttrNode("StartDateTime", startDateTime, UrlNode);
	UpXmlParser.SetAttrNode("EndDateTime", endDateTime, UrlNode);
	UpXmlParser.SetAttrNode("Size", 100000, UrlNode);
	UpXmlParser.SetAttrNode("URL", URL, UrlNode);
	UpXmlParser.SetAttrNode("ExpireDays", PROPMANAGER::instance()->GetTempFileExpire(), UrlNode);

	string retxml;
	UpXmlParser.SaveToString(retxml);
	return retxml;
}

std::string TranslateUpXMLForRADIO::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
{
	std::string headerxml=GetXmlHeader(task,"QualityQuery");
	XmlParser parser(headerxml.c_str());
	pXMLNODE rootNode = parser.GetRootNode();

	size_t count=0;//指标测量的数目，多余1000条打包
	std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();
	for (;ptr!=FreqQualityInfo.end();++ptr)
	{
		count+=(*ptr).second.size();
	}
	if (count>1000)
	{
		string strURL,sharepath;
		sharepath=PROPMANAGER::instance()->GetTempSharePath();
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort() + sharepath+"Quality_";
		time_t curtime;
		int type=0;
		float valu=0.0;
		string strtime=TimeUtil::DateTimeToString(time(0));
		string dstZipPath=PROPMANAGER::instance()->GetTempFileLoac()+"Quality_"+strtime+".zip";
		string srcFilePath=PROPMANAGER::instance()->GetTempFileLoac()+"Quality_"+strtime+".txt";
		strURL +=strtime+".zip";
		ofstream ofile(srcFilePath.c_str());
		std::map< string,vector<eQualityInfo> >::iterator itr=FreqQualityInfo.begin();
		for (;itr!=FreqQualityInfo.end();++itr)
		{
			for(vector<eQualityInfo>::iterator vecitr=(*itr).second.begin();vecitr!=(*itr).second.end();vecitr++)
			{
		//		freq=StrUtil::Str2Float(vecitr->freq);
				std::string channelcode;
				CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,vecitr->freq,channelcode);

				curtime=TimeUtil::StrToDateTime(vecitr->checktime);
				type =StrUtil::Str2Int(vecitr->type);
				valu = StrUtil::Str2Float(vecitr->valu)/1000;
			
				ofile<<channelcode<<" "<<curtime<<" "<<type<<" "<<valu<<" "<<endl;
			}
		}
		ofile.close();
		OSFunction::CompressFile(dstZipPath,srcFilePath);
		pXMLNODE filenode=parser.CreateNodePtr(parser.GetRootNode(),"QualityReportFile");
		parser.SetAttrNode(string("URL"),strURL,filenode); //设置压缩数据存放地址
		parser.SetAttrNode(string("Size"),(int)count,filenode); //压缩数据文件大小
	}
	else
	{
		pXMLNODE reportnode=parser.CreateNodePtr(parser.GetRootNode(),"QualityReport");
		for (std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();ptr!=FreqQualityInfo.end();++ptr)
		{	
			std::map< string,vector<eQualityInfo> >  TimeQualityInfo;
			//对checktime进行整理，相同checktime的eQualityInfo放入同一vector容器里
			for (size_t i = 0;i<(*ptr).second.size();i++)
			{
				TimeQualityInfo[(*ptr).second.at(i).checktime].push_back((*ptr).second.at(i));
			}

			//对于不同的checktime创建Quality节点及QualityIndex节点列表
			for (std::map< string,vector<eQualityInfo> >::iterator in_ptr=TimeQualityInfo.begin();in_ptr!=TimeQualityInfo.end();++in_ptr)
			{
				try
				{
					std::string channelcode;
					CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,(*ptr).first,channelcode);
					pXMLNODE qualitynode=parser.CreateNodePtr(reportnode,"Quality");
					parser.SetAttrNode(string("EquCode"),(*in_ptr).second[0].STD,qualitynode); //设置Quality节点的STD属性
					parser.SetAttrNode(string("ChannelCode"),channelcode,qualitynode);  //设置Quality节点的Freq属性
					parser.SetAttrNode(string("CheckTime"),(*in_ptr).first,qualitynode); //设置Quality节点的CheckTime属性
					//相同STD、 Freq、 CheckTime的指标结果列表
					for (vector<eQualityInfo>::iterator pptr=(*in_ptr).second.begin();pptr!=(*in_ptr).second.end();++pptr)
					{
						float Value=StrUtil::Str2Float((*pptr).valu);
						(*pptr).valu=StrUtil::Float2Str(Value/1000);
						pXMLNODE qualityindexnode=parser.CreateNodePtr(qualitynode,"QualityIndex");
						parser.SetAttrNode(string("Type"),(*pptr).type,qualityindexnode); //设置QualityIndex节点的Type属性
						parser.SetAttrNode(string("Desc"),(*pptr).desc,qualityindexnode); //设置QualityIndex节点的Desc属性
						parser.SetAttrNode(string("Value"),(*pptr).valu,qualityindexnode); //设置QualityIndex节点的Value属性
					}
				}
				catch(...)
				{
					string error = "设置QualityReport子节点属性失败";
					APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
					return "";
				}
			}

		}

	}
	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}

std::string TranslateUpXMLForRADIO::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
{
	std::string retXml=GetXmlHeader(task,"ClientInfoQuery");
	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE newNode=parser.CreateNodePtr(rootNode,"ClientInfoQueryReport");

	//根据客户端的信息构造xml
	if(vecClient.size()>0)
	{
		//构造ClientInfoQueryReport节点
		pXMLNODE ClientNode = parser.GetNodeFromPath("Msg/ClientInfoQueryReport");
		size_t count=0;


		for(;count<vecClient.size();++count)
		{	sVedioUserInfo user=vecClient[count];
		if(user.fail_num==1)
			continue;
		pXMLNODE clientinfonode=parser.CreateNodePtr(ClientNode,"ClientInfo");
		parser.SetAttrNode(std::string("URL"),user.URL,clientinfonode);
		int num=0;
		for (;num < vecClient.size();++num)
		{
			if (user.URL==vecClient[num].URL)
			{
				//填写连接客户信息
				pXMLNODE childnode=parser.CreateNodePtr(clientinfonode,"Client");
				parser.SetAttrNode(std::string("IP"),vecClient[num].IP,childnode);
				parser.SetAttrNode(std::string("UserName"),vecClient[num].Name,childnode);
				parser.SetAttrNode(std::string("Priority"),vecClient[num].Priority,childnode);
				parser.SetAttrNode(std::string("CenterCode"),vecClient[num].CenterCode,childnode);
				vecClient[num].fail_num=1;

			}
		}
		}

	}
	parser.SaveToString(retXml);

	return retXml;

}
std::string TranslateUpXMLForRADIO::TranslateStopClient( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"VideoClientStop");//标准的返回头xml
	return retXml;
}
//返回运行图设置XML
std::string TranslateUpXMLForRADIO::TranslateRunPlanSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"RunPlanScan");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec )
{
	std::string retXml=GetXmlHeader(task,"TaskSchedulerQuery");//标准的返回头xml
	XmlParser parser(retXml.c_str());
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE ReportNode = parser.CreateNodePtr(rootNode,"TaskSchedulerReport");
	for(int i=0;i<taskinfovec.size();i++)
	{
		if (taskinfovec[i].taskname == "QualityTask")	//指标任务不显示
		{
			continue;
		}

		pXMLNODE TaskNode = parser.CreateNodePtr(ReportNode,"Task");
		parser.SetAttrNode("TaskID",taskinfovec[i].taskid,TaskNode);
		parser.SetAttrNode("MsgID",taskinfovec[i].msgid,TaskNode);
		parser.SetAttrNode("DateTime",taskinfovec[i].setdatetime,TaskNode);
		parser.SetAttrNode("SrcCode",task->GetDstCode(),TaskNode);
		parser.SetAttrNode("ChCode",taskinfovec[i].channelid,TaskNode);

		pXMLNODE SchedulerNode = parser.CreateNodePtr(TaskNode,"Scheduler");
		parser.SetAttrNode("State",string("Work"),SchedulerNode);
		parser.SetAttrNode("StartDateTime",TimeUtil::DateTimeToStr(taskinfovec[i].startdatetime),SchedulerNode);
		parser.SetAttrNode("EndDateTime",TimeUtil::DateTimeToStr(taskinfovec[i].enddatetime),SchedulerNode);
	}
	parser.SaveToString(retXml);
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateManualTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec )
{
	std::string retXml=GetXmlHeader(task,"ManualTaskSchedulerQuery");//标准的返回头xml
	XmlParser parser(retXml.c_str());
	pXMLNODE rootNode =parser.GetRootNode();
	pXMLNODE ReportNode =parser.CreateNodePtr(rootNode,"ManualTaskSchedulerReport");

	pXMLNODE SourceNode = parser.CreateNodePtr(ReportNode,"Resources");
	std::list<int> devicedlist;
	PROPMANAGER::instance()->GetTaskDeviceList(std::string("AutoRecord"), RADIO,devicedlist);	//只获取录像通道
	std::list<int>::iterator ptr=devicedlist.begin();
	for(;ptr!=devicedlist.end();ptr++)
	{
		pXMLNODE DeviceNode = parser.CreateNodePtr(SourceNode,"DeviceID");
		parser.SetAttrNode("Code",StrUtil::Int2Str(*ptr),DeviceNode);
		for(int k=0;k<taskinfovec.size();k++)
		{
			if(taskinfovec[k].deviceid==*ptr)
			{
				pXMLNODE TaskNode = parser.CreateNodePtr(DeviceNode,"Task");
				parser.SetAttrNode("TaskID",taskinfovec[k].taskid,TaskNode);
				parser.SetAttrNode("MsgID",taskinfovec[k].msgid,TaskNode);
				parser.SetAttrNode("DateTime",taskinfovec[k].setdatetime,TaskNode);
				parser.SetAttrNode("SrcCode",task->GetDstCode(),TaskNode);
				parser.SetAttrNode("ChCode",taskinfovec[k].channelid,TaskNode);

				pXMLNODE SchedulerNode = parser.CreateNodePtr(TaskNode,"Scheduler");
				parser.SetAttrNode("State",string("Work"),SchedulerNode);
				parser.SetAttrNode("StartDateTime",TimeUtil::DateTimeToStr(taskinfovec[k].startdatetime),SchedulerNode);
				parser.SetAttrNode("EndDateTime",TimeUtil::DateTimeToStr(taskinfovec[k].enddatetime),SchedulerNode);

			}
		}
	}
	pXMLNODE ConflictsNode = parser.CreateNodePtr(ReportNode,"Conflicts");

	parser.SaveToString(retXml);

	return retXml;
}


std::string TranslateUpXMLForRADIO::TranslateChannelCheck(const XMLTask* task, const std::vector<sChannelInfo>& vecChanInfo)
{
	std::string retXml=GetXmlHeader(task,"RadioChannelCheckReport");//标准的返回头xml

	XmlParser retParser( retXml.c_str() );
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE ChanReportNode = retParser.CreateNodePtr(rootNode, "RadioChannelCheckReport");

	if ( vecChanInfo.empty() )
	{
		string tempXml;
		retParser.SaveToString( tempXml );
		return tempXml;
	}

	for (int i=0; i<vecChanInfo.size(); i++)
	{
		pXMLNODE channelNode = retParser.CreateNodePtr(ChanReportNode, "Channel");

		retParser.SetAttrNode("Code", vecChanInfo[i].ChannelCode, channelNode);
		retParser.SetAttrNode("Name", vecChanInfo[i].ProgramName, channelNode);
		retParser.SetAttrNode("Freq", vecChanInfo[i].Freq, channelNode);
	}

	string upXml = "";
	retParser.SaveToString(upXml);
	return upXml;
}


std::string TranslateUpXMLForRADIO::TranslateRecordCheck(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo)
{
	std::string retXml=GetXmlHeader(task,"AutoRecordCheckReport");//标准的返回头xml

	XmlParser retParser( retXml.c_str() );
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE reportNode = retParser.CreateNodePtr(rootNode, "AutoRecordCheckReport");

	if ( mapTaskInfo.empty() )
	{
		string tempXml;
		retParser.SaveToString( tempXml );
		return tempXml;
	}

	std::map<int,sTaskInfo>::const_iterator taskIter = mapTaskInfo.begin();
	for (; taskIter!=mapTaskInfo.end(); taskIter++)
	{
		pXMLNODE channelNode = retParser.CreateNodePtr(reportNode, "Channel");
 
		string code = "";
		CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO, (*taskIter).second.freq, code);
		retParser.SetAttrNode("Code", code, channelNode);
		retParser.SetAttrNode("Freq", (*taskIter).second.freq, channelNode);
		retParser.SetAttrNode("DeviceID", (*taskIter).first, channelNode);
		retParser.SetAttrNode("Bps", string("32000"), channelNode);
		retParser.SetAttrNode("Priority", task->GetPriority(), channelNode);
	}

	string upXml = "";
	retParser.SaveToString(upXml);
	return upXml;
}


std::string TranslateUpXMLForRADIO::TranslateAlarmParamCheck(const XMLTask* task, const CodeAlarmParamMap& mapAlarmParam)
{
	std::string retXml=GetXmlHeader(task,"QualityAlarmParamCheckResp");//标准的返回头xml

	XmlParser retParser( retXml.c_str() );
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE checkNode = retParser.CreateNodePtr(rootNode, "QualityAlarmParamCheckResp");

	if (mapAlarmParam.empty())
	{
		string tempXml;
		retParser.SaveToString( tempXml );
		return tempXml;
	}

	CodeAlarmParamMap::const_iterator mapIter = mapAlarmParam.begin();
	for (; mapIter!=mapAlarmParam.end(); mapIter++)
	{
		pXMLNODE ChannelNode = retParser.CreateNodePtr(checkNode, "QualityAlarmParamChannel");
		retParser.SetAttrNode("ChannelCode", (*mapIter).first, ChannelNode);

		std::vector<sAlarmParam>::const_iterator alarmParamIter = (*mapIter).second.begin();
		for (; alarmParamIter!=(*mapIter).second.end(); alarmParamIter++)
		{
			//新频报警参数无需上报(新安播)
			if ((*alarmParamIter).TypeID=="50" || (*alarmParamIter).TypeID=="51"
				|| (*alarmParamIter).TypeDesc=="新频报警" )
			{
				continue;
			}

			pXMLNODE alarmParamNode = retParser.CreateNodePtr(ChannelNode, "QualityAlarmParam");
			retParser.SetAttrNode("AlarmType", (*alarmParamIter).TypeID, alarmParamNode);
			retParser.SetAttrNode("Desc", (*alarmParamIter).TypeDesc, alarmParamNode);
			if (StrUtil::Str2Int(alarmParamIter->TypeID) < 10)
				retParser.SetAttrNode("DownThreshold", (*alarmParamIter).DownThreshold, alarmParamNode);
			else
				retParser.SetAttrNode("DownThreshold", (*alarmParamIter).Num, alarmParamNode);
			retParser.SetAttrNode("UpThreshold", (*alarmParamIter).UpThreshold, alarmParamNode);
			retParser.SetAttrNode("TimeThreshold", (*alarmParamIter).Duration, alarmParamNode);
		}
	}

	string upXml = "";
	retParser.SaveToString( upXml );
	return upXml;
}


std::string TranslateUpXMLForRADIO::TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam)
{
	std::string retXml=GetXmlHeader(task,"RunPlanCheckReport");		//标准的返回头xml

	XmlParser retParser( retXml.c_str() );
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE reportNode = retParser.CreateNodePtr(rootNode, "RunPlanCheckReport");

	RunPlanParamMap::const_iterator cRunplanIter = mapRunplanParam.begin();
	for (; cRunplanIter!=mapRunplanParam.end(); cRunplanIter++)
	{
		pXMLNODE channelNode = retParser.CreateNodePtr(reportNode, "RunPlanChannel");
		retParser.SetAttrNode("EquCode", equCode, channelNode);
		retParser.SetAttrNode("ChannelCode", (*cRunplanIter).first, channelNode);

		RunPlanParamVec::const_iterator RunplanVecIter = (*cRunplanIter).second.begin();
		for (; RunplanVecIter!=(*cRunplanIter).second.end(); RunplanVecIter++)
		{
			//dayofweek,starttime,endtime同时不为空，则认为是CycleWorkTime
			if ( (*RunplanVecIter).DayOfWeek!="" && (*RunplanVecIter).StartTime!="" && (*RunplanVecIter).EndTime!="" )
			{	
				pXMLNODE cycleNode = retParser.CreateNodePtr(channelNode, "CycleWorkTime");
				retParser.SetAttrNode("DayOfWeek", (*RunplanVecIter).DayOfWeek, cycleNode);
				retParser.SetAttrNode("StartTime", (*RunplanVecIter).StartTime, cycleNode);
				retParser.SetAttrNode("EndTime", (*RunplanVecIter).EndTime, cycleNode);
				retParser.SetAttrNode("ValidStartDateTime", (*RunplanVecIter).ValidStartDateTime, cycleNode);
				retParser.SetAttrNode("ValidEndDateTime", (*RunplanVecIter).ValidEndDateTime, cycleNode);
			}
			else
			{
				pXMLNODE singleNode = retParser.CreateNodePtr(channelNode, "SingleWorkTime");
				retParser.SetAttrNode("StartDateTime", (*RunplanVecIter).StartDateTime, singleNode);
				retParser.SetAttrNode("EndDateTime", (*RunplanVecIter).EndDateTime, singleNode);
			}
		}
	}

	string upXml = "";
	retParser.SaveToString( upXml );
	return upXml;
}

std::string TranslateUpXMLForRADIO::TranslateAlarmExpireTask(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"TaskAndAlarmExpire");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForRADIO::TranslateTaskRecordFile(  const XMLTask* task,std::vector<sRecordFileInfo> FileInfo)
{
	std::string headxml = GetXmlHeader(task, "TaskRecordFileQuery");

	XmlParser UpXmlParser( headxml.c_str() );
	pXMLNODE ReportNode = UpXmlParser.CreateNodePtr(UpXmlParser.GetRootNode(), "TaskRecordFileQuery");
	pXMLNODE RecNode = UpXmlParser.CreateNodePtr(ReportNode, "TaskRecord");
	UpXmlParser.SetAttrNode("EquCode", std::string(""),RecNode);

	//新安播要求返回channelCode
	std::string strChannelId = task->GetChannelID();
	std::string strChannelCode;
	CHANNELINFOMGR::instance()->GetChanCodeByChanId(RADIO, strChannelId, strChannelCode);
	if (strChannelCode.length()<1)
	{
		strChannelCode = strChannelId;
	}
	UpXmlParser.SetAttrNode("ChCode", strChannelCode,RecNode);
	UpXmlParser.SetAttrNode("TaskID", task->GetTaskID(),RecNode);

	for(int i=0;i<FileInfo.size();i++)
	{
		pXMLNODE UrlNode = UpXmlParser.CreateNodePtr(RecNode, "Record");
		UpXmlParser.SetAttrNode("Number", std::string("0"), UrlNode);
		UpXmlParser.SetAttrNode("RecordID", 100, UrlNode);
		UpXmlParser.SetAttrNode("StartDateTime", FileInfo[i].starttime, UrlNode);
		UpXmlParser.SetAttrNode("EndDateTime", FileInfo[i].endtime, UrlNode);
		UpXmlParser.SetAttrNode("Size", 100000, UrlNode);
		UpXmlParser.SetAttrNode("URL", FileInfo[i].url, UrlNode);
		UpXmlParser.SetAttrNode("ExpireDays", PROPMANAGER::instance()->GetTempFileExpire(), UrlNode);
	}
	string retxml;
	UpXmlParser.SaveToString(retxml);
	return retxml;
}