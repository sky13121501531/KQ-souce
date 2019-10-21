
#include "./ATVTcpDeviceAccess.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"


static	const long pStandardConstFreq[55] = {
	49750000,
	57750000,
	65750000,
	77250000,
	168250000,
	176250000,
	184250000,
	192250000,
	200250000,
	208250000,
	216250000,
	471250000,
	479250000,
	487250000,
	495250000,
	503250000,
	511250000,
	519250000,
	527250000,
	535250000,
	543250000,
	551250000,
	559250000,
	607250000,
	615250000,
	623250000,
	631250000,
	639250000,
	647250000,
	655250000,
	663250000,
	671250000,
	679250000,
	687250000,
	695250000,
	703250000,
	711250000,
	719250000,
	727250000,
	735250000,
	743250000,
	751250000,
	759250000,
	767250000,
	775250000,
	783250000,
	791250000,
	799250000,
	807250000,
	815250000,
	823250000,
	831250000,
	839250000,
	847250000,
	855250000,
};


static	const long AudioConstFreq[111] = {
	49750000,
	57750000,
	65750000,
	77250000,
	85250000,
	112250000,
	120250000,
	128250000,
	136250000,
	144250000,
	152250000,
	160250000,
	168250000,
	176250000,
	184250000,
	192250000,
	200250000,
	208250000,
	216250000,
	224250000,
	232250000,
	240250000,
	248250000,
	256250000,
	264250000,
	272250000,
	280250000,
	288250000,
	296250000,
	304250000,
	312250000,
	320250000,
	328250000,
	336250000,
	344250000,
	352250000,
	360250000,
	368250000,
	376250000,
	384250000,
	392250000,
	400250000,
	408250000,
	416250000,
	424250000,
	432250000,
	440250000,
	448250000,
	456250000,
	464250000,
	471250000,
	479250000,
	487250000,
	495250000,
	503250000,
	511250000,
	519250000,
	527250000,
	535250000,
	543250000,
	551250000,
	559250000,
	567250000,
	575250000,
	583250000,
	591250000,
	599250000,
	607250000,
	615250000,
	623250000,
	631250000,
	639250000,
	647250000,
	655250000,
	663250000,
	671250000,
	679250000,
	687250000,
	695250000,
	703250000,
	711250000,
	719250000,
	727250000,
	735250000,
	743250000,
	751250000,
	759250000,
	767250000,
	775250000,
	783250000,
	791250000,
	799250000,
	807250000,
	815250000,
	823250000,
	831250000,
	839250000,
	847250000,
	855250000,
	863250000,
	871250000,
	879250000,
	887250000,
	895250000,
	903250000,
	911250000,
	919250000,
	927250000,
	935250000,
	943250000,
	951250000
};


AtvTcpDeviceAccess::AtvTcpDeviceAccess(int deviceid,std::string strIP,int nPort) : TCPDeviceAccess(deviceid, strIP, nPort)
{
	mImageLevel = 0.00f;
	mAudioLevel = 0.00f;
	mI2AOffLevel = 0.00f;
	mCN = 0.00f;
	mFreqOffSet = 0.00f;
	mSlope = 0.00f;

	mAnalyserImageOffset = 0;
	mAnalyserAudioOffset = 0;
	mAnalyserI2AOffset = 0;
	mAnalyserCnOffset = 0;
	mAnalyserOffOffset = 0;
	mAnalyserSlopeOffset = 0;

	for(int i=0;i<5;i++)
	{
		fOldAnalyser[i] = 13.0f;
	}
}

