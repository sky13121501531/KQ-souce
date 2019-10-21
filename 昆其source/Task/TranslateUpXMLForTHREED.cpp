///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateUpXMLForTHREED.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将原子任务执行结果的XML转化为应用系统所需的XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXMLForTHREED.h"
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
#include "../Foundation/BaseAnalyser.h"
#include "../Foundation/OSFunction.h"
#include <fstream>
//返回实时视频XML
std::string TranslateUpXMLForTHREED::TranslateStreamRealtimeQuery(const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChangeProgramQuery");//标准的返回头xml

	std::string index, url;
	if (task->GetRetValue()==RUN_SUCCESS)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(修改接口)
		PROPMANAGER::instance()->GetStreamProtocol(THREED,protocol);
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
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"ReturnInfo");//创建ReturnInfo节点
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"RealStreamURL");//创建ReturnInfo节点的子节点RealStreamURL节点

	retParser.SetAttrNode("Index",index,mediaStreamNode);//设置MediaStream节点的index属性
	retParser.SetAttrNode("URL",url,mediaStreamNode);//设置MediaStream节点的URL属性
	retParser.SaveToString(retXml);
	return retXml;
}
//返回轮播查询XML
std::string TranslateUpXMLForTHREED::TranslateStreamRoundQuery(const XMLTask* task,std::string deviceID,bool multi)
{
	std::string retXml;//返回的xml
	std::string retType="StreamRoundInfoQuery";//硬件返回xml的return节点的类型和值

	std::string headerxml=GetXmlHeader(task,retType);//标准的返回头xml

	std::string index, url;
	if(task->GetRetValue()==5)
	{	
		XmlParser headParser;
		headParser.Set_xml(headerxml);
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("StreamRoundInfoQuery"),retNode );
		headParser.SaveToString(retXml);
		return retXml;
	}
	if (task->GetRetValue()==0)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(修改接口)
		PROPMANAGER::instance()->GetStreamProtocol(THREED,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() +"/roundstream" ;//视频播放地址
		}
		else
		{
			//wz_0217
			int virtualid = 0;
			PROPMANAGER::instance()->GetVirDeviceId(THREED, virtualid);
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(virtualid);	//视频播放地址
			//wz_0217
		}
	}
	XmlParser retParser;
	retParser.Set_xml(headerxml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"ReturnInfo");//创建ReturnInfo节点
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"StreamRoundInfoQuery");//创建ReturnInfo节点的子节点StreamRoundInfoQuery节点
	pXMLNODE roundStreamNode=retParser.CreateNodePtr(mediaStreamNode,"RoundStream");

	retParser.SetAttrNode("Index",index,roundStreamNode);//设置MediaStream节点的index属性
	retParser.SetAttrNode("URL",url,roundStreamNode);//设置MediaStream节点的URL属性
	retParser.SaveToString(retXml);

	return retXml;
}
//返回频道扫描XML
std::string TranslateUpXMLForTHREED::TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML)
{
	std::string retXml=GetXmlHeader(task,"ChannelScanQuery");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
	pXMLNODE channelScanQueryNode=parser.CreateNodePtr(returnInfoNode,"ChannelScanQuery");//为返回的xml创建ChannelScanReport节点
	std::string scantime = TimeUtil::GetCurDateTime();
	parser.SetAttrNode("ScanTime",scantime,channelScanQueryNode);
	//初始化属性
	string   STD, Freq, QAM, HeaderMode, CarrierNumber, EncodeEfficiency, InterwovenMode,SymbolRate;
	string   DoublePilot, PnPhase, OrgNetID, TsID;
	string   Program, ProgramID,ServiceID, VideoPID, AudioPID, Encryption, Encrypt,ISHDTV; 

	XmlParser retParser;
	retParser.Set_xml(strRtnXML);

	pXMLNODE rtnTypeNode=retParser.GetNodeFromPath("Msg/Return");
	std::string value;
	retParser.GetAttrNode(rtnTypeNode,"Value",value);//解析硬件返回xml中的value值，以此判断硬件执行成功与否
	if (value=="0")
	{
		pXMLNODE rtnChannelScanReportNode=retParser.GetNodeFromPath("Msg/ChannelScanReport");//硬件返回xml的ChannelScanReport节点
		pXMLNODELIST nodeList=retParser.GetNodeList(rtnChannelScanReportNode);//硬件返回xml的ChannelScanReport节点的子节点列表
		for (int j=0;j!=nodeList->Size();++j)
		{
			pXMLNODE channelScanNode=parser.CreateNodePtr(channelScanQueryNode,"ChannelScan");//为返回的xml创建ChannelScan节点
			pXMLNODE childNode=retParser.GetNextNode(nodeList);//硬件返回xml的ChannelScan节点的子节点列表

			retParser.GetAttrNode(childNode,"Freq",Freq);//获得Freq属性值
			retParser.GetAttrNode(childNode,"OrgNetID",OrgNetID);//获得OrgNetID属性值
			retParser.GetAttrNode(childNode,"TsID",TsID);//获得TsID属性值
			retParser.GetAttrNode(childNode,"SymbolRate",SymbolRate);
			retParser.GetAttrNode(childNode,"QAM",QAM);

			parser.SetAttrNode("Freq",Freq,channelScanNode);//设置返回xml的ChannelScan节点的Freq属性值
			parser.SetAttrNode("OrgNetID",OrgNetID,channelScanNode);//设置返回xml的ChannelScan节点的OrgNetID属性值
			parser.SetAttrNode("TsID",TsID,channelScanNode);//设置返回xml的ChannelScan节点的TsID属性值

			//add by gxd 2010-10-15
			parser.SetAttrNode("QAM",QAM,channelScanNode);
			parser.SetAttrNode("SymbolRate",SymbolRate,channelScanNode);

			pXMLNODELIST childNodeList=retParser.GetNodeList(childNode);//硬件返回xml的ChannelScan节点的子节点列表
			for (int k=0;k!=childNodeList->Size();++k)
			{
				pXMLNODE channelNode=parser.CreateNodePtr(channelScanNode,"Channel");//为返回的xml创建Channel节点

				pXMLNODE subChildNode=retParser.GetNextNode(childNodeList);//硬件返回xml的ChannelScan节点的子节点channel
				retParser.GetAttrNode(subChildNode,"Program",Program);//获得Program属性值
				retParser.GetAttrNode(subChildNode,"ProgramID",ProgramID); //获得ProgramID属性值
				retParser.GetAttrNode(subChildNode,"ServiceID",ServiceID);//获得ServiceID属性值
				retParser.GetAttrNode(subChildNode,"VideoPID",VideoPID);//获得VideoPID属性值
				retParser.GetAttrNode(subChildNode,"AudioPID",AudioPID);//获得AudioPID属性值
				retParser.GetAttrNode(subChildNode,"Encrypt", Encrypt);
				retParser.GetAttrNode(subChildNode,"HDTV",ISHDTV);

				parser.SetAttrNode("Program",Program,channelNode);//设置返回xml的Channel节点的Program属性值
				parser.SetAttrNode("ProgramID",ProgramID,channelNode); //设置返回xml的Channel节点的ProgramID属性值
				parser.SetAttrNode("ServiceID",ServiceID,channelNode);//设置返回xml的Channel节点的ServiceID属性值
				parser.SetAttrNode("VideoPID",VideoPID,channelNode);//设置返回xml的Channel节点的VideoPID属性值
				parser.SetAttrNode("AudioPID",AudioPID,channelNode);//设置返回xml的Channel节点的AudioPID 	
				parser.SetAttrNode("Encrypt", Encrypt,channelNode);
				parser.SetAttrNode("HDTV",ISHDTV,channelNode);

				pXMLNODELIST StreamNodeList=retParser.GetNodeList(subChildNode);
				for(int n=0;n!=StreamNodeList->Size();n++)
				{
					string StreamType,PID;
					pXMLNODE StreamNode=parser.CreateNodePtr(channelNode,"elementryPID");

					pXMLNODE subStreamNode=retParser.GetNextNode(StreamNodeList);
					retParser.GetAttrNode(subStreamNode,"StreamType",StreamType);
					retParser.GetAttrNode(subStreamNode,"PID",PID);

					parser.SetAttrNode("StreamType",StreamType,StreamNode);
					parser.SetAttrNode("PID",PID,StreamNode);
				}
			}
		}
	}
	parser.SaveToString(retXml);
	return retXml;
}
//返回频道设置xml
std::string TranslateUpXMLForTHREED::TranslateChannelSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChannelSetReport");//标准的返回头xml
	return retXml;
}

