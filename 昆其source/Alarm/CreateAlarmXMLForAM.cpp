
#include "CreateAlarmXMLForAM.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForAM::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string amxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	amxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser amparser(amxml.c_str());

// 	string  amMsgID ;
// 	DBMANAGER::instance()->GetMsgID(amMsgID);
// 	int ammsgid=StrUtil::Str2Int(amMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++ammsgid));

	pXMLNODE amroot=amparser.GetRootNode();
	amparser.SetAttrNode( "Version",string("1"),amroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	amparser.SetAttrNode( "MsgID",MsgID,amroot );//消息id属性
	amparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,amroot );//当前时间
	//amparser.SetAttrNode( "MsgID",ammsgid,amroot );//消息id属性
	//amparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),amroot );//当前时间

	amparser.SetAttrNode( "Type",string("AMUp"),amroot );//消息类型
	amparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(AM),amroot );//本机标识，可通过接口获得
	amparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(AM),amroot );//目标机器标识
	amparser.SetAttrNode( "ReplyID",string("-1"),amroot );//回复的消息id

	pXMLNODE amreportnode=amparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE amalarmnode=amparser.CreateNodePtr(amreportnode,"QualityAlarm");
	amparser.SetAttrNode("EquCode",string(""),amalarmnode);
	amparser.SetAttrNode("AlarmID",alarmid,amalarmnode);
	amparser.SetAttrNode("Mode",alarmvalue,amalarmnode);
	string chcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(AM,checkparam.Freq,chcode);
	amparser.SetAttrNode("ChCode",chcode,amalarmnode);
	amparser.SetAttrNode("Type",checkparam.TypeID,amalarmnode);
	amparser.SetAttrNode("Desc",checkparam.TypeDesc,amalarmnode);
	amparser.SetAttrNode("Reason",string(""),amalarmnode);
	amparser.SetAttrNode("Value",checkparam.TypedValue,amalarmnode);
	amparser.SetAttrNode("CheckTime",TimeUtil::GetCurDateTime(),amalarmnode);
	amparser.SetAttrNode("InRunPlan",string("1"),amalarmnode);

	std::string retXML;
	amparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForAM::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string amxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	amxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser amparser(amxml.c_str());

// 	std::string  amMsgID ;
// 	DBMANAGER::instance()->GetMsgID(amMsgID);
// 	int ammsgid=StrUtil::Str2Int(amMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++ammsgid));

	pXMLNODE amroot=amparser.GetRootNode();
	amparser.SetAttrNode( "Version",string("1"),amroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	amparser.SetAttrNode( "MsgID",MsgID,amroot );//消息id属性
	amparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,amroot );//当前时间
	//amparser.SetAttrNode( "MsgID",ammsgid,amroot );//消息id属性
	//amparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),amroot );//当前时间

	amparser.SetAttrNode( "Type",string("AMUp"),amroot );//消息类型
	amparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(AM),amroot );//本机标识，可通过接口获得
	amparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(AM),amroot );//目标机器标识
	amparser.SetAttrNode( "ReplyID",string("-1"),amroot );//回复的消息id

	pXMLNODE amreportnode=amparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE amalarmnode=amparser.CreateNodePtr(amreportnode,"QualityAlarm");
	//amparser.SetAttrNode("EquCODE",string(""),amalarmnode);
	amparser.SetAttrNode("AlarmID",alarmid,amalarmnode);
	amparser.SetAttrNode("Mode",alarmvalue,amalarmnode);
	string channelcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(AM,checkparam.ChannelID,channelcode);
	amparser.SetAttrNode("ChCode",channelcode,amalarmnode);
	amparser.SetAttrNode("Type",checkparam.TypeID,amalarmnode);
	amparser.SetAttrNode("Desc",checkparam.TypeDesc,amalarmnode);
	amparser.SetAttrNode("Reason",string(""),amalarmnode);
	amparser.SetAttrNode("Value",string("1"),amalarmnode);
	amparser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),amalarmnode);
	amparser.SetAttrNode("InRunPlan",string("1"),amalarmnode);

	std::string retXML;
	amparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForAM::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	XmlParser parser(xml.c_str());
// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("1"),root );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	//parser.SetAttrNode( "MsgID",msgid,root );//消息id属性
	//parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),root );//当前时间

	parser.SetAttrNode( "Type",string("AMUp"),root );//消息类型
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(AM),root );//本机标识，可通过接口获得
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(AM),root );//目标机器标识
	parser.SetAttrNode( "ReplyID",string("-1"),root );//回复的消息id
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Type",string("AlarmSearchPSet"),RetNode);
	parser.SetAttrNode("Value",string("0"),RetNode);
	parser.SetAttrNode("Desc",string("成功"),RetNode);
	node=parser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchPSet");
	parser.SetAttrNode("Index",checkparam.DeviceID,SetNode);
	parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
	parser.SetAttrNode("ServiceID",string(""),SetNode);
	parser.SetAttrNode("VideoPID",string(""),SetNode);
	parser.SetAttrNode("AudioPID",string(""),SetNode);
	parser.SetAttrNode("PrgName",string(""),SetNode);
	childnode=parser.CreateNodePtr(SetNode,"AlarmSearchP");
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);
	parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}