AtvTcpDeviceAccess::~AtvTcpDeviceAccess()	
{
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
}
bool AtvTcpDeviceAccess::GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
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
		if(dvbtype=="ATV"||dvbtype=="CTV")
		{
			switch(i_type)
			{
			case 1:
				f_value=1000*GetImageLevel(freq);
				break;
			case 2:
				f_value=1000*GetAudioLevel(freq);
				break;
			case 3:
				{
					if((ff[0] == ff[1])&&(ff[0] == 0))
					{
						f_value=1000*GetI2AOffLevel(freq);
					}
					else
					{
						f_value = (float)(ff[0] - ff[1]);
					}	
					break;
				}

			case 4:
				f_value=1000*GetCN(freq);
				break;
			case 5:
				f_value=1000*GetFreqOffSet(freq);
				break;
			case 6:
				f_value=1000*GetSlope(freq);
				break;
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
	Retparser.SetAttrNode("Freq",StrUtil::Float2Str(freq),reportNode);
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

bool AtvTcpDeviceAccess::GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
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

		if(dvbtype=="ATV"||dvbtype=="CTV")
		{
			float retLevel = 0.00f;
			float oldLevel = 10;
			retLevel = retLevel<0?0:retLevel;
			int m=0;
			do
			{
				retLevel = GetImageLevel(StrUtil::Str2Float(freq));
				m++;
			}while((retLevel<=10)&&(m<3));
			if(retLevel<10)
			{
				retLevel = oldLevel;	//控制底噪
			}
			Temp.freq=1000*(StrUtil::Str2Float(freq));
			Temp.level = retLevel;
			SpectrumVec.push_back(Temp);
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

bool AtvTcpDeviceAccess::GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	string retATVxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					 <Msg DVBType=\"ATV\" >\
					 <Return Type=\"\" Value=\"0\" Desc=\"成功\" Comment=\"\"/> \
					 <ChannelScan></ChannelScan></Msg>";
	string retCTVxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					 <Msg DVBType=\"CTV\" >\
					 <Return Type=\"\" Value=\"0\" Desc=\"成功\" Comment=\"\"/> \
					 <ChannelScan></ChannelScan></Msg>";

	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	if (dvbtype=="ATV")
	{
		XmlParser retpaser(retATVxml.c_str());
		pXMLNODE channscannode=parser.GetNodeFirstChild(childnode);
		string startfreq,endfreq;
		parser.GetAttrNode(channscannode,"StartFreq",startfreq);
		parser.GetAttrNode(channscannode,"EndFreq",endfreq);
		ChannelScan_Obj obj;

		PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,obj.chan);
		obj.Startfreq = (int)(StrUtil::Str2Float(startfreq)*1000.0);
		obj.Endfreq = (int)(StrUtil::Str2Float(endfreq)*1000.0);

		if (obj.Startfreq < 49750)
			obj.Startfreq = 49750;
		if (obj.Endfreq > 855250)
			obj.Endfreq = 855250;


		ChannelScanRetMessage_Obj Channelinfo;
		float ResultFreq=0.0;
		//目前默认为标准频道扫描
		if(SendCmdForChannelScan(MSG_GET_STDCHANSCAN,(void*)&obj,&Channelinfo,sizeof(ChannelScan_Obj))>0)
		{
			pXMLNODE retchannode=retpaser.GetNodeFromPath("Msg/ChannelScan");
			for(int i=0;i<111;i++)
			{
				if(Channelinfo.freq[i]==1)
				{
					ResultFreq=((float)(AudioConstFreq[i]))/1000000;
					string tmpResult=StrUtil::Float2Str(ResultFreq);
					//tmpResult=tmpResult.substr(0,tmpResult.find(".")+2);
					pXMLNODE freqnode=retpaser.CreateNodePtr(retchannode,"Channel");
					retpaser.SetAttrNode("Freq",tmpResult,freqnode);
				}
			}
			retpaser.SaveToString(strRetMsg);
			return true;
		}
		else
		{
			strRetMsg="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"ATV\" >\
					  <Return Type=\"\" Value=\"1\" Desc=\"失败\" Comment=\"\"/> \
					  <ChannelScan></ChannelScan></Msg>";
			return false;
		}
	}
	else if (dvbtype=="CTV")
	{
		XmlParser retpaser(retCTVxml.c_str());
		pXMLNODE channscannode=parser.GetNodeFirstChild(childnode);
		string startfreq,endfreq;
		parser.GetAttrNode(channscannode,"StartFreq",startfreq);
		parser.GetAttrNode(channscannode,"EndFreq",endfreq);
		ChannelScan_Obj obj;

		PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,obj.chan);
		obj.Startfreq = (int)(StrUtil::Str2Float(startfreq)*1000.0);
		obj.Endfreq = (int)(StrUtil::Str2Float(endfreq)*1000.0);

		if (obj.Startfreq < 49750)
			obj.Startfreq = 49750;
		if (obj.Endfreq > 855250)
			obj.Endfreq = 855250;


		ChannelScanRetMessage_Obj Channelinfo;
		float ResultFreq=0.0;
		//目前默认为标准频道扫描
		if(SendCmdForChannelScan(MSG_GET_STDCHANSCAN,(void*)&obj,&Channelinfo,sizeof(ChannelScan_Obj))>0)
		{
			pXMLNODE retchannode=retpaser.GetNodeFromPath("Msg/ChannelScan");
			for(int i=0;i<111;i++)
			{
				if(Channelinfo.freq[i]==1)
				{
					ResultFreq=((float)(AudioConstFreq[i]))/1000000;
					string tmpResult=StrUtil::Float2Str(ResultFreq);
					//tmpResult=tmpResult.substr(0,tmpResult.find(".")+2);
					pXMLNODE freqnode=retpaser.CreateNodePtr(retchannode,"Channel");
					retpaser.SetAttrNode("Freq",tmpResult,freqnode);
				}
			}
			retpaser.SaveToString(strRetMsg);
			return true;
		}
		else
		{
			strRetMsg="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"CTV\" >\
					  <Return Type=\"\" Value=\"1\" Desc=\"失败\" Comment=\"\"/> \
					  <ChannelScan></ChannelScan></Msg>";
			return false;
		}
	}
	else
		return false;
	
	return true;
}