//返回视频用户查询XML
std::string TranslateUpXMLForTHREED::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
{
	std::string retXml=GetXmlHeader(task,"ClientInfoQuery");
	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE newNode=parser.CreateNodePtr(rootNode,"ClientInfoQueryReport");

	//根据客户端的信息构造xml
	if(vecClient.size()>0)
	{
		std::string serverip="http://"+PROPMANAGER::instance()->GetHttpVideoIp()+"/";
		//构造ClientInfoQueryReport节点
		pXMLNODE ClientNode = parser.GetNodeFromPath("Msg/ClientInfoQueryReport");
		size_t count=0;
		while (count<vecClient.size())
		{
			sVedioUserInfo user=vecClient[count];
			pXMLNODE clientinfonode=parser.CreateNodePtr(ClientNode,"ClientInfo");
			parser.SetAttrNode(std::string("URL"),serverip+user.DeviceID,clientinfonode);
			for (;count < vecClient.size();++count)
			{
				if (user.DeviceID==vecClient[count].DeviceID)
				{
					//填写连接客户信息
					pXMLNODE childnode=parser.CreateNodePtr(clientinfonode,"Client");
					parser.SetAttrNode(std::string("IP"),vecClient[count].IP,childnode);
					parser.SetAttrNode(std::string("UserName"),vecClient[count].Name,childnode);
					parser.SetAttrNode(std::string("Priority"),vecClient[count].Priority,childnode);
					parser.SetAttrNode(std::string("CenterCode"),string("900000X20"),childnode);
				}
				else
					break;
			}
		}
	}
	parser.SaveToString(retXml);

	return retXml;

}
//返回表查询XML
std::string TranslateUpXMLForTHREED::TranslateTableQuery(const XMLTask *task, std::string strFileName)
{
	std::string rtnxml = GetXmlHeader(task,"table");

	XmlParser parser;
	parser.Set_xml(rtnxml);

	pXMLNODE queryReport = parser.CreateNodePtr( parser.GetRootNode(),"ReturnInfo" );

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");

	std::string strURL;

	if (task->GetRetValue() == RUN_SUCCESS)
	{
		//modified by jidushan 11.03.31 : 表共享路径改为通过配置文件获取
		strURL = "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":";
		strURL += PROPMANAGER::instance()->GetHttpServerPort();
		std::string sharepath;
		PROPMANAGER::instance()->GetTableSharePath(task->GetDVBType(), sharepath);
		strURL += sharepath + strFileName;
	}

	parser.SetAttrNode( "Redirect",strURL,RetNode ); //设置URL属性

	parser.SaveToString(rtnxml); //保存到字符串
	return rtnxml;

}
//返回录像设置XML
std::string TranslateUpXMLForTHREED::TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML;

	if(task->GetTaskID() == "0")
	{
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
	}
	else
	{
		retXML=GetXmlHeader(task,"ProvisionalRecordTaskSet");
	}
	return retXML;
}
//返回偱切录像设置XML
std::string TranslateUpXMLForTHREED::TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML = GetXmlHeader(task,"RoundRecordSet");
	return retXML;
}

