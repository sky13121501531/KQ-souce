
#include "CreateAlarmXMLForRADIO.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForRADIO::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string radioxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	radioxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser radioparser(radioxml.c_str());

// 	string  radioMsgID ;
// 	DBMANAGER::instance()->GetMsgID(radioMsgID);
// 	int radiomsgid=StrUtil::Str2Int(radioMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++radiomsgid));

	pXMLNODE radioroot=radioparser.GetRootNode();
	radioparser.SetAttrNode( "Version",string("1"),radioroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	radioparser.SetAttrNode( "MsgID",MsgID,radioroot );//消息id属性
	radioparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,radioroot );//当前时间
	//radioparser.SetAttrNode( "MsgID",radiomsgid,radioroot );//消息id属性
	//radioparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),radioroot );//当前时间

	radioparser.SetAttrNode( "Type",string("RadioUp"),radioroot );//消息类型
	radioparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),radioroot );//本机标识，可通过接口获得
	radioparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),radioroot );//目标机器标识
	radioparser.SetAttrNode( "ReplyID",string("-1"),radioroot );//回复的消息id

	pXMLNODE radioreportnode=radioparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE radioalarmnode=radioparser.CreateNodePtr(radioreportnode,"QualityAlarm");
	radioparser.SetAttrNode("EquCode",string(""),radioalarmnode);
	radioparser.SetAttrNode("AlarmID",alarmid,radioalarmnode);
	radioparser.SetAttrNode("Mode",alarmvalue,radioalarmnode);
	string chcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,checkparam.Freq,chcode);
	radioparser.SetAttrNode("ChCode",chcode,radioalarmnode);
	radioparser.SetAttrNode("Type",checkparam.TypeID,radioalarmnode);
	radioparser.SetAttrNode("Desc",checkparam.TypeDesc,radioalarmnode);
	radioparser.SetAttrNode("Reason",string(""),radioalarmnode);
	radioparser.SetAttrNode("Value",checkparam.TypedValue,radioalarmnode);
	radioparser.SetAttrNode("CheckTime",TimeUtil::GetCurDateTime(),radioalarmnode);
	radioparser.SetAttrNode("InRunPlan",string("1"),radioalarmnode);

	std::string retXML;
	radioparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForRADIO::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string radioxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	radioxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser radioparser(radioxml.c_str());

// 	string  radioMsgID ;
// 	DBMANAGER::instance()->GetMsgID(radioMsgID);
// 	int radiomsgid=StrUtil::Str2Int(radioMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++radiomsgid));

	pXMLNODE radioroot=radioparser.GetRootNode();
	radioparser.SetAttrNode( "Version",string("1"),radioroot );//版本

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	radioparser.SetAttrNode( "MsgID",MsgID,radioroot );//消息id属性
	radioparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,radioroot );//当前时间
	//radioparser.SetAttrNode( "MsgID",radiomsgid,radioroot );//消息id属性
	//radioparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),radioroot );//当前时间

	radioparser.SetAttrNode( "Type",string("RadioUp"),radioroot );//消息类型
	radioparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),radioroot );//本机标识，可通过接口获得
	radioparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),radioroot );//目标机器标识
	radioparser.SetAttrNode( "ReplyID",string("-1"),radioroot );//回复的消息id

	pXMLNODE radioreportnode=radioparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE radioalarmnode=radioparser.CreateNodePtr(radioreportnode,"QualityAlarm");
	//radioparser.SetAttrNode("EquCODE",string(""),radioalarmnode);
	radioparser.SetAttrNode("AlarmID",alarmid,radioalarmnode);
	radioparser.SetAttrNode("Mode",alarmvalue,radioalarmnode);
	string channelcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,checkparam.ChannelID,channelcode);
	radioparser.SetAttrNode("ChCode",channelcode,radioalarmnode);
	radioparser.SetAttrNode("Type",checkparam.TypeID,radioalarmnode);
	radioparser.SetAttrNode("Desc",checkparam.TypeDesc,radioalarmnode);
	radioparser.SetAttrNode("Reason",string(""),radioalarmnode);
	radioparser.SetAttrNode("Value",string("1"),radioalarmnode);
	radioparser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),radioalarmnode);
	radioparser.SetAttrNode("InRunPlan",string("1"),radioalarmnode);

	std::string retXML;
	radioparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForRADIO::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
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

	parser.SetAttrNode( "Type",string("RadioUp"),root );//消息类型
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),root );//本机标识，可通过接口获得
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),root );//目标机器标识
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