bool AtvTcpDeviceAccess::GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
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

	if (dvbtype == string("ATV")||dvbtype == string("CTV"))
	{
		tuner_obj.freq=int(StrUtil::Str2Float(freq)*1000);
		media_obj.video_bitrate=StrUtil::Str2Int(bps);
		if(media_obj.video_bitrate>700)
			media_obj.video_bitrate=700;
	}

	if ((dvbtype == string("ATV")||dvbtype == string("CTV")) && TurnFreq(tuner_obj) && SetVideoRate(media_obj))
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

int AtvTcpDeviceAccess::SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)
{
	if ( ConnectToServer() == false)
		return -1;

	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[200] = {0};
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
		sysmsg = string("通道[")  + strIPAddress + string(":") + StrUtil::Int2Str(mChannelID) + string("]发送指令失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -1;
	}
	sysmsg = string("通道[") + strIPAddress + string(":") + StrUtil::Int2Str(mChannelID) + string("]发送指令成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);

	int len = -1;

	ACE_Time_Value TimeOut(60);

	len =stream.recv((char*)&RetMsg,sizeof(RetMsg),&TimeOut);

	if(len<=0)
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]数据接收失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -1;
	}

	sysmsg = string("通道[")  + strIPAddress + string(":")  + StrUtil::Int2Str(mChannelID) + string("]数据接收成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);

	ChannelScanRetMessage_Handle rm = (ChannelScanRetMessage_Handle)(RetMsg);

	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x49 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("通道[") + strIPAddress + string(":")  + StrUtil::Int2Str(mChannelID) + string("]返回数据错误");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);
		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->status=rm->status;
	memcpy(pRetObj->freq,rm->freq,sizeof(pRetObj->freq));

	sysmsg = string("通道[")  + strIPAddress + string(":")  + StrUtil::Int2Str(mChannelID) + string("]频道扫描成功");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);

	return 1;
}