//返回指标任务设置xml
std::string TranslateUpXMLForTHREED::TranslateQualitySet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityReportTaskSet");//标准的返回头xml
	return retXml;
}
//返回指标查询XML
std::string TranslateUpXMLForTHREED::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
{
	string rtnHead =  GetXmlHeader(task,"QualityQuery");

	//声明解析类的对象
	XmlParser parser;
	parser.Set_xml(rtnHead);

	//设置返回XML根元素属性
	size_t count=0;//指标测量的数目，多余1000条打包
	std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();
	for (;ptr!=FreqQualityInfo.end();++ptr)
	{
		count+=(*ptr).second.size();
	}
	if (count>1000)
	{
		string strURL;
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath()+"Quality_";
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
				std::string channelcode;
				CHANNELINFOMGR::instance()->GetChannelCodeByFreq(THREED,vecitr->freq,channelcode);
				curtime=TimeUtil::StrToDateTime(vecitr->checktime);
				type =StrUtil::Str2Int(vecitr->type);
				valu = StrUtil::Str2Float(vecitr->valu)/1000;
				ofile<<channelcode<<" "<<curtime<<" "<<type<<" "<<valu<<endl;
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
		for (std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();ptr!=FreqQualityInfo.end();++ptr)
		{
			pXMLNODE reportnode=parser.CreateNodePtr(parser.GetRootNode(),"QualityReport");

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
					pXMLNODE qualitynode=parser.CreateNodePtr(reportnode,"Quality");
					parser.SetAttrNode(string("STD"),(*in_ptr).second[0].STD,qualitynode); //设置Quality节点的STD属性
					parser.SetAttrNode(string("Freq"),(*ptr).first,qualitynode);  //设置Quality节点的Freq属性
					parser.SetAttrNode(string("CheckTime"),(*in_ptr).first,qualitynode); //设置Quality节点的CheckTime属性
					//相同STD、 Freq、 CheckTime的指标结果列表
					for (vector<eQualityInfo>::iterator pptr=(*in_ptr).second.begin();pptr!=(*in_ptr).second.end();++pptr)
					{

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

//实时指标查询
std::string TranslateUpXMLForTHREED::TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml)
{
	string rtnHead =  GetXmlHeader(task,"GetIndexSet");

	//声明解析类的对象
	XmlParser parser;
	parser.Set_xml(rtnHead);
	XmlParser devParser;
	devParser.Set_xml(retxml);
	string freq,type,desc,val;
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE devRootNode= devParser.GetRootNode();

	pXMLNODE IndexNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE GetListNode = parser.CreateNodePtr(IndexNode,"GetIndexSet");
	pXMLNODE ReportNode = devParser.GetNodeFromPath("Msg/QualityQueryReport");
	devParser.GetAttrNode(ReportNode,"Freq",freq);
	parser.SetAttrNode("Index",task->GetDeviceID(),GetListNode);
	parser.SetAttrNode("Freq",freq,GetListNode);
	pXMLNODE paramsNode = devParser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");

	pXMLNODELIST paramList = devParser.GetNodeList(paramsNode);

	int count=devParser.GetChildCount(paramsNode);

	for(int k=0;k<count;k++)
	{
		pXMLNODE getnode=NULL;
		pXMLNODE paramnode=NULL;

		paramnode = devParser.GetNextNode(paramList);
		if(devParser.GetNodeName(paramnode)=="QualityIndex")
		{
			string add;
			float level;
			getnode=parser.CreateNodePtr(GetListNode,"GetIndex");
			devParser.GetAttrNode(paramnode,"Type",type);
			devParser.GetAttrNode(paramnode,"Desc",desc);
			devParser.GetAttrNode(paramnode,"Value",val);
			/*if (type=="1")
			{
				DEVICEFREQLEVEL::instance()->InitDevice();
				std::string tmpfreq=freq;
				if (tmpfreq.length()>3)
				{
					tmpfreq=tmpfreq.substr(0,3);
				}
				DEVICEFREQLEVEL::instance()->GetFreqLevel(StrUtil::Str2Float(tmpfreq),level);
				DEVICEFREQLEVEL::instance()->CloseDevice();
				val=StrUtil::Float2Str(level);
			}*/
			

			if (type == "2" && val.length() >=5)
			{
				val = "1.00";
			}
			if (type == "9")
			{
				parser.SetAttrNode("Type",type,getnode);
				parser.SetAttrNode("Desc",desc,getnode);

				pXMLNODELIST paramStarList = devParser.GetNodeList(paramnode);

				int startnum=devParser.GetChildCount(paramnode);

				for(int startindex=0;startindex<startnum;++startindex)
				{
					pXMLNODE startgetnode = devParser.GetNextNode(paramStarList);

					string Ivalue,Qvalue;
					pXMLNODE startnode=parser.CreateNodePtr(getnode,"Value");
					devParser.GetAttrNode(startgetnode,"I",Ivalue);
					devParser.GetAttrNode(startgetnode,"Q",Qvalue);

					parser.SetAttrNode("I",Ivalue,startnode);
					parser.SetAttrNode("Q",Qvalue,startnode);
				}
				continue;
			}

			QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(task->GetDVBType(),StrUtil::Int2Str(task->GetDeviceID()),type,"","",add);
			float val_int = StrUtil::Str2Float(val)+StrUtil::Str2Float(add);
			parser.SetAttrNode("Type",type,getnode);
			parser.SetAttrNode("Desc",desc,getnode);
			parser.SetAttrNode("Value",StrUtil::Float2Str(val_int),getnode);
		}
	}

	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}
//返回报警参数设置XML
std::string TranslateUpXMLForTHREED::TranslateAlarmParamSet( const XMLTask* task )
{
	std::string strTaskType = (const_cast<XMLTask*>(task))->GetObjectName();
	std::string retXml=GetXmlHeader(task, strTaskType.c_str() );//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForTHREED::TranslateAlarmReport( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"AlarmTypeSet");//标准的返回头xml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateCleanAlarmStateTask(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"CleanAlarmState");//标准的返回头xml
	return retXml;
}


//wz_110107
std::string TranslateUpXMLForTHREED::TranslateMatrixQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"MatrixQuery");//标准的返回头xml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateChangeQAMQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"ChangeQAMQuery");//标准的返回头xml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateLoopAlarmInfo(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"LoopAlaInf");//标准的返回头xml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateRecordParamSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"RecordParamSet");//标准的返回头xml
	return retXml;
}

//返回通道状态查询XML
std::string TranslateUpXMLForTHREED::TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus)
{
	std::string retXml=GetXmlHeader(task,"GetNvrStatus");
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE deviceNode=parser.CreateNodePtr(rootNode,"ReturnInfo");

	pXMLNODE statusnode=parser.GetNodeFromPath("Msg/ReturnInfo");
	for (size_t i=0;i!=VecDeviceStatus.size();++i)
	{
		pXMLNODE nodeinfo=parser.CreateNodePtr(statusnode,"GetNvrIndexStatus");//创建GetNvrIndexStatus节点
		//设置GetNvrIndexStatus节点的属性
		std::string Freq,ServiceID;
		parser.SetAttrNode("Index",VecDeviceStatus[i].DevicID,nodeinfo);
		parser.SetAttrNode("Status",VecDeviceStatus[i].Statuc,nodeinfo);
		parser.SetAttrNode("Freq",Freq,nodeinfo);
		parser.SetAttrNode("ServiceID",ServiceID,nodeinfo);
		parser.SetAttrNode("Desc",VecDeviceStatus[i].Desc,nodeinfo);

	}
	parser.SaveToString(retXml);
	return retXml;
}

