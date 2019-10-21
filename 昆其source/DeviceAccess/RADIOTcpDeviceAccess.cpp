
#include "./RADIOTcpDeviceAccess.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/ChannelInfoMgr.h"


RadioTcpDeviceAccess::RadioTcpDeviceAccess(int deviceid,std::string strIP,int nPort) : TCPDeviceAccess(deviceid, strIP, nPort)
{	
}
RadioTcpDeviceAccess::~RadioTcpDeviceAccess()	
{
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
}
bool RadioTcpDeviceAccess::GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	vector<QualityDesc> vecDesc;
	char* source="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser Retparser(source);
	float freq,f_value=0.0f;
	string StrFreq;
	pXMLNODE QuaryNode = parser.GetNodeFirstChild(root);
	parser.GetAttrNode(QuaryNode,"Freq",StrFreq);
	pXMLNODE SrcparamNode = parser.GetNodeFromPath("Msg/QualityParam");
	pXMLNODELIST paramList = parser.GetNodeList(SrcparamNode);
	freq =StrUtil::Str2Float(StrFreq);
	RadioQuaRetMessage_Obj rqr;
	bool bRadioQua=false;
	int ff[7]={0,0,0,0,0,0,0};
	int count = paramList->Size();
	for(int k=0;k<count;k++)
	{
		std::string Type,Desc,Value;
		QualityDesc TempDesc;
		pXMLNODE childNode = parser.GetNextNode(paramList);
		parser.GetAttrNode(childNode,"Type",Type);
		parser.GetAttrNode(childNode,"Desc",Desc);
		parser.GetAttrNode(childNode,"Value",Value);
		int i_type=StrUtil::Str2Int(Type);

		if(dvbtype=="RADIO")
		{
			string scantype=PROPMANAGER::instance()->GetScanType(RADIO);
			if(scantype=="2")//针对陕西项目作的特殊处理，从开路电视指标卡获得广播的电平值，(场强卡+中短波采集卡)指标设备用2
			{
				int temfreq = (int)(freq*1000.0);
				int level = SendCmdToTVCom(MSG_GET_QUA,(void *)&temfreq,sizeof(int));
				if(level>0)
				{
					f_value=level*10;
				}
				else
				{
					f_value=0.0f;
				}

			}
			else
			{
				if(!bRadioQua)
				{
					if(GetQuality(freq,rqr))
						bRadioQua=true;
				}	
				if(bRadioQua)
				{
					switch(i_type)
					{
					case 1:
						f_value=rqr.level_int;		//信号电平
						if(f_value<6)
							f_value=0.0f;
						break;
					case 2:
						f_value = rqr.dev_int;		//调制度
						break;
					case 3:
						f_value=rqr.dF;				//频偏
						break;
					case 4:
						f_value=6000;
						break;
					case 5:
						f_value=8000;
						break;
					case 6:
						f_value=7000;
						break;
					}
				}
				else
				{
					f_value =0.0f;
				}
			}
		}
		else
			return false;
		
		ff[k]=(int)f_value;
		TempDesc.Desc=Desc;
		TempDesc.Type=Type;
		TempDesc.Value=StrUtil::Float2Str(f_value);
		vecDesc.push_back(TempDesc);
	}
	pXMLNODE retrootNode=Retparser.GetRootNode();
	Retparser.SetAttrNode("DVBType",dvbtype,retrootNode);

	pXMLNODE retNode=Retparser.CreateNodePtr(retrootNode,"Return");
	Retparser.SetAttrNode("Type",string("QualityQuery"),retNode);
	Retparser.SetAttrNode("Value",string("0"),retNode);
	Retparser.SetAttrNode("Desc",string("成功"),retNode);
	Retparser.SetAttrNode("Comment",string(""),retNode);

	pXMLNODE reportNode=Retparser.CreateNodePtr(retrootNode,"QualityQueryReport");

	Retparser.SetAttrNode("STD",string(""),reportNode);
	if (dvbtype=="RADIO")
//	{
		Retparser.SetAttrNode("Freq",StrUtil::Float2Str1(freq),reportNode);
