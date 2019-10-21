
#include "CreateAlarmXMLForDVBC.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/Config.h"
extern time_t AlarmTime;
std::string CreateAlarmXMLForDVBC::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		xml+="<Msg></Msg>";
	}
	else
	{
		xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	}
	XmlParser parser(xml.c_str());

	string strDateTime = TimeUtil::GetCurDateTime();
 	string  MsgID ;
 	DBMANAGER::instance()->GetMsgID(MsgID);
 	int msgid=StrUtil::Str2Int(MsgID);
 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	pXMLNODE root=parser.GetRootNode();
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		parser.SetAttrNode( "Version",string("3.0"),root );//版本
	}
	else
	{
		parser.SetAttrNode( "Version",string("2.6"),root );//版本
	}
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	//
	std::string strSrcCode;
	std::string strDesCode;
	std::string strUrl;
	if(checkparam.DVBType == DVBC)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBC_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBCUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == DVBS)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBS_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBSUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == CTTB)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DTMB_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DTMBUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == RADIO)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("FM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("FMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == AM)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("AM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("AMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	//
	parser.SetAttrNode( "ReplyID",string("-1"),root );//回复的消息id
	pXMLNODE node=NULL;
	pXMLNODE childnode=NULL;

	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		//
		pXMLNODE RetNode=parser.GetNodeFromPath("Msg");
		pXMLNODE SetNode;
		if(checkparam.AlarmType == ALARM_TR101_290)
		{
			pXMLNODE RFNode=parser.CreateNodePtr(RetNode,"AlarmStreamReport");
			SetNode=parser.CreateNodePtr(RFNode,"AlarmStream");
		}
		else
		{
			pXMLNODE RFNode=parser.CreateNodePtr(RetNode,"AlarmRFReport");
			SetNode=parser.CreateNodePtr(RFNode,"AlarmRF");
		}
		parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
		childnode=parser.CreateNodePtr(SetNode,"Alarm");
		parser.SetAttrNode("Type",checkparam.TypeID,childnode);
			
		std::string error;
		if (alarmvalue=="0")
		{
			parser.SetAttrNode("Value",(string)"1",childnode);
			error=checkparam.TypeDesc+"报警";
		}
		else
		{
			parser.SetAttrNode("Value",(string)"2",childnode);
			error=checkparam.TypeDesc+"报警解除";
		}
		parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(time(0)),childnode);
		parser.SetAttrNode("Desc",error,childnode);
		parser.SetAttrNode("AlarmID",checkparam.AlarmID,childnode);
	}
	else
	{
		pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
		parser.SetAttrNode("Type",string("AlarmSearchFSet"),RetNode);
		parser.SetAttrNode("Value",string("0"),RetNode);
		parser.SetAttrNode("Desc",string("成功"),RetNode);
		node=parser.GetNodeFromPath("Msg/ReturnInfo");
		pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchFSet");
		parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
		childnode=parser.CreateNodePtr(SetNode,"AlarmSearchF");
		parser.SetAttrNode("Type",checkparam.TypeID,childnode);
			
		std::string error;
		if (alarmvalue=="0")
		{
			parser.SetAttrNode("Value",(string)"1",childnode);
			error=checkparam.TypeDesc+"报警";
		}
		else
		{
			parser.SetAttrNode("Value",(string)"2",childnode);
			error=checkparam.TypeDesc+"报警解除";
		}
		parser.SetAttrNode("AlarmID",checkparam.AlarmID,childnode);
		parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(time(0)),childnode);
		parser.SetAttrNode("Desc",error,childnode);
	}


	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;

}
std::string CreateAlarmXMLForDVBC::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		xml+="<Msg></Msg>";
	}
	else
	{
		xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	}

	string strDateTime = TimeUtil::GetCurDateTime();

 	string  MsgID ;
 	DBMANAGER::instance()->GetMsgID(MsgID);
 	int msgid=StrUtil::Str2Int(MsgID);
 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		parser.SetAttrNode( "Version",string("3.0"),root );//版本
	}
	else
	{
		parser.SetAttrNode( "Version",string("2.6"),root );//版本
	}
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	//
	std::string strSrcCode;
	std::string strDesCode;
	std::string strUrl;
	if(checkparam.DVBType == DVBC)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBC_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBCUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == DVBS)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBS_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBSUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == CTTB)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DTMB_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DTMBUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == RADIO)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("FM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("FMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == AM)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("AM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("AMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	//
	parser.SetAttrNode( "ReplyID",string("-1"),root );//回复的消息id
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	std::string apid,vpid,serviceid,prgname;
	CHANNELINFOMGR::instance()->GetChannelInfo(checkparam.DVBType,checkparam.ChannelID,vpid,apid,serviceid,prgname);
	if((PROPMANAGER::instance()->GetCurWorkVersion() != "3.0")&&(PROPMANAGER::instance()->GetCurWorkVersion() != "3.9")&&(PROPMANAGER::instance()->GetCurWorkVersion() != "4.0")&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
	{
		pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
		parser.SetAttrNode("Type",string("AlarmSearchPSet"),RetNode);
		parser.SetAttrNode("Value",string("0"),RetNode);
		parser.SetAttrNode("Desc",string("成功"),RetNode);
		node=parser.GetNodeFromPath("Msg/ReturnInfo");
		pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchPSet");
		parser.SetAttrNode("Index",checkparam.DeviceID,SetNode);
		parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
		parser.SetAttrNode("ServiceID",serviceid,SetNode);
		parser.SetAttrNode("VideoPID",vpid,SetNode);
		parser.SetAttrNode("AudioPID",apid,SetNode);
		childnode=parser.CreateNodePtr(SetNode,"AlarmSearchP");
		parser.SetAttrNode("Type",checkparam.TypeID,childnode);
		parser.SetAttrNode("AlarmID",alarmid,childnode);
	}
	else
	{
		node=parser.GetNodeFromPath("Msg");
		pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmChannelReport");
		childnode=parser.CreateNodePtr(SetNode,"AlarmChannel");
		parser.SetAttrNode("Freq",checkparam.Freq,childnode);
		parser.SetAttrNode("ServiceID",serviceid,childnode);
		parser.SetAttrNode("AlarmID",alarmid,childnode);
		pXMLNODE TranschanNode = parser.CreateNodePtr(childnode, "TranscodeInfo");
		pXMLNODE TranschanNodeInfoNode = parser.CreateNodePtr(TranschanNode, "Info");
		CoderDev CoderInfo;
		PROPMANAGER::instance()->GetCoderInfo(StrUtil::Str2Int(checkparam.DeviceID),CoderInfo);
		parser.SetAttrNode("ChassisID", CoderInfo.s_monitorindex, TranschanNodeInfoNode);
		parser.SetAttrNode("Position", CoderInfo.s_cardindex, TranschanNodeInfoNode);
		parser.SetAttrNode("ModuleID", CoderInfo.s_modindex, TranschanNodeInfoNode);
		parser.SetAttrNode("Transcode", CoderInfo.s_modcoderindex, TranschanNodeInfoNode);
		//
	}
	std::string error;
	if (alarmvalue=="0")
	{
		parser.SetAttrNode("Value",(string)"1",childnode);
		error=checkparam.TypeDesc+"报警";
	}
	else
	{
		parser.SetAttrNode("Value",(string)"2",childnode);
		error=checkparam.TypeDesc+"报警解除";
	}
	parser.SetAttrNode("Type", checkparam.TypeID, childnode);
	parser.SetAttrNode("Desc",error,childnode);

    if(checkparam.TypeDesc=="失锁")
    {
        if(checkparam.mode=="0")
        {
            time_t time_0  = checkparam.CheckTime - 30;
            parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(time_0),childnode);//extern time_t AlarmTime;
        }
        else
        {
            time_t time_1 = checkparam.CheckTime + 30;
            parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(time_1),childnode);//extern time_t AlarmTime;
        }
    }
    else
    {
        parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);//extern time_t AlarmTime;
    }
	string retXML = "";
	parser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForDVBC::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	XmlParser parser(xml.c_str());
// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("2.6"),root );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	//parser.SetAttrNode( "MsgID",msgid,root );//消息id属性
	//parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),root );//当前时间

	parser.SetAttrNode( "Type",string("MonUp"),root );//消息类型
	if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetAgentInfoSet("6"),root );//本机标识，可通过接口获得
	}
	else
	{
		parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBC),root );//本机标识，可通过接口获得
	}
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBC),root );//目标机器标识
	parser.SetAttrNode( "ReplyID",string("-1"),root );//回复的消息id
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Type",string("AlarmSearchPSet"),RetNode);
	parser.SetAttrNode("Value",string("0"),RetNode);
	parser.SetAttrNode("Desc",string("成功"),RetNode);
	node=parser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchPSet");
	//parser.SetAttrNode("Index",checkparam.DeviceID,SetNode);
	parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
	parser.SetAttrNode("ServiceID",string(""),SetNode);
	parser.SetAttrNode("VideoPID",string(""),SetNode);
	parser.SetAttrNode("AudioPID",string(""),SetNode);
	//parser.SetAttrNode("PrgName",string(""),SetNode);
	childnode=parser.CreateNodePtr(SetNode,"AlarmSearchP");
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);
	parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForDVBC::CreateEquipmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		xml+="<Msg></Msg>";
	}
	else
	{
		xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	}
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		parser.SetAttrNode( "Version",string("3.0"),root );//版本
	}
	else
	{
		parser.SetAttrNode( "Version",string("2.6"),root );//版本
	}

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime,root );//当前时间
	//
	std::string strSrcCode;
	std::string strDesCode;
	std::string strUrl;

	/*if(checkparam.DVBType == DVBC)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBC_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBCUp"),root );

		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == DVBS)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBS_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBSUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == CTTB)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DTMB_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DTMBUp"),root );

		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == RADIO)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("FM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("FMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	else if(checkparam.DVBType == AM)
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("AM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("AMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}*/
	if(checkparam.DeviceID == "0")
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DTMB_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DTMBUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	if(checkparam.DeviceID == "1")
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("DVBC_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("DVBCUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	if(checkparam.DeviceID == "2")
	{
		PROPMANAGER::instance()->GetAlarmUpInfoByType("FM_ALARMSENDUP",strSrcCode,strDesCode,strUrl);
		parser.SetAttrNode( "Type",string("FMUp"),root );
		parser.SetAttrNode( "SrcCode",strSrcCode,root );
		parser.SetAttrNode( "DstCode",strDesCode,root );
	}
	//
	parser.SetAttrNode( "ReplyID",string("-1"),root );//回复的消息id
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	if((PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")||(PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		node=parser.GetNodeFromPath("Msg");
		//AlarmEquipmentReport
		pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmEquipmentReport");
		childnode=parser.CreateNodePtr(SetNode,"AlarmInfo");
		parser.SetAttrNode("Type",string("1"),childnode);
		if (checkparam.mode=="0")
		{
			parser.SetAttrNode("Value",(string)"1",childnode);
		}
		else
		{
			parser.SetAttrNode("Value",(string)"2",childnode);
		}
		parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
		CoderDev CoderInfo;
		PROPMANAGER::instance()->GetCoderInfo(StrUtil::Str2Int(checkparam.DeviceID),CoderInfo);
		//string strChassisID = StrUtil::Int2Str(StrUtil::Str2Int(checkparam.DeviceID) + 1);
		parser.SetAttrNode("ChassisID", checkparam.DeviceID, childnode);
		parser.SetAttrNode("Describe",checkparam.TypeDesc,childnode);
		parser.SetAttrNode("AlarmID",checkparam.AlarmID,childnode);
	}
	else
	{
		pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
		parser.SetAttrNode("Type",string("EquipmentBreakDownSet"),RetNode);
		parser.SetAttrNode("Value",string("0"),RetNode);
		parser.SetAttrNode("Desc",string("成功"),RetNode);
		node=parser.GetNodeFromPath("Msg/ReturnInfo");
		pXMLNODE SetNode=parser.CreateNodePtr(node,"BreakDownQuery");
		childnode=parser.CreateNodePtr(SetNode,"BreakInfo");
		parser.SetAttrNode("BreakType",string("2"),childnode);
		parser.SetAttrNode("Value",checkparam.mode,childnode);
		parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
		parser.SetAttrNode("Index",string("1"),childnode);
		parser.SetAttrNode("Position",checkparam.DeviceID,childnode);
		parser.SetAttrNode("Describe",checkparam.TypeDesc,childnode);
	}
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}