//返回运行图设置XML
std::string TranslateUpXMLForTHREED::TranslateRunPlanSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AlarmTimeSet");//标准的返回头xml
	return retXml;
}

//返回自动录像音视频查询XML
std::string TranslateUpXMLForTHREED::TranslateAutoRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"NVRVideoHistoryInquiry");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");           //创建ReturnInfo节点
	pXMLNODE autoRecordReportNode=parser.CreateNodePtr(returnInfoNode,"NVRVideoHistoryInquiry"); //创建NVRVideoHistoryInquiry节点
	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordReportNode,"NVRVideoHistory");        //创建NVRVideoHistory节点

	parser.SetAttrNode("URL",URL,autoRecordNode);  //设置NVRVideoHistory节点的URL属性

	parser.SaveToString(retXml);
	return retXml;
}

//返回自动录像节目文件下载XML
std::string TranslateUpXMLForTHREED::TranslateAutoRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"NVRVideoHistoryDownInquiry");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");                     //创建ReturnInfo节点
	pXMLNODE autoRecordFileReportNode=parser.CreateNodePtr(returnInfoNode,"NVRVideoHistoryDownInquiry");   //创建NVRVideoHistoryDownInquiry节点

	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordFileReportNode,"NVRVideoHistoryDown");      //创建NVRVideoHistoryDown节点

	parser.SetAttrNode("URL",URL,autoRecordNode);    //设置NVRVideoHistoryDown节点的URL属性

	parser.SaveToString(retXml);
	return retXml;
}