// 	}
// 	else
// 	{
// 		Retparser.SetAttrNode("Freq",StrUtil::Float2Str(freq),reportNode);
// 	}

	Retparser.SetAttrNode("SymbolRate",string(""),reportNode);

	pXMLNODE paramNode=Retparser.CreateNodePtr(reportNode,"QualityParam");
	for(int i=0;i<count;i++)
	{
		pXMLNODE indexNode=Retparser.CreateNodePtr(paramNode,"QualityIndex");
		Retparser.SetAttrNode("Type",vecDesc[i].Type,indexNode);
		Retparser.SetAttrNode("Desc",vecDesc[i].Desc,indexNode);
		Retparser.SetAttrNode("Value",vecDesc[i].Value,indexNode);
	}

	Retparser.SaveToString(strRetMsg);
	return true;
}



bool RadioTcpDeviceAccess::GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	vector<SpectrumInfo> SpectrumVec;
	std::string StartFreq,EndFreq,StepFreq;
	pXMLNODE QueryNode = parser.GetNodeFromPath("Msg/SpectrumQuery");
	parser.GetAttrNode(QueryNode,"EndFreq",EndFreq);
	parser.GetAttrNode(QueryNode,"StartFreq",StartFreq);
	parser.GetAttrNode(QueryNode,"StepFreq",StepFreq);
	pXMLNODE ParamNode=parser.GetNodeFromPath("Msg/SpectrumParam");
	pXMLNODELIST NodeList =parser.GetNodeList(ParamNode);
	int ListCount =parser.GetChildCount(ParamNode);
	for(int i=0;i<ListCount;i++)
	{
		OSFunction::Sleep(0,50);
		SpectrumInfo Temp;
		string freq,level;
		pXMLNODE IndexNode=parser.GetNextNode(NodeList);
		parser.GetAttrNode(IndexNode,"Freq",freq);

		if(dvbtype =="RADIO")
		{
			string scantype=PROPMANAGER::instance()->GetScanType(RADIO);
			if(scantype=="2")//(场强卡+中短波采集卡)指标设备用2
			{
				SpecialRadiopayload_Obj obj;
				SpecialRadioRetMessage_Obj Retobj;
				obj.startfreq = (int)(StrUtil::Str2Float(StartFreq)*1000.0);
				obj.endfreq   = (int)(StrUtil::Str2Float(EndFreq)*1000.0);
				obj.step = (int)(StrUtil::Str2Float(StepFreq)*1000.0);
				for(int temfreq=obj.startfreq;temfreq<obj.endfreq;)
				{
					int level = SendCmdToTVCom(MSG_GET_QUA,(void *)&temfreq,sizeof(int));
					Temp.freq=temfreq;
					Temp.level=(float)(level)/100.0;
					SpectrumVec.push_back(Temp);
					temfreq=temfreq+obj.step;
				}
				break;
			}
			else
			{
				float ff = (float)StrUtil::Str2Float(freq);
				RadioSpecRetMessage_Obj rsr;
				if(GetRadioSpecResult(ff,rsr))
				{
					float fnowfreq = 0; 
					if(rsr.freq>86000)
					{
						for(int tt=0;tt<64;tt++)	//64个频点
						{
							double sum=0;
							float tempfreq = (rsr.freq-3.66*512) + tt*3.66*16+8*3.66;	//计算频点
							if(tempfreq>1000*StrUtil::Str2Float(EndFreq))
								break;
							for(int kk=0;kk<16;kk++)	
							{
								float templevel=0.0f;
								if(kk==0&&tt==0)
								{
									templevel=rsr.spec[1];
								}
								else 
								{
									templevel=rsr.spec[tt*16+kk];
								}
								double temp=pow(10.0,(double)(templevel/10));
								sum+=temp;		
							}
							if(tempfreq>108040) break;
							double finallevel=10*log10(sum);
							Temp.freq=tempfreq;
							Temp.level =(float)finallevel;
							SpectrumVec.push_back(Temp);
						}
					}
				}
			}

		}
		else
			return false;
	}

	char* source="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser Retparser(source);

	pXMLNODE retrootNode=Retparser.GetRootNode();
	Retparser.SetAttrNode("DVBType",dvbtype,retrootNode);

	pXMLNODE retNode=Retparser.CreateNodePtr(retrootNode,"Return");
	Retparser.SetAttrNode("Type",string("SpectrumQuery"),retNode);
	Retparser.SetAttrNode("Value",string("0"),retNode);
	Retparser.SetAttrNode("Desc",string("成功"),retNode);
	Retparser.SetAttrNode("Comment",string(""),retNode);

	pXMLNODE reportNode=Retparser.CreateNodePtr(retrootNode,"SpectrumQueryReport");

	Retparser.SetAttrNode("STD",string(""),reportNode);
	Retparser.SetAttrNode("SymbolRate",string(""),reportNode);

	pXMLNODE paramNode=Retparser.CreateNodePtr(reportNode,"SpectrumParam");

	for(int k=0;k<SpectrumVec.size();k++)
	{
		pXMLNODE indexNode=Retparser.CreateNodePtr(paramNode,"SpectrumIndex");
		Retparser.SetAttrNode("Freq",StrUtil::Float2Str(SpectrumVec[k].freq),indexNode);
		Retparser.SetAttrNode("Value",StrUtil::Float2Str(SpectrumVec[k].level),indexNode);
	}

	Retparser.SaveToString(strRetMsg);
	return true;
}