bool AtvTcpDeviceAccess::GetFreqLevel(float fFreq, float &fLevel)//
{
	if((fFreq>0.01&&fFreq<1000)) //100k~1000M
	{
		int nFreq = (fFreq+0.0002)*1000;
		if(QualityRealtimeQueryTV(nFreq,fLevel))
			return true;
	}
	return false;
}

bool AtvTcpDeviceAccess::QualityRealtimeQueryTV(int freq,float& f_level)
{
	{//先查查n秒内有没有测过，测过就用历史数据，没测过就直接跳过下一波
		int n =10;//10秒内都用历史数据 需要根据硬件特性进行调整
		std::vector<TVQuality>::iterator pTV;
		for(pTV=m_vecTVhistory.begin();pTV!=m_vecTVhistory.end();pTV++)
		{//判断频点相同则更新指标列表中历史记录
			if((pTV->Freq - freq)<1 && (pTV->Freq - freq)>-1)
			{
				if((pTV->getTime - time(0))>n)
					break;
				else
				{
					f_level  = pTV->ImageLevel;
					return true;
				}
			}
		}
	}

	int level = SendCmdToTVCom(MSG_GET_QUA,(void *)&freq,sizeof(int));
	f_level=(float)(level)/100.0;
	string sysmsg;
	if(level>=0)
	{
		std::vector<TVQuality>::iterator pTV;
		bool isUpdate = false;
		TVQuality tvQ;
		tvQ.Freq = freq;
		tvQ.ImageLevel = f_level;
		tvQ.getTime = time(0);
		for(pTV=m_vecTVhistory.begin();pTV!=m_vecTVhistory.end();pTV++)
		{
			if((pTV->Freq - tvQ.Freq)<1 && (pTV->Freq - tvQ.Freq)>-1)
			{//判断频点相同则更新指标列表中历史记录
				//*pRQR = *pRetObj;
				pTV->ImageLevel = tvQ.ImageLevel;
				pTV->getTime = tvQ.getTime;
				isUpdate = true;
				break;
			}
		}
		if(!isUpdate)
		{//如果vec中无数据更新则认定为无数据记录,把数据插入vec中
			m_vecTVhistory.push_back(tvQ);
		}
		//sysmsg = string("通道[") + mstrDeviceIP +string(":") + StrUtil::Int2Str(mChannelID) + string("]指标测量成功");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);
		return true;
	}
	else
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]指标测量失败");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);
		return false;
	}
	return false;
}

float AtvTcpDeviceAccess::GetImageLevel(float Freq)
{
	if(GetFreqLevel(Freq,mImageLevel))
	{
		float ff = 0;
		mImageLevel += ff;			
		if(mImageLevel > (ff+1))
		{
			fOldAnalyser[0] = mImageLevel;
		}
		else 
		{
			mImageLevel = fOldAnalyser[0];
		}
	}
	else
	{
		mImageLevel = fOldAnalyser[0];
		mImageLevel = 0.00f;
	}
	return mImageLevel;
}

float AtvTcpDeviceAccess::GetAudioLevel(float Freq)
{
	if(GetFreqLevel(Freq + 6.5f,mAudioLevel))
	{
		if(mAudioLevel<0)
		{
			mAudioLevel = mImageLevel-15.5;
		}
		float ff = 0;
		if(ff <= 20&&ff >= -20)
		{
			mAudioLevel += ff;
		}
		if(mAudioLevel > (ff+1))
			fOldAnalyser[1] = mAudioLevel;
		else 
			mAudioLevel = fOldAnalyser[1];
	}
	else
	{
		mAudioLevel = 0.00f;
	}
	return mAudioLevel;
}