//返回任务录像音视频查询XML
std::string TranslateUpXMLForTHREED::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"NVRTaskRecordInquiry");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE rootnode=parser.GetRootNode();
	pXMLNODE rtnnode=parser.GetNodeFromPath("Msg/Return");
	if(URL.length()<1)
	{
		parser.SetAttrNode("Value",string("1"),rtnnode);
		parser.SetAttrNode("Desc",string("没有查到相关数据"),rtnnode);
		pXMLNODE errnode=parser.CreateNodePtr(rootnode,"ErrReport");
		pXMLNODE NVRTaskRecordInquiryNode=parser.CreateNodePtr(errnode,"NVRTaskRecordInquiry");
		pXMLNODE NVRTaskRecord =parser.CreateNodePtr(NVRTaskRecordInquiryNode,"NVRTaskRecord");
		parser.SetAttrNode("TaskID",task->GetTaskID(),NVRTaskRecord);
		parser.SetAttrNode("StartDateTime",startDateTime,NVRTaskRecord);
		parser.SetAttrNode("EndDateTime",endDateTime,NVRTaskRecord);
		parser.SetAttrNode("Comment",string("内部错误"),NVRTaskRecord);

	}
	else
	{
		pXMLNODE returnInfoNode = parser.CreateNodePtr(rootnode,"ReturnInfo");  

		//创建ReturnInfo节点
		pXMLNODE taskRecordReportNode=parser.CreateNodePtr(returnInfoNode,"NVRTaskRecordInquiry");  //创建NVRTaskRecordInquiry节点
		pXMLNODE taskRecordNode=parser.CreateNodePtr(taskRecordReportNode,"NVRTaskRecord");         //创建NVRTaskRecord节点

		parser.SetAttrNode("TaskID",task->GetTaskID(),taskRecordReportNode);    //设置NVRTaskRecordInquiry节点的TaskID属性
		parser.SetAttrNode("Index",string("1"),taskRecordNode);                 //设置NVRTaskRecord节点的Index属性
		parser.SetAttrNode("URL",URL,taskRecordNode);                          //设置NVRTaskRecord节点的URL属性
		parser.SetAttrNode("StartDateTime",startDateTime,taskRecordNode);      //设置NVRTaskRecord节点的StartDateTime属性
		parser.SetAttrNode("EndDateTime",endDateTime,taskRecordNode);          //设置NVRTaskRecord节点的EndDateTime属性

	}

	parser.SaveToString(retXml);
	return retXml;
}