bool RadioTcpDeviceAccess::GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	string retRADIOxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					   <Msg DVBType=\"RADIO\" >\
					   <Return Type=\"\" Value=\"0\" Desc=\"成功\" Comment=\"\"/> \
					   <ChannelScan></ChannelScan></Msg>";
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	if(dvbtype=="RADIO")
	{
		XmlParser retpaser(retRADIOxml.c_str());
		pXMLNODE channscannode=parser.GetNodeFirstChild(childnode);
		string strstartfreq,strendfreq;
		parser.GetAttrNode(channscannode,"StartFreq",strstartfreq);
		parser.GetAttrNode(channscannode,"EndFreq",strendfreq);

		float ResultFreq=0.0;
		int centerfreqs[40]={0};
		int pFreq[300]={0};
		RadioSpecRetMessage_Obj SpecRetObj[40];
		int len=0,lenth=0;
		int startfreq=StrUtil::Str2Float(strstartfreq)*1000,endfreq=StrUtil::Str2Float(strendfreq)*1000;

		string scantype=PROPMANAGER::instance()->GetScanType(RADIO);
		if(scantype=="2")
		{
			SpecialRadiopayload_Obj obj;
			SpecialRadioRetMessage_Obj Retobj;
			obj.startfreq = startfreq;
			obj.endfreq   = endfreq;
			obj.step = 100;
			PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,obj.chan);
			int ret=SendCmdForRadioSpecialChannelScan(MSG_GET_SPECIAL_AUDIO_CHANSCAN,&obj,&Retobj,sizeof(SpecialRadiopayload_Obj));
			if(ret<0)
			{
				strRetMsg="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
						  <Msg DVBType=\"RADIO\" >\
						  <Return Type=\"\" Value=\"1\" Desc=\"失败\" Comment=\"\"/> \
						  <ChannelScan></ChannelScan></Msg>";
				return false;
			}
			else
			{
				pXMLNODE retchannode=retpaser.GetNodeFromPath("Msg/ChannelScan");
				for(int k=0;k<Retobj.len;k++)
				{

					ResultFreq=((float)(Retobj.value[k]))/1000;
					string tmpResult=StrUtil::Float2Str1(ResultFreq);

					pXMLNODE freqnode=retpaser.CreateNodePtr(retchannode,"Channel");
					retpaser.SetAttrNode("Freq",tmpResult,freqnode);
				}
				retpaser.SaveToString(strRetMsg);
				return true;
			}
		}
		else
		{
			CaculateCenterFreq(startfreq,endfreq,centerfreqs,len);

			for(int i=0;i<len;i++)
			{
				if(SpecScan(centerfreqs[i],SpecRetObj[i]))
				{
					int num=Findpolars(&SpecRetObj[i].spec[1],centerfreqs[i]/100,&pFreq[lenth]);
					lenth+=num;
				}
				else
				{
					continue;
				}
			}		

			if(lenth==0)
			{
				strRetMsg="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
						  <Msg DVBType=\"RADIO\" >\
						  <Return Type=\"\" Value=\"1\" Desc=\"失败\" Comment=\"\"/> \
						  <ChannelScan></ChannelScan></Msg>";
				return false;
			}

			for(int j=lenth-1;j>=0;j--)
			{
				if(pFreq[j]>endfreq)
					lenth--;
				else
					break;
			}
			pXMLNODE retchannode=retpaser.GetNodeFromPath("Msg/ChannelScan");
			for(int k=0;k<lenth;k++)
			{

				ResultFreq=((float)(pFreq[k]))/1000;
				string tmpResult=StrUtil::Float2Str1(ResultFreq);

				pXMLNODE freqnode=retpaser.CreateNodePtr(retchannode,"Channel");
				retpaser.SetAttrNode("Freq",tmpResult,freqnode);
			}

			retpaser.SaveToString(strRetMsg);
			return true;
		}
	}
	else
		return false;
	
	return true;
}