float AtvTcpDeviceAccess::GetI2AOffLevel(float Freq)
{
	if (mImageLevel == 0.00f)
	{
		GetFreqLevel(Freq,mImageLevel);
	}
	if (mAudioLevel == 0.00f) 
	{
		GetFreqLevel(Freq + 6.5f,mAudioLevel);
	}

	//mAnalyserAudioOffset = mAnalyserImageOffset = COffsetLoad::GetInstance()->GetOffsetLevel(Freq);

	mI2AOffLevel = (mImageLevel + mAnalyserImageOffset - mAudioLevel - mAnalyserAudioOffset) > 0?(mImageLevel + mAnalyserImageOffset - mAudioLevel - mAnalyserAudioOffset):(mImageLevel + mAnalyserImageOffset - mAudioLevel - mAnalyserAudioOffset);

	mI2AOffLevel += mAnalyserI2AOffset;

	return mI2AOffLevel;
}

float AtvTcpDeviceAccess::GetAlignedFreq(float freq)
{
	float fAlignedFreq = freq * 1000000;
	for(int i=1; i<55; ++i)
	{
		if(pStandardConstFreq[i] > fAlignedFreq)
		{
			if(pStandardConstFreq[i] - fAlignedFreq < fAlignedFreq - pStandardConstFreq[i-1])
			{
				fAlignedFreq = (float)pStandardConstFreq[i];
			}
			else
			{
				fAlignedFreq = (float)pStandardConstFreq[i-1];
			}
			break;
		}
	}
	return (float)(fAlignedFreq/1000000.0);
}

float AtvTcpDeviceAccess::GetCN(float Freq)
{
	GetBaseCN(GetAlignedFreq(Freq), mCN);
	mCN += mAnalyserCnOffset;
	return mCN;
}

float AtvTcpDeviceAccess::GetFreqOffSet(float Freq)
{
	return mFreqOffSet;
}

float AtvTcpDeviceAccess::GetSlope(float Freq)
{
	mSlope = 0.00f;
	mSlope += mAnalyserSlopeOffset;
	return mSlope;
}

bool AtvTcpDeviceAccess::GetBaseCN(float fFreq, float &fCN)
{
	float fLevel, fNoise;
	GetFreqLevel(fFreq,fLevel);
	GetFreqLevel((float)(fFreq - 1.1),fNoise);
	fCN = fLevel - fNoise;
	if(fCN < 1.0)
	{
		fCN = 0.0f;
	}
	return true;
}

float AtvTcpDeviceAccess::GetOpenTVStandardFreq(float freq)
{
	double fAlignedFreq = freq * 1000000;
	for(int i=0; i<111; ++i)
	{
		if (AudioConstFreq[i] == 464250000)
		{
			int n= 0;
		}
		if((AudioConstFreq[i] - fAlignedFreq >= 0) && (fAlignedFreq - AudioConstFreq[i-1] > 0))
		{
			fAlignedFreq = (double)AudioConstFreq[i];
			break;
		}
	}
	return (fAlignedFreq/1000000);
}

bool AtvTcpDeviceAccess::TurnFreq( TunerConfig_Obj& tunerinfo)
{
	bool rtn = SendCmdToServer(MSG_SET_VIDEOTUNER,(void*)&tunerinfo,sizeof(TunerConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("通道[")  + strIPAddress +string(":")+ StrUtil::Int2Str(mChannelID) + string("]视频调频失败[") + \
				 StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("通道[")  + strIPAddress +string(":")+StrUtil::Int2Str(mChannelID) + string("]视频调频成功[") + \
				 StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);
	}

	return rtn;
}

bool AtvTcpDeviceAccess::SetVideoRate(const MediaConfig_Obj& rateinfo)
{
	bool rtn = SendCmdToServer(MSG_SET_VIDEOBITRATE,(void*)&rateinfo,sizeof(MediaConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]设置视频码率失败[") + \
				 StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
				 string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("通道[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]设置视频码率成功[") + \
			StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
			string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,ATV);
	}

	return rtn;
}