//返回任务录像节目文件下载XML
std::string TranslateUpXMLForTHREED::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"NVRTaskRecordDownInquiry");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");                   //创建ReturnInfo节点
	pXMLNODE taskRecordFileReportNode=parser.CreateNodePtr(returnInfoNode,"NVRTaskRecordDownInquiry");   //创建NVRTaskRecordDownInquiry节点

	pXMLNODE taskRecordNode=parser.CreateNodePtr(taskRecordFileReportNode,"NVRTaskRecordDown");   //创建NVRTaskRecordDown节点
		
	parser.SetAttrNode("URL",URL,taskRecordNode);    //设置NVRTaskRecordDown节点的URL属性

	parser.SaveToString(retXml);
	return retXml;
}
//MHP查询XML
std::string TranslateUpXMLForTHREED::TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord)
{
	std::string retXml=GetXmlHeader(task,"MHPQuery");//标准的返回头xml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //创建ReturnInfo节点
	pXMLNODE MHPQueryNode=parser.CreateNodePtr(returnInfoNode,"MHPQuery");               //创建MHPQuery节点
	pXMLNODE MHPNode=parser.CreateNodePtr(MHPQueryNode,"MHP");                           //创建MHP节点

	parser.SetAttrNode("ScanTime",TimeUtil::GetCurDateTime(),MHPQueryNode);        //设置MHPQuery节点的Ftp属性
	parser.SetAttrNode("Ftp",ftp,MHPNode);        //设置MHP节点的Ftp属性
	parser.SetAttrNode("UserName",userName,MHPNode);        //设置MHP节点的UserName属性
	parser.SetAttrNode("Pass",passWord,MHPNode);        //设置MHP节点的Pass属性
	parser.SaveToString(retXml);
	return retXml;

}
//指标补偿设置XML
std::string TranslateUpXMLForTHREED::TranslateQualityCompensationSet(const XMLTask *task)
{
	std::string retXml=GetXmlHeader(task,"IndexCompensationSet");//标准的返回头xml  
	return retXml;
}
//数据业务分析时间设置XML
std::string TranslateUpXMLForTHREED::TranslateAutoAnalysisTimeSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AutoAnalysisTimeSet");//标准的返回头xml
	return retXml;
}
//环境指标查询XML
std::string TranslateUpXMLForTHREED::TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec)
{
	std::string retXml = GetXmlHeader(task,"GetIndexESet");
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //创建ReturnInfo节点
	pXMLNODE getIndexESetNode=parser.CreateNodePtr(returnInfoNode,"GetIndexESet");       //创建GetIndexESet节点
	EnvMapInfo::iterator ptr = EnvMap.begin();
	for (;ptr!=EnvMap.end();ptr++)
	{
		pXMLNODE Node=parser.CreateNodePtr(getIndexESetNode,"GetIndexE");                     
		parser.SetAttrNode("Type",(*ptr).first.Type,Node);        
		parser.SetAttrNode("Desc",(*ptr).first.Desc,Node);        
		parser.SetAttrNode("Value",(*ptr).second,Node);    
		sCheckParam param;
		param.DVBType = task->GetDVBType();
		param.AlarmType = ALARM_ENVIRONMENT;
		param.TypeID = (*ptr).first.Type;
		param.TypeDesc = (*ptr).first.Desc;
		param.STD = "";
		param.Freq = task->GetTaskFreq();
		param.SymbolRate = "";
		param.ChannelID = task->GetChannelID();
		param.TypedValue = (*ptr).second;

		paramVec.push_back(param);

	}
	parser.SaveToString(retXml);
	return retXml;

}
//手动录像设置XML
std::string TranslateUpXMLForTHREED::TranslateManualRecordSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ManualRecordQuery");//标准的返回头xml
	return retXml;
}
//构造返回XML头信息
std::string TranslateUpXMLForTHREED::GetXmlHeader(const XMLTask* task ,std::string retType)
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	string Desc;
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
	default:
		Desc="失败";
		break;
	}

	std::string msgType;//消息类型
	if (task->GetDVBType()== THREED)
		msgType="3DUp";
	else
		return "";

	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg

	headParser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//版本
	//if (retType=="ProvisionalRecordTaskSet"||retType=="NVRVideoHistoryInquiry"||retType=="NVRTaskRecordInquiry")
	//{
	//	headParser.SetAttrNode( "MsgID",task->GetMsgID(),headRootNode );//消息id属性
	//}
	//else
	//	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//消息id属性
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
	headParser.SaveToString( retXml );

	return retXml;
}
//手动录像主动上报
std::string TranslateUpXMLForTHREED::TranslateManualRecordReport( const XMLTask *task,const std::string& strStandXML,std::string URL )
{
	const char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";
	XmlParser parser,temParser(cheader);
	parser.Set_xml(strStandXML);
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,type,protocol,datetime,srccode,dstcode,srcurl,priority;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"DVBType",type);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);

	if(type=="THREED")
	{
		type="3DUp";
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("Type",type,stdRootNode);
	temParser.SetAttrNode("DateTime",TimeUtil::GetCurDateTime(),stdRootNode);
	temParser.SetAttrNode("SrcCode",dstcode,stdRootNode);
	temParser.SetAttrNode("DstCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstURL",task->GetSrcURL(),stdRootNode);
	temParser.SetAttrNode("ReplyID",msgid,stdRootNode);

	pXMLNODE retNode=temParser.GetNodeFromPath("Msg/Return");
	temParser.SetAttrNode("Type",std::string("ManualRecordQuery"),retNode);
	temParser.SetAttrNode("Value",std::string("0"),retNode);
	temParser.SetAttrNode("Desc",std::string("成功"),retNode);

	std::string Index,Time,Path;
	pXMLNODE recordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(recordNode,"DeviceID",Index);
	parser.GetAttrNode(recordNode,"Time",Time);
	parser.GetAttrNode(recordNode,"Path",Path);
	pXMLNODE node=temParser.CreateNodePtr(temParser.CreateNodePtr("Msg","ReturnInfo"),"ManualRecordQuery");
	pXMLNODE retRecordNode=temParser.CreateNodePtr(node,"ManualRecord");
	temParser.SetAttrNode("Index",Index,retRecordNode);
	temParser.SetAttrNode("Time",Time,retRecordNode);
	temParser.SetAttrNode("Path",Path,retRecordNode);
	temParser.SetAttrNode("URL",URL,retRecordNode);

	std::string retXML;
	temParser.SaveToString(retXML);
	return retXML;
}
std::string TranslateUpXMLForTHREED::TranslateEPGQuery(const XMLTask* task,std::string URL)
{
	std::string retXML = GetXmlHeader(task,"EPGQuery");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE retNode = parser.GetNodeFromPath("Msg/Return");
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //创建ReturnInfo节点 


	std::string strURL;

	if (task->GetRetValue() == RUN_SUCCESS)
	{
		//modified by jidushan 11.03.31 : 表共享路径改为通过配置文件获取
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort();
		std::string sharepath;
		PROPMANAGER::instance()->GetTableSharePath(task->GetDVBType(), sharepath);
		strURL += sharepath + URL;
	}

	parser.SetAttrNode("Redirect",strURL,retNode);       //设置EPG路径信息                                
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateFreqAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchFSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE  returnInfoNode = parser.CreateNodePtr("Msg","ReturnInfo");
	pXMLNODE  alarmSetNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchFSet");
	std::string freq,index;
	std::string rtnXML;
	if (vecAlarmResult.empty())
	{
		parser.SetAttrNode(string("Freq"),string(""),alarmSetNode);
		parser.SetAttrNode(string("Index"),string("0"),alarmSetNode);
		parser.SaveToString(rtnXML);
		return rtnXML;
	}
	parser.SetAttrNode(string("Freq"),vecAlarmResult[0].Freq,alarmSetNode);
	parser.SetAttrNode(string("Index"),vecAlarmResult[0].DeviceID,alarmSetNode);

	std::vector<sCheckParam>::iterator ptr = vecAlarmResult.begin();
	for (;ptr!=vecAlarmResult.end();ptr++)
	{
		pXMLNODE alarmSearchNode = parser.CreateNodePtr(alarmSetNode,"AlarmSearchF");
		parser.SetAttrNode(string("Type"),(*ptr).TypeID,alarmSearchNode);
		parser.SetAttrNode(string("Desc"),(*ptr).TypeDesc,alarmSearchNode);
		parser.SetAttrNode(string("Value"),(*ptr).mode,alarmSearchNode);
		parser.SetAttrNode(string("Time"),(*ptr).StrCheckTime,alarmSearchNode);
	}

	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateProgramAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchPSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE  returnInfoNode = parser.CreateNodePtr("Msg","ReturnInfo");
	pXMLNODE  alarmSetNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchPSet");
	std::string freq,index;
	std::string rtnXML;
	if (vecAlarmResult.empty())
	{
		parser.SetAttrNode(string("Freq"),string(""),alarmSetNode);
		parser.SetAttrNode(string("Index"),string("0"),alarmSetNode);
		parser.SetAttrNode(string("ServiceID"),string(""),alarmSetNode);
		parser.SetAttrNode(string("VideoPID"),string(""),alarmSetNode);
		parser.SetAttrNode(string("AudioPID"),string(""),alarmSetNode);
		parser.SaveToString(rtnXML);
		return rtnXML;
	}
	parser.SetAttrNode(string("Freq"),vecAlarmResult[0].Freq,alarmSetNode);
	parser.SetAttrNode(string("Index"),vecAlarmResult[0].DeviceID,alarmSetNode);
	parser.SetAttrNode(string("ServiceID"),vecAlarmResult[0].ServiceID,alarmSetNode);
	parser.SetAttrNode(string("VideoPID"),vecAlarmResult[0].VideoID,alarmSetNode);
	parser.SetAttrNode(string("AudioPID"),vecAlarmResult[0].AudioID,alarmSetNode);

	std::vector<sCheckParam>::iterator ptr = vecAlarmResult.begin();
	for (;ptr!=vecAlarmResult.end();ptr++)
	{
		pXMLNODE alarmSearchNode = parser.CreateNodePtr(alarmSetNode,"AlarmSearchP");
		parser.SetAttrNode(string("Type"),(*ptr).TypeID,alarmSearchNode);
		parser.SetAttrNode(string("Desc"),(*ptr).TypeDesc,alarmSearchNode);
		parser.SetAttrNode(string("Value"),(*ptr).mode,alarmSearchNode);
		parser.SetAttrNode(string("Time"),(*ptr).StrCheckTime,alarmSearchNode);
	}

	parser.SaveToString(rtnXML);
	return rtnXML;
	return "";
}