bool RadioTcpDeviceAccess::GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	string retsuccxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"\" >\
					  <Return Type=\"TSQuery\" Value=\"0\" Desc=\"成功\" Comment=\"\"/> \</Msg>";
	string retfailxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"\" >\
					  <Return Type=\"TSQuery\" Value=\"1\" Desc=\"失败\" Comment=\"\"/> \</Msg>";

	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	pXMLNODE tsnode=parser.GetNodeFromPath("Msg/TSQuery/TS");
	string freq,bps;
	parser.GetAttrNode(tsnode,"Bps",bps);
	parser.GetAttrNode(tsnode,"Freq",freq);

	MediaConfig_Obj media_obj;
	TunerConfig_Obj tuner_obj;

	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,media_obj.chan);		
	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,tuner_obj.chan);

	if (dvbtype == string("RADIO"))
	{
		tuner_obj.freq=int(StrUtil::Str2FloatForFMFreq(freq)*1000)+100;
		media_obj.audio_idx=GetAudioIndex(StrUtil::Str2Int(bps));
	}

	if (dvbtype == string("RADIO")&& TurnFreq(tuner_obj) && SetAudioRate(media_obj))
	{
		string name;
		CHANNELINFOMGR::instance()->GetProNameByFreq(OSFunction::GetEnumDVBType(dvbtype),freq,name);
		SubtitleConfig_Obj osdinfo;

		if ((name.empty() && name.length()< 39))
		{
			memcpy(osdinfo.subtitle0,"未知频道",8);
		}
		else
		{
			memcpy(osdinfo.subtitle0,name.c_str(),name.length());
		}
		OSDInfo osd;
		eDVBType etype=OSFunction::GetEnumDVBType(dvbtype);
		PROPMANAGER::instance()->GetOSDInfo(etype,"0",osd);	//模拟无高清
		if(osd.Info.length()<1)
			osd.Info="金石.北京";
		memcpy(osdinfo.subtitle1,osd.Info.c_str(),40);
		osdinfo.size=StrUtil::Str2Int(osd.FontSize);
		osdinfo.subtitle0_x=StrUtil::Str2Int(osd.ProgramX);
		osdinfo.subtitle0_y=StrUtil::Str2Int(osd.ProgramY);
		osdinfo.subtitle1_x=StrUtil::Str2Int(osd.InfoX);
		osdinfo.subtitle1_y=StrUtil::Str2Int(osd.InfoY);
		osdinfo.time_x=StrUtil::Str2Int(osd.TimeX);
		osdinfo.time_y=StrUtil::Str2Int(osd.TimeY);
		osdinfo.mode=StrUtil::Str2Int(osd.TimeType);
		SetOSD(tuner_obj.chan,osdinfo);
		SetSysTime();

		strRetMsg=retsuccxml;
		return true;
	}
	else
	{
		strRetMsg=retfailxml;
		return false;
	}
	return true;
}