std::string TranslateUpXMLForTHREED::TranslateEnvAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE retNode = parser.GetNodeFromPath("Msg/Return");
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //创建ReturnInfo节点 
	pXMLNODE alarmNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchESet");

	for(size_t i=0;i<vecAlarmResult.size();i++)
	{
		pXMLNODE childNode = parser.CreateNodePtr(alarmNode,"AlarmSearchE");
		parser.SetAttrNode("Type",vecAlarmResult[i].TypeID,childNode);
		parser.SetAttrNode("Desc",vecAlarmResult[i].TypeDesc,childNode);
		parser.SetAttrNode("Value",vecAlarmResult[i].mode,childNode);
		parser.SetAttrNode("Time",vecAlarmResult[i].StrCheckTime,childNode);
	}                             
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateOSDSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"OSDSet");//标准的返回头xml
	return retXml;
}

std::string TranslateUpXMLForTHREED::TranslateStreamRealtimeRate(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"NVRSteamRateSet");
	return retXML;
}
std::string TranslateUpXMLForTHREED::TranslateRecordCapabilityQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"RecordCapabilityQuery");
	XmlParser parser;
	parser.Set_xml(retXML); 
	list<int> DeviceList;
	int num=0;
	//PROPMANAGER::instance()->GetAllDeviceList(DeviceList);	
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	for(int devi = 0;devi < devnum;devi++)
	{
		bool ret = PROPMANAGER::instance()->IsDeviceAvaiable(devi,"AutoRecord",THREED);
		if(ret)
			num++;

	}
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //创建ReturnInfo节点 
	pXMLNODE QueryNode = parser.CreateNodePtr(returnInfoNode,"RecordCapabilityQuery");
	parser.SetAttrNode("Index",string("0"),QueryNode);
	parser.SetAttrNode("OptiCount",StrUtil::Int2Str(num),QueryNode);
	parser.SetAttrNode("PessCount",StrUtil::Int2Str(num),QueryNode);

	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateHDDefAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"HDDefAudioParamSet");
	return retXML;
}
std::string TranslateUpXMLForTHREED::TranslateAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"AudioParamSet");
	return retXML;
}


std::string TranslateUpXMLForTHREED::TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retxml;
	eAlarmType type=ALARM_NONE;
	if(!vecAlarmResult.empty())
	{
		type=vecAlarmResult[0].AlarmType;
	}
	switch(type)
	{
	case ALARM_ENVIRONMENT:
		retxml=TranslateEnvAlarmQuery(task,vecAlarmResult);
		break;
	case ALARM_PROGRAM:
		retxml=TranslateProgramAlarmQuery(task,vecAlarmResult);
		break;
	case ALARM_FREQ:
	case ALARM_TR101_290:
		retxml=TranslateFreqAlarmQuery(task,vecAlarmResult);
		break;
	}
	return retxml;
}


std::string TranslateUpXMLForTHREED::TranslateStopClient(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"VideoClientStop");//标准的返回头xml
	return retXml;
}