int RadioTcpDeviceAccess::SendCmdForRadioSpecialChannelScan(MSG_TYPE msg_type,void* info,SpecialRadioRetMessage_Handle pRetObj, int infolen)
{
	if ( ConnectToServer() == false)
		return -1;
	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[1024] = {0};
	//信息头
	PkgHeader_Obj   ph;

	ph.header   = 0x48;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//信息内容
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;

	string sysmsg;

	ACE_Time_Value sendtimeout(10);
	if(stream.send((char*)sendbuf, 200,&sendtimeout)<=0)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -1;
	}

	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	int len = -1;

	ACE_Time_Value TimeOut(60);

	len =stream.recv((char*)&RetMsg,sizeof(RetMsg),&TimeOut);

	if(len<=0)
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]数据接收失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -1;
	}

	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]数据接收成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	SpecialRadioRetMessage_Handle rm = (SpecialRadioRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x49 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]返回数据错误");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->status=rm->status;
	pRetObj->len = rm->len;
	for(int i=0;i<(pRetObj->len);i++)
	{
		pRetObj->value[i]=rm->value[i];
	}

	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]频道扫描成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	return 1;
}


int RadioTcpDeviceAccess::SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen)
{
	if ( ConnectToServer() == false)
		return -1;
	int pkgLen = 0;
	char sendbuf[200] = {0}; 
	char RetMsg[2000] = {0};

	//信息头
	PkgHeader_Obj   ph;

	ph.header   = 0x53;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//信息内容
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;
	//发送信息
	ACE_Time_Value SendTimeOut(10);
	ACE_Time_Value RecvTimeOut (60);

	string sysmsg;

	if(stream.send((char*)sendbuf, pkgLen,&SendTimeOut) <= 0)		//int lenth = send(m_sSocket,(char*)sendbuf, len, 0)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -2;
	}
	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	//接收返回数据
	int lenth=0;
	if((lenth=stream.recv((char *)&RetMsg,sizeof(RadioSpecRetMessage_Obj),&RecvTimeOut) <= 0))
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]数据接收失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -3;
	}
	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]数据接收成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	OSFunction::Sleep(0,100);

	RadioSpecRetMessage_Handle rm = (RadioSpecRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x54 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]返回数据错误");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->freq=rm->freq;
	pRetObj->status=rm->status;
	pRetObj->level_int=rm->level_int;
	memcpy(pRetObj->spec,rm->spec,1024*sizeof(char));

	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]频谱扫描成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	return 1;
}


int RadioTcpDeviceAccess::SendCmdForQuality(MSG_TYPE msg_type,void* info, RadioQuaRetMessage_Handle pRetObj,int infolen)
{
	if ( ConnectToServer() == false)
		return -1;
	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[5000] = {0};
	//信息头
	PkgHeader_Obj   ph;

	ph.header   = 0x53;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//信息内容
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;

	ACE_Time_Value sendtimeout(8);
	ACE_Time_Value rectimeout(20);

	string sysmsg;
	//发送信息
	if(stream.send((char*)sendbuf, pkgLen,&sendtimeout) <= 0)		
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -2;
	}
	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]发送指令成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);
	//接收返回数据
	int lenth=0;
	if((lenth=stream.recv(( char *)&RetMsg,sizeof(RetMsg),&rectimeout)) <= 0)
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]数据接收失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -3;
	}
	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]数据接收成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	RadioQuaRetMessage_Handle rm = (RadioQuaRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x54 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]返回数据错误");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->dev_int=rm->dev_int;
	pRetObj->dF=rm->dF;
	pRetObj->freq=rm->freq;
	pRetObj->level_int=rm->level_int;
	pRetObj->status=rm->status;

	sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]指标测量成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);

	return 1;
}


bool RadioTcpDeviceAccess::SpecScan(int freq,RadioSpecRetMessage_Obj& RadioSpec)
{
	Radiopayload_Obj freqinfo;

	freqinfo.flag=0x01;
	freqinfo.freq=freq;
	freqinfo.gain=5;
	int ret=SendCmdForSpecScan(MSG_GET_RADIO,(void *)&freqinfo,&RadioSpec,sizeof(Radiopayload_Obj));
	if(ret>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void  RadioTcpDeviceAccess::CaculateCenterFreq(int startfreq,int endfreq,int *centerfreqs,int &len)
{
	int I=(endfreq-startfreq)*1000/(3660*1024);

	int middlefreq=startfreq;
	int k=0;
	centerfreqs[0]=startfreq+(511*3660)/1000;
	for(k=1;k<=I;k++)
	{
		centerfreqs[k]=(centerfreqs[k-1]+(3660*1023)/1000);
	}

	len=k;
	return ;
}

int RadioTcpDeviceAccess::Findpolars(char spec_int[1023],int fFreq,int *result)
{
	int ij,k,z,max=0,addk=0,addmax=0;
	double sum=0.0;
	int i_result = 0;
	float polar = 0.0;
	char tmp = 0;
	int throwhold;
	for(ij=0;ij<993;)
	{
		for(k=0;k<29;k++)
		{
			for(z=0;z<(29-k);z++)
			{
				if(((spec_int[ij+z+1+k]-spec_int[ij+k])<10)||
					((spec_int[ij+z+k+1]-spec_int[ij+k])>-10))
				{
					addk++;
				}
				else
				{
					addk = 0;
					sum = 0.0;
					break;
				}
			}
			if(addk==0)
				break;
			sum+=pow(10.0,(double)spec_int[ij+k]/20.0);
		}
		if(addk)
			break;
		ij+=(z+k);
	}
	throwhold = (int)(log10(sum/29.0)*20.0);
	for(ij=0;ij<1023;ij++)
	{
		tmp = spec_int[ij];
		addmax = 0;
		max = 0;
		if((tmp>(throwhold+10))&&(tmp<127))
		{
			if(ij>972)
			{
				for(k=ij+1;k<1023;k++)
				{
					if(tmp>spec_int[k])
						addmax++;
					else
					{
						break;
					}
				}
				for(k=ij+1;(k<(ij+10))&&(k<1023);k++)
				{
					if(tmp==spec_int[k])
					{
						addmax++;
					}
					else
					{
						break;
					}
				}
				if(addmax==(1022-ij))
				{

				}
				else
				{
					continue;
				}
			}
			else
			{
				for(k=ij+1;k<ij+51;k++)
				{
					if(tmp>spec_int[k])
					{
						addmax++;
					}
					else
					{
						break;
					}
				}

				for(k=ij+1;(k<(ij+10))&&(k<1023);k++)
				{
					if(tmp==spec_int[k])
					{
						addmax++;
					}
					else
					{
						break;
					}
				}

				if(addmax==50)
				{

				}
				else
				{
					continue;
				}
			}
			if(ij<50)
			{
				for(k=0;k<ij;k++)
				{
					if(tmp>spec_int[k])
					{
						max++;
					}
					else
					{
						break;
					}
				}
				if(k>0)
				{
					for(k=ij-1;(k>(ij-10))&&(k>=0);k--)
					{
						if(tmp==spec_int[k])
						{
							max++;
						}
						else
						{
							break;
						}
					}
				}

				if(max==ij)
				{
					polar=(float)fFreq*100.0-(512-ij)*3.66;
					if(polar<87000||polar>108000)
						continue;
					result[i_result] = (int)polar;
					if(i_result>0)
					{
						if((result[i_result]-result[i_result-1])<100)
						{
							result[i_result-1]=(result[i_result]+result[i_result-1])/2;
							i_result--;
						}
					}
					i_result++;
				}
				else
				{
					continue;
				}
			}
			else
			{
				for(k=(ij-50);k<ij;k++)
				{
					if(tmp>spec_int[k])
					{
						max++;
					}
					else
					{
						break;
					}
				}
				for(k=(ij-1);(k>(ij-10))&&(k>=0);k--)
				{
					if(tmp==spec_int[k])
					{
						max++;
					}
					else
					{
						break;
					}
				}
				if(max==50)
				{
					polar=(float)fFreq*100.0-(511-ij)*3.66;
					if(polar<87000||polar>108000)
						continue;
					result[i_result] = (int)polar;
					if(i_result>0)
					{
						if((result[i_result]-result[i_result-1])<100)
						{
							result[i_result-1]=(result[i_result]+result[i_result-1])/2;
							i_result--;
						}
					}
					i_result++;
				}
				else
				{
					continue;
				}
			}
		}
		else
		{
			continue;
		}
	}

	return i_result;
}


bool RadioTcpDeviceAccess::GetRadioSpecResult(float fFreq, RadioSpecRetMessage_Obj &rqr)
{
	if(fFreq<110&&fFreq>=87)
	{
		int nfreq = (int)((fFreq+0.0002)*1000);	//修正float to int 修正添加0.2
		if(SpecScan(nfreq,rqr))
		{
			return true;
		}
		else
		{
			string sysmsg = string("广播频点[") + StrUtil::Int2Str(nfreq) + string("]测量失败");
			//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
		}
	}
	return false;
}


bool RadioTcpDeviceAccess::GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)
{	
	if(fFreq<108&&fFreq>=87)
	{
		int nfreq = (int)((fFreq+0.0002)*1000);	//修正float to int 修正添加0.2

		Radiopayload_Obj freqinfo;
		freqinfo.flag = 0x02;
		freqinfo.freq = nfreq;
		freqinfo.gain = 5;
		int ret=SendCmdForQuality(MSG_GET_RADIO,(void *)&freqinfo,&rqr,sizeof(Radiopayload_Obj));
		if(ret>0)
		{
			return true;
		}
		else
		{
			string sysmsg = string("广播频点[")  + strIPAddress  +string(":") + StrUtil::Int2Str(nfreq) + string("]测量失败");
			//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
			
			return false;
		}
	}
	return false;
}


// bool RadioTcpDeviceAccess::QueryQuality(int freq,RadioQuaRetMessage_Obj& RadioQua)
// {
// 	Radiopayload_Obj freqinfo;
// 	freqinfo.flag=0x02;
// 	freqinfo.freq=freq;
// 	freqinfo.gain=5;
// 	int ret=SendCmdForQuality(MSG_GET_RADIO,(void *)&freqinfo,&RadioQua,sizeof(Radiopayload_Obj));
// 	if(ret>0)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }


bool RadioTcpDeviceAccess::GetRadioFreqLevel(float fFreq, float &fLevel)
{
	if(fFreq<108&&fFreq>=87)
	{
		int nfreq = (int)((fFreq+0.0002)*1000);	//修正float to int 修正添加0.2
		int index = int((fFreq-87)*10);		//修正float to int 修正添加0.2

		RadioQuaRetMessage_Obj rqr;
		memset(&rqr, 0, sizeof(RadioQuaRetMessage_Obj) );

		Radiopayload_Obj freqinfo;
		freqinfo.flag = 0x02;
		freqinfo.freq = nfreq;
		freqinfo.gain = 5;
		int ret=SendCmdForQuality(MSG_GET_RADIO,(void *)&freqinfo,&rqr,sizeof(Radiopayload_Obj));
		if(ret>0)
		{
			fLevel = rqr.level_int;	
			return true;
		}
		else
		{
			string sysmsg = string("广播频点[") + StrUtil::Int2Str(nfreq) + string("]测量失败");
			//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
		
			return false;
		}
	}
	return false;
}

bool RadioTcpDeviceAccess::TurnFreq( TunerConfig_Obj& tunerinfo)
{
	//tunerinfo.freq += 100;		//适应新板卡的频偏
	bool rtn = SendCmdToServer(MSG_SET_FMTUNER,(void*)&tunerinfo,sizeof(TunerConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("通道[")+ strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]音频调频失败[") + \
			StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("通道[") + strIPAddress  +string(":") + StrUtil::Int2Str(mChannelID) + string("]音频调频成功[") + \
			StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);
	}

	return rtn;
}

bool RadioTcpDeviceAccess::SetAudioRate(const MediaConfig_Obj& rateinfo)
{
	bool rtn = SendCmdToServer(MSG_SET_AUDIOBITRATE,(void*)&rateinfo,sizeof(MediaConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]设置音频码率失败[") + \
			StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
			string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("通道[") + StrUtil::Int2Str(mChannelID) + string("]设置音频码率成功[") + \
			StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
			string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,RADIO);
	}

	return rtn;
}