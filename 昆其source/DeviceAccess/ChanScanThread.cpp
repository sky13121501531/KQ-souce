
#include "ChanScanThread.h"
#include "HTTPDeviceAccess.h"
#include "UdpRecvChanScanThread.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../BusinessProcess/BusinessIterfaceMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/AppLog.h"
#include "../Communications/DeviceServer.h"
#include <fstream>
static const int FreqNum = 137;
static	const long digitfreq[FreqNum] = {
	115000,
	123000,
	131000,
	139000,
	147000,
	155000,
	163000,
	171000,
	179000,
	187000,
	195000,
	203000,
	210000,//西双版纳频点
	211000,
	219000,
	226000,//西双版纳频点
	227000,
	234000,//西双版纳频点
	235000,
	242000,//西双版纳频点
	243000,
	250000,//西双版纳频点
	251000,
	258000,//西双版纳频点
	259000,
	266000,//西双版纳频点
	267000,
	274000,//西双版纳频点
	275000,
	282000,//西双版纳频点
	283000,
	290000,//西双版纳频点
	291000,
	298000,//西双版纳频点
	299000,
	306000,//西双版纳频点
	307000,
	314000,//西双版纳频点
	315000,
	322000,//西双版纳频点
	323000,
	330000,//西双版纳频点
	331000,
	338000,//西双版纳频点
	339000,
	346000,//西双版纳频点
	347000,
	354000,//西双版纳频点
	355000,
	362000,//西双版纳频点
	363000,
	370000,//西双版纳频点
	371000,
	378000,//西双版纳频点
	379000,
	386000,//西双版纳频点
	387000,
	394000,//西双版纳频点
	395000,
	402000,//西双版纳频点
	403000,
	410000,//西双版纳频点
	411000,
	419000,
	426000,//西双版纳频点
	427000,
	434000,//西双版纳频点
	435000,
	442000,//西双版纳频点
	443000,
	450000,//西双版纳频点
	451000,
	458000,//西双版纳频点
	459000,//
	466000,
	467000,//伊犁本地节目频点
	474000,
	482000,
	490000,
	498000,
	506000,
	514000,
	522000,
	530000,
	538000,
	546000,
	554000,
	562000,
	570000,
	578000,
	586000,
	594000,
	602000,
	610000,
	618000,
	626000,
	634000,
	642000,
	650000,
	658000,
	666000,
	674000,
	682000,
	690000,
	698000,
	706000,
	714000,
	722000,
	730000,
	738000,
	746000,
	754000,
	762000,
	770000,
	778000,
	786000,
	794000,
	802000,
	810000,
	818000,
	826000,
	834000,
	842000,
	850000,
	858000,
	866000,
	874000,
	882000,
	890000,
	898000,
	906000,
	914000,
	922000,
	930000,
	938000,
	946000,
	954000
};

//-----字符转化-----
void bw_encodestr(char* dest,char* src)
{
	char  *dest_str=NULL;
	char  *src_str=NULL;
	int  len,i;

	dest_str = dest;
	src_str = src;

	i = 0;
	len = 0;
	while(1)
	{
		if((src_str[i]=='%')&&
			((src_str[i+1]>='0'&&src_str[i+1]<='9')||(src_str[i+1]>='a'&&src_str[i+1]<='f')||(src_str[i+1]>='A'&&src_str[i+1]<='F'))&&
			((src_str[i+2]>='0'&&src_str[i+2]<='9')||(src_str[i+2]>='a'&&src_str[i+2]<='f')||(src_str[i+2]>='A'&&src_str[i+2]<='F')))
		{
			sscanf(src_str+i,"%%%02x",dest_str+len);
			len += 1;
			i += 3;
			continue;
		}
		if(src_str[i]=='\0') break;
		dest_str[len++] = src_str[i++];
	}
	dest_str[len] = '\0';
}

ChanScanThread::ChanScanThread()
{
	m_bChanScanWorkType = false;
	m_sNoticeXml = "";
	m_sOpVersion = "2.6";
}

ChanScanThread::~ChanScanThread()
{

}
int ChanScanThread::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int ChanScanThread::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
//频道扫描工作方法:目录C:\Table\DVBC\ChannelScanQuery.xml文件不存在立即启动扫描,已经存在文件C:\Table\DVBC\ChannelScanQuery.xml则与上次扫描超过12小时,在零晨2:15分启动扫描
//生成的格式与上报的格式有区别,见void ChannelScanQueryTask::Run(void) SendXML(TranslateUpXML::TranslateChannelScan(this,strRtnXml));//发送xml
//说明:如果数据库PsisiSchedulerTask_Time项为V30表示是3.0接口,否则为其他接口是具体的时间 目前3.0接口各通道的扫描结果是覆盖的,后续可考虑在文件名中添加通道
int ChanScanThread::svc()
{
	//
	string retime ;
	std::string scantype;
	std::string sfreq;
	std::string sweeklyswitch;
	std::string sepg = "1";
	std::string sCheckHD = "0";
	std::string sScanVer;
	/*scantype 程序启动时扫频道表类型 0: 全扫 1: 单频点扫描 2: 不扫
	**weeklyswitch 1: 每天都扫 0:只扫一次
	**epg 0表示不扫 1表示扫 2 表示所有EPG信息都用配置频点扫描
	*/
	PROPMANAGER::instance()->GetScanInfo(scantype,sfreq,sweeklyswitch,sepg, retime, sCheckHD,sScanVer);
	if(sScanVer.size()>0)
	{
		m_sOpVersion = sScanVer;
	}
	if (retime == "")
	{
		retime = "04:01:00";
	}
	string sDBRetime;
	DBMANAGER::instance()->GetAutoAnalysisTime(sDBRetime);//优先数据库获取
	if(sDBRetime.size() > 0)
	{
		if(sDBRetime == "V30")
		{
			m_sOpVersion = "3.0";
		}
		else
		{
			retime = sDBRetime;
		}
	}
	int bepg = 1;
	if(sepg == "0")
	{
		bepg = 0;
	}
	else if(sepg == "2")
	{
		bepg = 2;
	}
	//
	std::map<std::string, std::string> mapDvbCardIndex;
	string strlist = "";
	//
	PROPMANAGER::instance()->GetTaskDeviceList("ChannelScanQueryTask", strlist);
	if (strlist == "")//没有配置扫描通道 就不扫
	{
		bFlag = false;
	}
	else
	{
		GetDVBTypeAndCardIndex(strlist, mapDvbCardIndex);
		bFlag = true;
		m_bChanScanWorkType = true;
	}
	if (scantype == "2")
	{
		m_bChanScanWorkType = false;
	}
	//
	if(m_sOpVersion == "3.0")
	{
		string PSIsetTime,PSIType,PSIScanType,PSIDevID;
		DBMANAGER::instance()->GetAutoAnalysisInfo(PSIsetTime,PSIType,PSIScanType,PSIDevID);
		if(PSIsetTime.size()>0)
		{
			retime = PSIsetTime;
		}
		if(PSIDevID.size()>0)
		{
			int iChassisID,iPosition,iChanId;
			sscanf(PSIDevID.c_str(),"%d-%d-%d", &iChassisID, &iPosition, &iChanId);
			string TunerDeviceID = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,iChassisID,iPosition,iChanId));
			mapDvbCardIndex.clear();
			string strdev = "DVBC:";
			strdev+=TunerDeviceID;
			GetDVBTypeAndCardIndex(strdev, mapDvbCardIndex);
		}
	}
	//
	string strXML = "";
	//读取xml文件中的内容
	ifstream xmlFile("C:/vscttb/dvbsfreqinfo.xml");
	string strTemp = "";
	while (getline(xmlFile,strTemp))
	{
		strXML += strTemp;
	}
	xmlFile.close();
	//
	if(strXML.size()>0)
	{
		XmlParser parser(strXML.c_str());
		pXMLNODE freqlistNode=parser.GetNodeFromPath("freqlist");
		int nFreqCount = parser.GetChildCount(freqlistNode);
		pXMLNODELIST ifreqNodeList = parser.GetNodeList(freqlistNode);
		for (int nIndex=0; nIndex<nFreqCount; nIndex++)
		{
			sdvbsfreqinfo afi;
			pXMLNODE freqNode = parser.GetNextNode(ifreqNodeList);
			parser.GetAttrNode(freqNode,"locfreq",afi.slocfreq);
			parser.GetAttrNode(freqNode,"downfreq",afi.sdownfreq);
			parser.GetAttrNode(freqNode,"hv",afi.shv);
			parser.GetAttrNode(freqNode,"symbol",afi.ssymbol);
			parser.GetAttrNode(freqNode,"fec",afi.sfec);
			parser.GetAttrNode(freqNode,"is22k",afi.s22k);
			sdvbsfreqinfoList.push_back(afi);
		}
	}
	//
	while (bFlag)
	{
		if(m_sOpVersion == "3.0")
		{
			string PSIsetTime,PSIType,PSIScanType,PSIDevID;
			DBMANAGER::instance()->GetAutoAnalysisInfo(PSIsetTime,PSIType,PSIScanType,PSIDevID);
			if(PSIsetTime.size()>0)
			{
				retime = PSIsetTime;
			}
			if(PSIDevID.size()>0)
			{
				mapDvbCardIndex.clear();
				string strdev = "DVBC:";
				strdev+=PSIDevID;
				GetDVBTypeAndCardIndex(strdev, mapDvbCardIndex);
			}
		}
		//
		bool bweeklyswitch = false;
		if (sweeklyswitch == "1")
		{
			string date = TimeUtil::GetCurDate();
			string datetime = date;
			datetime += " ";
			datetime += retime;
			time_t re_tm = TimeUtil::StrToDateTime(datetime);
			if (time(0) == re_tm)//每天的 retime 扫描一次
			{
				m_bChanScanWorkType = true;
				bweeklyswitch = true;
			}
		}
		
		if(m_bChanScanWorkType)
		{
			if((m_sNoticeXml.size()>0)&&m_sOpVersion == "3.0")
			{
				PROPMANAGER::instance()->SetTunerinfo(StrUtil::Str2Int(m_sNowScanDevid),DVBC,"All","All","频道扫描进行中");
				if(m_sNoticeXml.find("TableScanQuery")!=-1)
				{
					PSISIForDVBC(m_sNowScanDevid,m_sNowScanFreq);
				}
				else
				{
					if(m_sNoticeXml.find("Type=\"3\"")!=-1)//即时频道扫描采集不采epg
					{
						if(bepg == 2)
						{
							if(m_sNowScanFreq=="All")
							{
								ChanScanForDVBC(m_sNowScanDevid, 0, "0", bweeklyswitch, sfreq, sCheckHD);
							}
							else
							{
								ChanScanForDVBC(m_sNowScanDevid, 0, "1", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
						}
						else
						{
							if(m_sNowScanFreq=="All")
							{
								ChanScanForDVBC(m_sNowScanDevid, 0, "0", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
							else
							{
								ChanScanForDVBC(m_sNowScanDevid, 0, "1", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
						}
					}
					else
					{
						if(bepg == 2)
						{
							if(m_sNowScanFreq=="All")
							{
								ChanScanForDVBC(m_sNowScanDevid, bepg, "0", bweeklyswitch, sfreq, sCheckHD);
							}
							else
							{
								ChanScanForDVBC(m_sNowScanDevid, bepg, "1", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
						}
						else
						{
							if(m_sNowScanFreq=="All")
							{
								ChanScanForDVBC(m_sNowScanDevid, bepg, "0", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
							else
							{
								ChanScanForDVBC(m_sNowScanDevid, bepg, "1", bweeklyswitch, m_sNowScanFreq, sCheckHD);
							}
						}
					}
				}
				PROPMANAGER::instance()->FreeSetTunerDev(StrUtil::Str2Int(m_sNowScanDevid),DVBC,"All","All");
			}
			else
			{
				std::map<std::string, std::string>::iterator iter = mapDvbCardIndex.begin();
				for(iter; iter != mapDvbCardIndex.end(); iter++)
				{
					if ((*iter).first == "DVBC")
					{
						if(m_sOpVersion == "3.0")
						{
							PROPMANAGER::instance()->SetTunerinfo(StrUtil::Str2Int((*iter).second),DVBC,"All","All","频道扫描进行中");
							ChanScanForDVBC((*iter).second, bepg, scantype, bweeklyswitch, sfreq, sCheckHD);
							PROPMANAGER::instance()->FreeSetTunerDev(StrUtil::Str2Int((*iter).second),DVBC,"All","All");
						}
						else
						{
							ChanScanForDVBC((*iter).second, bepg, scantype, bweeklyswitch, sfreq, sCheckHD);
						}
					}
					else if ((*iter).first == "CTTB")
					{
						ChanScanForCTTB((*iter).second, bepg, scantype, bweeklyswitch, sfreq);
					}
					else if ((*iter).first == "FM")
					{
						ChanScanForFM((*iter).second);
					}
					else if ((*iter).first == "AM")
					{
						ChanScanForAM((*iter).second);
					}
					else if ((*iter).first == "DVBS")
					{
						ChanScanForDVBS((*iter).second);
					}
				}
			}
			m_bChanScanWorkType = false;
			if (m_sNoticeXmlTwo.find("Type=\"2\"")!=-1)
			{
				m_sNoticeXml = m_sNoticeXmlTwo;
				m_sNoticeXmlTwo = "";
				m_bChanScanWorkType = true;
			}
			else
			{
				m_sNoticeXml = "";
				m_sNoticeXmlTwo = "";
			}
		}
		else
		{
			Sleep(1000);
		}
	}
	bFlag = false;
	return 0;
}
int ChanScanThread::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
//
bool ChanScanThread::PSISIForDVBC(std::string sDevID,std::string sFreq)
{
	bool ret = true;
	//
	sFreqScanInfo freqscaninfo;
	PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
	//
	string Path;
	PROPMANAGER::instance()->GetXmlTablePath(DVBC,Path);
	string tablequeryfilename = Path + string("TableQuery.xml");
	//
	int idevID = StrUtil::Str2Int(sDevID);
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID,DVBC,tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0,DVBC,idevID,tunermonitorip,tunermonitorport);//tuner
	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		cout<< TimeUtil::GetCurDateTime()<<"开始-PSISI表采集............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		string cmd = "POST /device/psisisearch  HTTP/1.1  \r\n\r\n";
		string strRet;
		cmd+="chl=";
		cmd+=tdev.s_cardindex;
		cmd+="_";
		cmd+=tdev.s_chanindex;
		cmd+="&fre=";
		cmd+=sFreq;
		cmd+="&sym=";
		cmd+=freqscaninfo.SymbolRate;
		cmd+="&qam=2 ";
		cmd+=m_sNoticeXml;
		pDeviceAccess->SendTaskMsg(cmd,strRet);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		//
		if(strRet.size() <= 0)
		{
			cout<<"ChanScanThread命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
		}
		else
		{
			XmlParser psr;
			psr.Set_xml(strRet);
			pXMLNODE tableScanNode = psr.GetNodeFromPath("Msg/ReturnInfo/TableScanQuery");
			psr.SetAttrNode("ScanTime",TimeUtil::GetCurDateTime(),tableScanNode);//目标URL
			psr.SaveAsFile(tablequeryfilename.c_str());
		}
		cout<< TimeUtil::GetCurDateTime()<<"结束-PSISI表采集............."<<endl;
		//
	}
	else
	{
		cout<<"PSISIForDVBC命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
	}
	return ret;
}
//DVBC 频道扫描
bool ChanScanThread::ChanScanForDVBC(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq, std::string sCheckHD)
{
	std::vector<HDInfoForScan>  HDInfoFreqList;
	//
	sFreqScanInfo freqscaninfo;
	PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
	//
	string Path = "";
	PROPMANAGER::instance()->GetXmlTablePath(DVBC,Path);
	string filename = Path + string("ChannelScanQuery.xml");
	string epgfilename = Path + string("EPGQuery.xml");
	//
	char * strXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	char * strEpgXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	int idevID = StrUtil::Str2Int(sDevID);
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID,DVBC,tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0,DVBC,idevID,tunermonitorip,tunermonitorport);//tuner
	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		XmlParser pXPRet(strXML);
		pXMLNODE rootNode = pXPRet.GetRootNode();
		pXPRet.SetAttrNode(string("DVBType"),string("DVBC"),rootNode);
		pXMLNODE  returnNode = pXPRet.CreateNodePtr(rootNode,"Return");   //创建Return节点
		pXPRet.SetAttrNode(string("Type"),string("ChannelScanQuery"),returnNode);
		pXPRet.SetAttrNode(string("Value"),string("0"),returnNode);
		pXPRet.SetAttrNode(string("Desc"),string("成功"),returnNode);
		pXPRet.SetAttrNode(string("Comment"),string(""),returnNode);
		pXPRet.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),returnNode);
		pXMLNODE reportNode = pXPRet.CreateNodePtr(rootNode,"ChannelScanReport");
		//*****
		XmlParser pXPEpg(strEpgXML);
		pXMLNODE rootEpgNode = pXPEpg.GetRootNode();
		//pXPEpg.SetAttrNode(string("DVBType"),string("DVBC"),rootEpgNode);
		pXMLNODE  EpgreturnNode = pXPEpg.CreateNodePtr(rootEpgNode,"Return");   //创建Return节点
		//if(m_sOpVersion == "3.0")
		//{
			pXPEpg.SetAttrNode(string("Type"),string("EPGScanQuery"),EpgreturnNode);
		//}
		//else
		//{
		//	pXPEpg.SetAttrNode(string("Type"),string("EPGQuery"),EpgreturnNode);
		//}
		pXPEpg.SetAttrNode(string("Value"),string("0"),EpgreturnNode);
		pXPEpg.SetAttrNode(string("Desc"),string("成功"),EpgreturnNode);
		pXMLNODE EpgreportNode = pXPEpg.CreateNodePtr(rootEpgNode,"ReturnInfo");
		pXMLNODE ChEpgreportNode;
		//if(m_sOpVersion == "3.0")
		//{
			ChEpgreportNode = pXPEpg.CreateNodePtr(EpgreportNode,"EPGScanQuery");
		//}
		//else
		//{
		//	ChEpgreportNode = pXPEpg.CreateNodePtr(EpgreportNode,"EPGQuery");
		//}
		pXPEpg.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),ChEpgreportNode);
		//*****
		cout<< TimeUtil::GetCurDateTime()<<"开始-DVBC-频道扫描............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		int iUnknownCount = 0;
		for(int i=0;i<FreqNum;i++)
		{
			long curfreq = digitfreq[i];
			if(sScan == "1")
			{
				i=FreqNum+1;
				curfreq = StrUtil::Str2Long(sFreq)*1000;
			}
			cout<< TimeUtil::GetCurDateTime()<<"扫描频点"<<curfreq<<"............."<<endl;
			string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";
			string strRet;
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&fre=";
			cmd+=StrUtil::Long2Str(curfreq);
			cmd+="&sym=";
			cmd+=freqscaninfo.SymbolRate;
			cmd+="&qam=2";
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
			//
			if(strRet.size() <= 0)
			{
				cout<<"ChanScanThread命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				continue;
			}
			
			string strtem = SkipHTTPHead(strRet);
			if (strtem == "")
			{
				cout<<"ChanScanThread没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				continue;
			}
			if (!ValidProXml(strtem))
				continue;
			XmlParser psr;
			psr.Set_xml(strtem);
			pXMLNODE node=psr.GetNodeFromPath("html/body");
			pXMLNODELIST nodeList = psr.GetNodeList(node);
			int count = nodeList->Size();
			pXMLNODE scanNode = NULL;
			bool epgfreqchange = true;
			pXMLNODE freqNode = NULL;
			pXMLNODE epgNode = NULL;
			string Freq = "", OrgNetID = "", TsID = "", QAM = "", SymbolRate = "",nitname = "",Nitid = "",pronum = "";
			for(int fk=0;fk<count;fk++)
			{
				pXMLNODE childNode = psr.GetNextNode(nodeList);
				string name = psr.GetNodeName(childNode);
				if(name == "TSDesc")
				{
					psr.GetAttrNode(childNode,"stmid",TsID);
					psr.GetAttrNode(childNode,"orgnitid",OrgNetID);
					psr.GetAttrNode(childNode,"nitid",Nitid);
					psr.GetAttrNode(childNode,"nitname",nitname);
					psr.GetAttrNode(childNode,"pronum",pronum);
					cout<< TimeUtil::GetCurDateTime()<<"扫描到节目:扫描频点"<<curfreq<<":"<<nitname<<"............."<<endl;
					if((pronum.size()==0)&&(nitname.find("pronum=") != -1))
					{
						pronum = nitname.substr(7,nitname.size());
					}
					//*****
					if(StrUtil::Str2Int(pronum) > 0)
					{
						if(StrUtil::Str2Int(pronum) != count-1)
						{
							cout<<"个数解析有问题....."<<curfreq<<endl;
							cout<<strtem<<endl;
						}
						//
						scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");    //创建ChannelScan节点
						pXPRet.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),scanNode);
						pXPRet.SetAttrNode(string("SymbolRate"),string("6875"),scanNode);
						pXPRet.SetAttrNode(string("QAM"),string("3"),scanNode);
						pXPRet.SetAttrNode(string("OrgNetID"),OrgNetID,scanNode);
						pXPRet.SetAttrNode(string("TsID"),TsID,scanNode);
					}
					//*****
				}
				else if(name == "ProDesc")
				{
					string program = "",ProgramID = "",ServiceID = "",VideoPID = "",AudioPID = "",ca = "",pmt = "",pcr = "",cur = "",provider = "";
					psr.GetAttrNode(childNode,"sid",ServiceID);
					ProgramID = ServiceID;
					psr.GetAttrNode(childNode,"vid",VideoPID);
					if ((sCheckHD == "1")&&(VideoPID.size()>0)&&(VideoPID != "0"))
					{
						//
						HDInfoForScan HDInfoF;
						HDInfoF.sFreq = StrUtil::Long2Str(curfreq);
						HDInfoF.sServiceID = ServiceID;
						HDInfoF.sHD = "0";
						HDInfoFreqList.push_back(HDInfoF);
						//
					}
					int vidpid =  StrUtil::Str2Int(VideoPID);
					vidpid = vidpid&0x1fff;
					VideoPID = StrUtil::Int2Str(vidpid);
					psr.GetAttrNode(childNode,"aud0",AudioPID);
					int audpid = StrUtil::Str2Int(AudioPID);
					audpid = audpid&0x1fff;
					AudioPID = StrUtil::Int2Str(audpid);
					psr.GetAttrNode(childNode,"ca",ca);
					psr.GetAttrNode(childNode,"name",program);
					if(program.size()==0)
					{
						psr.GetAttrNode(childNode,"provider",provider);
						if(provider.find("name=") != -1)
						{
							program = provider.substr(5,provider.size());
						}
					}
					//改变XML特殊字符""-%22,<>-%3C%3E修改为()%28%29 &-%26修改为A-%41 '-%27修改为`-%60
					int t33indx = 0;
					for(int timce = 0;timce<100;timce++)
					{
						if(program.find("%22")!=-1 || program.find("%3C")!=-1 || program.find("%3E")!=-1
							|| program.find("%3c")!=-1 || program.find("%3e")!=-1 || program.find("%26")!=-1
							|| program.find("%27")!=-1)
						{
							int pt22 = program.find("%22");
							if(pt22!=-1)
							{
								if(t33indx%2==0)
								{
									program.replace(pt22,3,"%28");
								}
								else
								{
									program.replace(pt22,3,"%29");
								}
								t33indx++;
							}
							int pt3c = program.find("%3C");
							if(pt3c!=-1)
							{
								program.replace(pt3c,3,"%28");
							}
							pt3c = program.find("%3c");
							if(pt3c!=-1)
							{
								program.replace(pt3c,3,"%28");
							}
							int pt3e = program.find("%3E");
							if(pt3e!=-1)
							{
								program.replace(pt3e,3,"%29");
							}
							pt3e = program.find("%3e");
							if(pt3e!=-1)
							{
								program.replace(pt3e,3,"%29");
							}
							int pt26 = program.find("%26");
							if(pt26!=-1)
							{
								program.replace(pt26,3,"%41");
							}
							int pt27 = program.find("%27");
							if(pt27!=-1)
							{
								program.replace(pt27,3,"%60");
							}
						}
						else
						{
							break;
						}
					}
					//-----字符转化-----
					char* src = (char*)program.data();
					char *dest = new char[300];
					memset(dest,0,sizeof(char)*300);
					bw_encodestr(dest,src);
					program = dest;
					delete []dest;
					//----------------
					string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
					if(program.find(str) != -1)
					{
						program = program.substr(str.size(),program.size());
					}
					if (program == "")
					{
						program = "unknown";
						program += StrUtil::Int2Str(iUnknownCount);
						iUnknownCount++;
					}
					psr.GetAttrNode(childNode,"pmt",pmt);
					psr.GetAttrNode(childNode,"pcr",pcr);
					//*****
					if(scanNode)
					{
						pXMLNODE channelNode = pXPRet.CreateNodePtr(scanNode,"Channel");  //创建Channel节点
						pXPRet.SetAttrNode(string("ProgramID"),ServiceID,channelNode);
						pXPRet.SetAttrNode(string("Program"),program,channelNode);
						pXPRet.SetAttrNode(string("ServiceType"),string("1"), channelNode);	
						string sHDTV = "0";
						if (program.find("高清") != string::npos || program.find("HD") != string::npos)
						{
							sHDTV = "1";
						}
						else
						{
							sHDTV = "0";
						}
						pXPRet.SetAttrNode(string("HDTV"),sHDTV,channelNode);
						pXPRet.SetAttrNode(string("PmtPid"),pmt,channelNode); 
						pXPRet.SetAttrNode(string("PcrPid"),pcr,channelNode);		 
						pXPRet.SetAttrNode(string("ServiceID"),ServiceID,channelNode);  
						pXPRet.SetAttrNode(string("VideoPID"),VideoPID,channelNode);
						pXPRet.SetAttrNode(string("AudioPID"),AudioPID,channelNode);
						int ica = StrUtil::Str2Int(ca);
						if((ica%2)==1)//只能区分清流和加扰流,加扰已授权和未授权不能区分,手动配置
						{
							pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
							pXPRet.SetAttrNode(string("Encryption"), string("1"), channelNode);
						}
						else
						{
							if((AudioPID.size()>0)&&(AudioPID != "0"))
							{
								pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
							}
							else
							{
								pXPRet.SetAttrNode(string("Encrypt"),string("0"), channelNode);
							}
							pXPRet.SetAttrNode(string("Encryption"), string("0"), channelNode);
						}
						pXMLNODE StreamNode = pXPRet.CreateNodePtr(channelNode,"elementryPID");
						pXPRet.SetAttrNode("StreamType",string("2"),StreamNode);
						pXPRet.SetAttrNode("PID",VideoPID,StreamNode);
						pXMLNODE StreamNode1 = pXPRet.CreateNodePtr(channelNode,"elementryPID");
						pXPRet.SetAttrNode("StreamType",string("4"),StreamNode1);
						pXPRet.SetAttrNode("PID",AudioPID,StreamNode1);
						//******************************************EPG***************************
						if((bepg == 1||bepg == 2)&&(AudioPID.size()>0)&&(AudioPID != "0"))
						{
							cmd = "";
							strRet = "";
							cmd = "POST /device/epgsearch  HTTP/1.1  \r\n\r\n";
							cmd+="chl=";
							cmd+=tdev.s_cardindex;
							cmd+="_";
							cmd+=tdev.s_chanindex;
							cmd+="&fre=";
							if(bepg == 2)
							{
								cmd+=sFreq;
								cmd+=string("000");
							}
							else
							{
								cmd+=StrUtil::Long2Str(curfreq);
							}
							cmd+="&sym=";
							cmd+=freqscaninfo.SymbolRate;
							cmd+="&qam=2&orgnitid=";
							cmd+=OrgNetID;
							cmd+="&nitid=";
							cmd+=Nitid;
							cmd+="&stmid=";
							cmd+=TsID;
							cmd+="&sid=";
							cmd+=ServiceID;
							pDeviceAccess->SendTaskMsg(cmd,strRet);
							//
							bool epgseridchange = true;
							//
							if(strRet.size() <= 0)
							{
								cout<<"ChanScanThread-EPG命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
								continue;
							}
							string strtem = SkipHTTPHead(strRet);
							if (strtem == "")
							{
								cout<<"ChanScanThread-EPG没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
								continue;
							}
							string lastepgdate = "";
							XmlParser epgpsr;
							epgpsr.Set_xml(strtem);
							pXMLNODE epgnode=epgpsr.GetNodeFromPath("html/body");
							pXMLNODELIST epgnodeList = epgpsr.GetNodeList(epgnode);
							int epgcount = epgnodeList->Size();
							for(int ek=0;ek<epgcount;ek++)
							{
								pXMLNODE childNode = epgpsr.GetNextNode(epgnodeList);
								string evname = epgpsr.GetNodeName(childNode);
								if(evname == "Even")
								{
									if(epgfreqchange)
									{
										epgfreqchange = false;
										//if(m_sOpVersion == "3.0")
										//{
											freqNode = pXPEpg.CreateNodePtr(ChEpgreportNode,"FreqInfo");    
											pXPEpg.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),freqNode);
										//}
										//else
										//{
										//	epgNode = pXPEpg.CreateNodePtr(ChEpgreportNode,"EPGInfo");    
										//	pXPEpg.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),epgNode);
										//}
									}
									if(epgseridchange)
									{
										epgseridchange = false;
										//if(m_sOpVersion == "3.0")
										//{
											epgNode = pXPEpg.CreateNodePtr(freqNode,"ChannelInfo");    
											pXPEpg.SetAttrNode(string("ServiceID"),ServiceID,epgNode);
										//}
									}
									string id = "",type = "",status = "",free = "",start = "",dura = "",name = "";
									epgpsr.GetAttrNode(childNode,"id",id);
									epgpsr.GetAttrNode(childNode,"typ",type);
									epgpsr.GetAttrNode(childNode,"sta",status);
									epgpsr.GetAttrNode(childNode,"fre",free);
									epgpsr.GetAttrNode(childNode,"cur",cur);
									epgpsr.GetAttrNode(childNode,"str",start);
									epgpsr.GetAttrNode(childNode,"dur",dura);
									epgpsr.GetAttrNode(childNode,"nm",name);
									if((start.size() <= 0)||(dura.size() <= 0)||(name.size() <= 0))
									{
										continue;
									}
									//改变XML特殊字符""-%22,<>-%3C%3E修改为()%28%29 &-%26修改为A-%41 '-%27修改为`-%60
									int t22indx = 0;
									for(int timce = 0;timce<100;timce++)
									{
										if(name.find("%22")!=-1 || name.find("%3C")!=-1 || name.find("%3E")!=-1
											|| name.find("%3c")!=-1 || name.find("%3e")!=-1 || name.find("%26")!=-1
											|| name.find("%27")!=-1)
										{
											int pt22 = name.find("%22");
											if(pt22!=-1)
											{
												if(t22indx%2==0)
												{
													name.replace(pt22,3,"%28");
												}
												else
												{
													name.replace(pt22,3,"%29");
												}
												t22indx++;
											}
											int pt3c = name.find("%3C");
											if(pt3c!=-1)
											{
												name.replace(pt3c,3,"%28");
											}
											pt3c = name.find("%3c");
											if(pt3c!=-1)
											{
												name.replace(pt3c,3,"%28");
											}
											int pt3e = name.find("%3E");
											if(pt3e!=-1)
											{
												name.replace(pt3e,3,"%29");
											}
											pt3e = name.find("%3e");
											if(pt3e!=-1)
											{
												name.replace(pt3e,3,"%29");
											}
											int pt26 = name.find("%26");
											if(pt26!=-1)
											{
												name.replace(pt26,3,"%41");
											}
											int pt27 = name.find("%27");
											if(pt27!=-1)
											{
												name.replace(pt27,3,"%60");
											}
										}
										else
										{
											break;
										}
									}
									//-----字符转化-----
									char* epgsrc = (char*)name.data();
									char *epgdest = new char[600];
									memset(epgdest,0,sizeof(char)*600);
									bw_encodestr(epgdest,epgsrc);
									//
									string strtedescm = epgdest;
									if(strtedescm.find("?")!=-1||strtedescm.find("&")!=-1||strtedescm.find("#")!=-1
										||strtedescm.find(";")!=-1)
									{
										cout<<name<<"-->"<<strtedescm<<endl;
										APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
										APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
										APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,name,LOG_OUTPUT_FILE);
										APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strtedescm,LOG_OUTPUT_FILE);
									}
									//
									name = epgdest;
									delete []epgdest;
									//---------------------
									if(epgNode)
									{
										string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
										if(name.find(str) != -1)
										{
											name = name.substr(str.size(),name.size());
										}
										string strymd = start.substr(2,4);
										char temp = 't';
										char *str123 = &temp; 
										long lmdj = (long)strtol(strymd.c_str(), &str123, 16);//
										int y = int(float(lmdj-15078.2)/365.25);
										int m = int(float(lmdj-14956.1-int(y*365.25))/30.6001);
										int d = lmdj-14956-int(y*365.25)-int(m*30.6001);
										int k=0;
										if(m==14||m==15)
										{
											k=1;
										}
										y = y+k+1900;
										m=m-1-k*12;
										string date = "";
										string strtime = "";
										char now[64];
										memset(now,0,64);
										sprintf(now, "%04d-%02d-%02d", y, m, d);
										date = now;
										strtime = start.substr(6,2);
										strtime+=":";
										strtime+=start.substr(8,2);
										strtime+=":";
										strtime+=start.substr(10,2);
										start = date+" "+strtime;
										//
										start = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(start)+8*3600);//8小时校准
										if(((TimeUtil::StrToDateTime(start)-time(0))<=(24*30*3600))&&((TimeUtil::StrToDateTime(start)-time(0))>=(-7*30*3600)))//避免时间异常平台报格式错误
										{
											lastepgdate = start;
										}
										else
										{
											if(lastepgdate.size()>2)
											{
												start = lastepgdate;
											}
										}
										//
										if(dura.size() == 4)
										{
											strtime="00:00:";
											strtime+=dura.substr(2,2);
										}
										else if(dura.size() == 3)
										{
											strtime="00:00:0";
											strtime+=dura.substr(2,1);
										}
										else if(dura.size() == 5)
										{
											strtime="00:0";
											strtime+=dura.substr(2,1);
											strtime+=":";
											strtime+=dura.substr(3,2);
										}
										else if(dura.size() == 6)
										{
											strtime="00:";
											strtime+=dura.substr(2,2);
											strtime+=":";
											strtime+=dura.substr(4,2);
										}
										else if(dura.size() == 8)
										{
											strtime = dura.substr(2,2);
											strtime+=":";
											strtime+=dura.substr(4,2);
											strtime+=":";
											strtime+=dura.substr(6,2);
										}
										else if(dura.size() == 7)
										{
											strtime = "0";
											strtime += dura.substr(2,1);
											strtime+=":";
											strtime+=dura.substr(3,2);
											strtime+=":";
											strtime+=dura.substr(5,2);
										}
										else
										{
											cout<<"dura:"<<dura<<endl;
										}
										if(ValidEpgDur(strtime))
										{
											dura = strtime;
										}
										else
										{
											dura = "00:01:00";//随机值 避免平台显示语法失败
										}
										//
										//if(m_sOpVersion == "3.0")
										//{
											pXMLNODE epgchNode = pXPEpg.CreateNodePtr(epgNode,"EPGInfo");
											pXPEpg.SetAttrNode(string("EventName"),name,epgchNode);
											pXPEpg.SetAttrNode(string("EventType"),type,epgchNode);
											pXPEpg.SetAttrNode(string("StartTime"),start,epgchNode);  //设置StartTime
											pXPEpg.SetAttrNode(string("Duration"),dura,epgchNode);  //设置ProgramLen属性值
										//}
										//else
										//{
										//	pXMLNODE epgchNode = pXPEpg.CreateNodePtr(epgNode,"EPG");
										//	pXPEpg.SetAttrNode(string("ProgramID"),ServiceID,epgchNode);
										//	pXPEpg.SetAttrNode(string("Program"),name,epgchNode);
										//	pXPEpg.SetAttrNode(string("ProgramType"),type,epgchNode);
										//	pXPEpg.SetAttrNode(string("StartTime"),start,epgchNode);  //设置StartTime
										//	pXPEpg.SetAttrNode(string("ProgramLen"),dura,epgchNode);  //设置ProgramLen属性值
										//	pXPEpg.SetAttrNode(string("State"),status,epgchNode);         //设置State属性值
										//	pXPEpg.SetAttrNode(string("Encryption"),free,epgchNode);       //设置Encryption属性值
										//}
									}
								}
							}
						}	
						//******************************************EPG***************************
					}
				}
			}
		}
		pXPRet.SaveAsFile(filename.c_str());
		pXPEpg.SaveAsFile(epgfilename.c_str());
		if(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0")
		{
			if((HDInfoFreqList.size()>0)&&(sCheckHD == "1")&&(m_sNoticeXml == ""))
			{
				cout<< TimeUtil::GetCurDateTime()<<"结束-DVBC-SI扫描............."<<endl;
				cout<< TimeUtil::GetCurDateTime()<<"开始-DVBC-高标清扫描............."<<endl;
				for(int i=0;i<HDInfoFreqList.size();i++)
				{
					/*POST /device/vfmtsearch  HTTP/1.1  \r\n\r\n
					chl=0_0&fre=698000&sym=6875&qam=2&sid=101
					*/
					string strVfmtSearch = "POST /device/vfmtsearch  HTTP/1.1  \r\n\r\n";
					strVfmtSearch+="chl=";
					strVfmtSearch+=tdev.s_cardindex;
					strVfmtSearch+="_";
					strVfmtSearch+=tdev.s_chanindex;
					strVfmtSearch+="&fre=";
					strVfmtSearch+=HDInfoFreqList[i].sFreq;
					strVfmtSearch+="&sym=";
					strVfmtSearch+=freqscaninfo.SymbolRate;
					strVfmtSearch+="&qam=2&sid=";
					strVfmtSearch+=HDInfoFreqList[i].sServiceID;
					string strRetVfmtSearch;
					cout<< TimeUtil::GetCurDateTime()<<"--"<<HDInfoFreqList[i].sFreq<<"--"<<HDInfoFreqList[i].sServiceID<<endl;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strVfmtSearch,LOG_OUTPUT_FILE);
					pDeviceAccess->SendTaskMsg(strVfmtSearch,strRetVfmtSearch);
					cout<< TimeUtil::GetCurDateTime()<<"--"<<strRetVfmtSearch<<endl;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRetVfmtSearch,LOG_OUTPUT_FILE);
					if(strRetVfmtSearch.find("html/body") == -1)
					{
						cout<<"strVfmtSearch命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
						continue;
					}
					for(int re = 0; re<2;re++)
					{
						bool bok = false;
						//
						XmlParser psr(strRetVfmtSearch.c_str());
						pXMLNODE node=psr.GetNodeFromPath("html/body");
						pXMLNODELIST nodeList = psr.GetNodeList(node);
						int count = nodeList->Size();
						for(int k=0;k<count;k++)
						{
							pXMLNODE childNode = psr.GetNextNode(nodeList);
							string name = psr.GetNodeName(childNode);
							if(name == "ProDesc")
							{
								string sid, vfmt, width, height;
								psr.GetAttrNode(childNode, "sid", sid);
								psr.GetAttrNode(childNode, "vfmt", vfmt);
								if (sid == "")
								{
									continue;
								}
								if(sid == HDInfoFreqList[i].sServiceID)
								{
									int ipos = vfmt.find("_");
									if (ipos != string::npos)
									{
										width = vfmt.substr(0, ipos);
										height = vfmt.substr(ipos+1, vfmt.length());
									}
									if(vfmt != "0_0")
									{
										if (StrUtil::Str2Int(width) >= 1280 && StrUtil::Str2Int(height) >= 720)
										{//HD 720p=1280*720; 1080p=1920*1080;
											HDInfoFreqList[i].sHD = string("1");
										}
										else
										{
											HDInfoFreqList[i].sHD = string("0");
										}	
										bok = true;
									}
									else
									{
										HDInfoFreqList[i].sHD = string("-1");
									}
									break;
								}
							}
						}
						if((!bok)&&(re == 0))
						{
							cout<<HDInfoFreqList[i].sServiceID<<":分辨率获取失败,再执行一次.."<<endl;
							strRetVfmtSearch = "";
							APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,"再执行一此:",LOG_OUTPUT_FILE);
							pDeviceAccess->SendTaskMsg(strVfmtSearch,strRetVfmtSearch);
							cout<< TimeUtil::GetCurDateTime()<<"--"<<strRetVfmtSearch<<endl;
							APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRetVfmtSearch,LOG_OUTPUT_FILE);
							if(strRetVfmtSearch.size() < 0)
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
				//**************************
				pXMLNODE rtnChannelScanReportNode=pXPRet.GetNodeFromPath("Msg/ChannelScanReport");
				pXMLNODELIST nodeList=pXPRet.GetNodeList(rtnChannelScanReportNode);
				for (int j=0;j!=nodeList->Size();++j)
				{
					string Freq = "";
					pXMLNODE childNode=pXPRet.GetNextNode(nodeList);
					pXPRet.GetAttrNode(childNode,"Freq",Freq);
					pXMLNODELIST childNodeList=pXPRet.GetNodeList(childNode);
					for (int k=0;k!=childNodeList->Size();++k)
					{
						string ServiceID = "";
						pXMLNODE subChildNode=pXPRet.GetNextNode(childNodeList);
						pXPRet.GetAttrNode(subChildNode,"ServiceID",ServiceID);	
						string HDi = "";
						if(GetHDInfo(Freq,ServiceID,HDInfoFreqList,HDi))
						{
							if(HDi != "-1")
							{
								pXPRet.SetAttrNode("HDTV",HDi,subChildNode);	
								pXPRet.SetAttrNode("Encrypt",string("1"),subChildNode);	
							}
							else
							{
								pXPRet.SetAttrNode("Encrypt",string("0"),subChildNode);	
							}
						}
					}
				}
				pXPRet.SaveAsFile(filename.c_str());
				//**************************
			}
		}
		delete pDeviceAccess;
		cout<< TimeUtil::GetCurDateTime()<<"结束-DVBC-频道扫描............."<<endl;
		//
	
		BUSINESSITERFACEMGR::instance()->UplaodXML(CreatePreChanScanRet(DVBC));
		if((m_sNoticeXmlTwo.size()>0) &&(m_sNoticeXmlTwo.find("Type=\"3\"")!=-1)||((m_sOpVersion == "3.0")&&(m_sNoticeXml == "")))
		{
			BUSINESSITERFACEMGR::instance()->UplaodXML(CreatePreChanScanRetTwo(DVBC));
		}
	}
	return true;
}
//CTTB 频道扫描
bool ChanScanThread::ChanScanForCTTB(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq)
{
	string Path = "";
	PROPMANAGER::instance()->GetXmlTablePath(CTTB,Path);
	string filename = Path + string("ChannelScanQuery.xml");
	string epgfilename = Path + string("EPGQuery.xml");

	char* strXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	char* strEpgXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	int idevID = StrUtil::Str2Int(sDevID);
	int iUnknownCount = 0;
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID, CTTB, tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0, CTTB, idevID, tunermonitorip, tunermonitorport);//tuner
	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		XmlParser pXPRet(strXML);
		pXMLNODE rootNode = pXPRet.GetRootNode();
		pXPRet.SetAttrNode(string("DVBType"),string("CTTB"),rootNode);
		pXMLNODE  returnNode = pXPRet.CreateNodePtr(rootNode,"Return");   //创建Return节点
		pXPRet.SetAttrNode(string("Type"),string("ChannelScanQuery"),returnNode);
		pXPRet.SetAttrNode(string("Value"),string("0"),returnNode);
		pXPRet.SetAttrNode(string("Desc"),string("成功"),returnNode);
		pXPRet.SetAttrNode(string("Comment"),string(""),returnNode);
		pXPRet.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),returnNode);
		pXMLNODE reportNode = pXPRet.CreateNodePtr(rootNode,"ChannelScanReport");

		//*****
		XmlParser pXPEpg(strEpgXML);
		pXMLNODE rootEpgNode = pXPEpg.GetRootNode();
		pXPEpg.SetAttrNode(string("DVBType"),string("CTTB"),rootEpgNode);
		pXMLNODE  EpgreturnNode = pXPEpg.CreateNodePtr(rootEpgNode,"Return");   //创建Return节点
		pXPEpg.SetAttrNode(string("Type"),string("EPGQuery"),EpgreturnNode);
		pXPEpg.SetAttrNode(string("Value"),string("0"),EpgreturnNode);
		pXPEpg.SetAttrNode(string("Desc"),string("成功"),EpgreturnNode);
		pXMLNODE EpgreportNode = pXPEpg.CreateNodePtr(rootEpgNode,"ReturnInfo");
		pXMLNODE ChEpgreportNode = pXPEpg.CreateNodePtr(EpgreportNode,"EPGQuery");
		pXPEpg.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),ChEpgreportNode);
		//*****


		cout<< TimeUtil::GetCurDateTime()<<"开始-DTMB-频道扫描............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		
		for(int i=0; i < FreqNum; i++)
		{
			long curfreq = digitfreq[i];
			if(sScan == "1" && !bweeklyswitch)
			{
				i=FreqNum+1;
				curfreq = StrUtil::Str2Long(sFreq)*1000;
			}
			cout<< TimeUtil::GetCurDateTime()<<"扫描频点"<<curfreq<<"............."<<endl;
			string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_0";
			cmd+="&fre=";
			cmd+=StrUtil::Long2Str(curfreq);
			cmd+="&bandwith=8&spectrum=0";
			string strRet;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			std::string msg = "频道扫描:";
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
			//
			if(strRet.size() <= 0)
			{
				cout<<"ChanScanThd-DTMB命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				continue;
			}
			string strtem = SkipHTTPHead(strRet);
			if (strtem == "")
			{
				cout<<"ChanScanThd-DTMB没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				continue;
			}
			XmlParser psr;
			psr.Set_xml(strRet);
			pXMLNODE node=psr.GetNodeFromPath("html/body");
			pXMLNODELIST nodeList = psr.GetNodeList(node);
			int count = nodeList->Size();
			pXMLNODE scanNode = NULL;
			bool epgfreqchange = true;
			pXMLNODE epgNode = NULL;
			string Freq = "", OrgNetID = "", TsID = "", QAM = "", SymbolRate = "",nitname = "",Nitid = "",pronum = "";
			for(int k=0;k<count;k++)
			{
				pXMLNODE childNode = psr.GetNextNode(nodeList);
				string name = psr.GetNodeName(childNode);
				if(name == "TSDesc")
				{
					psr.GetAttrNode(childNode,"stmid",TsID);
					psr.GetAttrNode(childNode,"orgnitid",OrgNetID);
					psr.GetAttrNode(childNode,"nitid",Nitid);
					psr.GetAttrNode(childNode,"nitname",nitname);
					psr.GetAttrNode(childNode,"pronum",pronum);
					cout<< TimeUtil::GetCurDateTime()<<"扫描到节目:扫描频点"<<curfreq<<":"<<nitname<<"............."<<endl;
					if((pronum.size()==0)&&(nitname.find("pronum=") != -1))
					{
						pronum = nitname.substr(7,nitname.size());
					}
					//*****
					if(StrUtil::Str2Int(pronum) > 0)
					{
						if(StrUtil::Str2Int(pronum) != count-1)
						{
							cout<<"个数解析有问题....."<<curfreq<<endl;
							cout<<strtem<<endl;
						}
						scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");    //创建ChannelScan节点
						pXPRet.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),scanNode);
						pXPRet.SetAttrNode(string("SymbolRate"),string("6875"),scanNode);
						pXPRet.SetAttrNode(string("QAM"),string("3"),scanNode);
						pXPRet.SetAttrNode(string("OrgNetID"),OrgNetID,scanNode);
						pXPRet.SetAttrNode(string("TsID"),TsID,scanNode);
					}
					//*****
				}
				else if(name == "ProDesc")
				{
					string program = "",ProgramID = "",ServiceID = "",VideoPID = "",AudioPID = "",ca = "",pmt = "",pcr = "",cur = "",provider = "";
					psr.GetAttrNode(childNode,"sid",ServiceID);
					ProgramID = ServiceID;
					psr.GetAttrNode(childNode,"vid",VideoPID);
					int vidpid =  StrUtil::Str2Int(VideoPID);
					vidpid = vidpid&0x1fff;
					VideoPID = StrUtil::Int2Str(vidpid);
					psr.GetAttrNode(childNode,"aud0",AudioPID);
					int audpid = StrUtil::Str2Int(AudioPID);
					audpid = audpid&0x1fff;
					AudioPID = StrUtil::Int2Str(audpid);
					psr.GetAttrNode(childNode,"ca",ca);
					psr.GetAttrNode(childNode,"name",program);
					if(program.size()==0)
					{
						psr.GetAttrNode(childNode,"provider",provider);
						if(provider.find("name=") != -1)
						{
							program = provider.substr(5,provider.size());
						}
					}
					//改变XML特殊字符""-%22,<>-%3C%3E修改为()%28%29 &-%26修改为A-%41 '-%27修改为`-%60
					int t33indx = 0;
					for(int timce = 0;timce<100;timce++)
					{
						if(program.find("%22")!=-1 || program.find("%3C")!=-1 || program.find("%3E")!=-1
							|| program.find("%3c")!=-1 || program.find("%3e")!=-1 || program.find("%26")!=-1
							|| program.find("%27")!=-1)
						{
							int pt22 = program.find("%22");
							if(pt22!=-1)
							{
								if(t33indx%2==0)
								{
									program.replace(pt22,3,"%28");
								}
								else
								{
									program.replace(pt22,3,"%29");
								}
								t33indx++;
							}
							int pt3c = program.find("%3C");
							if(pt3c!=-1)
							{
								program.replace(pt3c,3,"%28");
							}
							pt3c = program.find("%3c");
							if(pt3c!=-1)
							{
								program.replace(pt3c,3,"%28");
							}
							int pt3e = program.find("%3E");
							if(pt3e!=-1)
							{
								program.replace(pt3e,3,"%29");
							}
							pt3e = program.find("%3e");
							if(pt3e!=-1)
							{
								program.replace(pt3e,3,"%29");
							}
							int pt26 = program.find("%26");
							if(pt26!=-1)
							{
								program.replace(pt26,3,"%41");
							}
							int pt27 = program.find("%27");
							if(pt27!=-1)
							{
								program.replace(pt27,3,"%60");
							}
						}
						else
						{
							break;
						}
					}
					//-----字符转化-----
					char* src = (char*)program.data();
					char *dest = new char[300];
					memset(dest,0,sizeof(char)*300);
					bw_encodestr(dest,src);
					program = dest;
					delete []dest;
					//----------------
					string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
					if(program.find(str) != -1)
					{
						program = program.substr(str.size(),program.size());
					}
					if (program == "")
					{
						program = "unknown";
						program += StrUtil::Int2Str(iUnknownCount);
						iUnknownCount++;
					}
					psr.GetAttrNode(childNode,"pmt",pmt);
					psr.GetAttrNode(childNode,"pcr",pcr);
					//*****
					if(scanNode)
					{
						pXMLNODE channelNode = pXPRet.CreateNodePtr(scanNode,"Channel");  //创建Channel节点
						pXPRet.SetAttrNode(string("ProgramID"),ServiceID,channelNode);
						pXPRet.SetAttrNode(string("Program"),program,channelNode);
						pXPRet.SetAttrNode(string("ServiceType"),string("1"), channelNode);					
						if (program.find("高清") != string::npos || program.find("HD") != string::npos)
						{
							pXPRet.SetAttrNode(string("HDTV"),string("1"),channelNode);
						}
						else
						{
							pXPRet.SetAttrNode(string("HDTV"),string("0"),channelNode);
						}
						pXPRet.SetAttrNode(string("PmtPid"),pmt,channelNode); 
						pXPRet.SetAttrNode(string("PcrPid"),pcr,channelNode);		 
						pXPRet.SetAttrNode(string("ServiceID"),ServiceID,channelNode);  
						pXPRet.SetAttrNode(string("VideoPID"),VideoPID,channelNode);
						pXPRet.SetAttrNode(string("AudioPID"),AudioPID,channelNode);
						int ica = StrUtil::Str2Int(ca);
						if((ica%2)==1)//只能区分清流和加扰流,加扰已授权和未授权不能区分,手动配置
						{
							pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
							pXPRet.SetAttrNode(string("Encryption"), string("1"), channelNode);
						}
						else
						{
							pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
							pXPRet.SetAttrNode(string("Encryption"), string("0"), channelNode);
						}
						pXMLNODE StreamNode = pXPRet.CreateNodePtr(channelNode,"elementryPID");
						pXPRet.SetAttrNode("StreamType",string("2"),StreamNode);
						pXPRet.SetAttrNode("PID",VideoPID,StreamNode);
						pXMLNODE StreamNode1 = pXPRet.CreateNodePtr(channelNode,"elementryPID");
						pXPRet.SetAttrNode("StreamType",string("4"),StreamNode1);
						pXPRet.SetAttrNode("PID",AudioPID,StreamNode1);
						//******************************************EPG***************************
		
						if(bepg == 1||bepg == 2)
						{
							cmd = "";
							strRet = "";
							cmd = "POST /device/epgsearch  HTTP/1.1  \r\n\r\n";
							cmd+="chl=";
							cmd+=tdev.s_cardindex;
							cmd+="_";
							cmd+=tdev.s_chanindex;
							cmd+="&fre=";
							cmd+=StrUtil::Long2Str(curfreq);
							cmd+="&bandwith=8&spectrum=0&orgnitid=";
							cmd+=OrgNetID;
							cmd+="&nitid=";
							cmd+=Nitid;
							cmd+="&stmid=";
							cmd+=TsID;
							cmd+="&sid=";
							cmd+=ServiceID;
							pDeviceAccess->SendTaskMsg(cmd,strRet);
							//
							if(strRet.size() <= 0)
							{
								cout<<"ChanScanThread-DTMB-EPG命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
								continue;
							}
							string strtem = SkipHTTPHead(strRet);
							if (strtem == "")
							{
								cout<<"ChanScanThread-DTMB-EPG没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
								continue;
							}
							XmlParser epgpsr;
							epgpsr.Set_xml(strtem);
							pXMLNODE epgnode=epgpsr.GetNodeFromPath("html/body");
							pXMLNODELIST epgnodeList = epgpsr.GetNodeList(epgnode);
							int epgcount = epgnodeList->Size();
							for(int k=0;k<epgcount;k++)
							{
								pXMLNODE childNode = epgpsr.GetNextNode(epgnodeList);
								string evname = epgpsr.GetNodeName(childNode);
								if(evname == "Even")
								{
									if(epgfreqchange)
									{
										epgfreqchange = false;
										epgNode = pXPEpg.CreateNodePtr(ChEpgreportNode,"EPGInfo");    //创建ChannelScan节点
										pXPEpg.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),epgNode);
									}
									string id="",type="",status="",free="",start="",dura="",name="";
									epgpsr.GetAttrNode(childNode,"id",id);
									epgpsr.GetAttrNode(childNode,"typ",type);
									epgpsr.GetAttrNode(childNode,"sta",status);
									epgpsr.GetAttrNode(childNode,"fre",free);
									epgpsr.GetAttrNode(childNode,"cur",cur);
									epgpsr.GetAttrNode(childNode,"str",start);
									epgpsr.GetAttrNode(childNode,"dur",dura);
									epgpsr.GetAttrNode(childNode,"nm",name);
									if(epgNode)
									{
										string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
										if(name.find(str) != -1)
										{
											name = name.substr(str.size(),name.size());
										}
										pXMLNODE epgchNode = pXPEpg.CreateNodePtr(epgNode,"EPG");
										pXPEpg.SetAttrNode(string("ProgramID"),ServiceID,epgchNode);
										pXPEpg.SetAttrNode(string("Program"),name,epgchNode);
										pXPEpg.SetAttrNode(string("ProgramType"),type,epgchNode);
										//
										string strymd = start.substr(2,4);
										char temp = 't';
										char *str123 = &temp; 
										long lmdj = (long)strtol(strymd.c_str(), &str123, 16);//
										int y = int(float(lmdj-15078.2)/365.25);
										int m = int(float(lmdj-14956.1-int(y*365.25))/30.6001);
										int d = lmdj-14956-int(y*365.25)-int(m*30.6001);
										int k=0;
										if(m==14||m==15)
										{
											k=1;
										}
										y = y+k+1900;
										m=m-1-k*12;
										string date = "";
										string time = "";
										char now[64];
										memset(now,0,64);
										sprintf(now, "%04d-%02d-%02d", y, m, d);
										date = now;
										time = start.substr(6,2);
										int iHour = StrUtil::Str2Int(time)+8;
										if (iHour >= 24)
										{
											iHour -= 24;
										}
										if (iHour >= 0)
										{
											char buf[3] = {0};
											sprintf(buf, "%02d", iHour);
											time = buf;
										}
										time+=":";
										time+=start.substr(8,2);
										time+=":";
										time+=start.substr(10,2);
										start = date+" "+time;
										//
										if(dura.size() == 4)
										{
											time="00:00:";
											time+=dura.substr(2,2);
										}
										else if(dura.size() == 3)
										{
											time="00:00:0";
											time+=dura.substr(2,1);
										}
										else if(dura.size() == 5)
										{
											time="00:0";
											time+=dura.substr(2,1);
											time+=":";
											time+=dura.substr(3,2);
										}
										else if(dura.size() == 6)
										{
											time="00:";
											time+=dura.substr(2,2);
											time+=":";
											time+=dura.substr(4,2);
										}
										else if(dura.size() == 8)
										{
											time = dura.substr(2,2);
											time+=":";
											time+=dura.substr(4,2);
											time+=":";
											time+=dura.substr(6,2);
										}
										else if(dura.size() == 7)
										{
											time = "0";
											time += dura.substr(2,1);
											time+=":";
											time+=dura.substr(3,2);
											time+=":";
											time+=dura.substr(5,2);
										}
										else
										{
											cout<<"dura:"<<dura<<endl;
										}
										dura = time;
										//
										pXPEpg.SetAttrNode(string("StartTime"),start,epgchNode);  //设置StartTime
										pXPEpg.SetAttrNode(string("ProgramLen"),dura,epgchNode);  //设置ProgramLen属性值
										pXPEpg.SetAttrNode(string("State"),status,epgchNode);         //设置State属性值
										pXPEpg.SetAttrNode(string("Encryption"),free,epgchNode);       //设置Encryption属性值
									}
								}
							}
						}	
						//******************************************EPG***************************
					}
				}
			}
			//
		}
		delete pDeviceAccess;
		pXPRet.SaveAsFile(filename.c_str());
		pXPEpg.SaveAsFile(epgfilename.c_str());
		cout<< TimeUtil::GetCurDateTime()<<"结束-DTMB-频道扫描............."<<endl;
		if (sScan == "0" || bweeklyswitch)//全频道扫描时再上报，单频点上报没意义
		{
			BUSINESSITERFACEMGR::instance()->UplaodXML(CreatePreChanScanRet(CTTB));
		}
	}
	return true;
}
bool ChanScanThread::ChanScanForDVBS(std::string sDevID)
{
	int bepg = 1;
	//
	string Path;
	PROPMANAGER::instance()->GetXmlTablePath(DVBS,Path);
	string filename = Path + string("ChannelScanQuery.xml");
	string epgfilename = Path + string("EPGQuery.xml");

	char* strXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	char* strEpgXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	int idevID = StrUtil::Str2Int(sDevID);
	int iUnknownCount = 0;
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID, DVBS, tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0, DVBS, idevID, tunermonitorip, tunermonitorport);//tuner
	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		XmlParser pXPRet(strXML);
		pXMLNODE rootNode = pXPRet.GetRootNode();
		pXPRet.SetAttrNode(string("DVBType"),string("DVBS"),rootNode);
		pXMLNODE  returnNode = pXPRet.CreateNodePtr(rootNode,"Return");   //创建Return节点
		pXPRet.SetAttrNode(string("Type"),string("ChannelScanQuery"),returnNode);
		pXPRet.SetAttrNode(string("Value"),string("0"),returnNode);
		pXPRet.SetAttrNode(string("Desc"),string("成功"),returnNode);
		pXPRet.SetAttrNode(string("Comment"),string(""),returnNode);
		pXPRet.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),returnNode);
		pXMLNODE reportNode = pXPRet.CreateNodePtr(rootNode,"ChannelScanReport");

		//*****
		XmlParser pXPEpg(strEpgXML);
		pXMLNODE rootEpgNode = pXPEpg.GetRootNode();
		pXPEpg.SetAttrNode(string("DVBType"),string("DVBS"),rootEpgNode);
		pXMLNODE  EpgreturnNode = pXPEpg.CreateNodePtr(rootEpgNode,"Return");   //创建Return节点
		pXPEpg.SetAttrNode(string("Type"),string("EPGQuery"),EpgreturnNode);
		pXPEpg.SetAttrNode(string("Value"),string("0"),EpgreturnNode);
		pXPEpg.SetAttrNode(string("Desc"),string("成功"),EpgreturnNode);
		pXMLNODE EpgreportNode = pXPEpg.CreateNodePtr(rootEpgNode,"ReturnInfo");
		pXMLNODE ChEpgreportNode = pXPEpg.CreateNodePtr(EpgreportNode,"EPGQuery");
		pXPEpg.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),ChEpgreportNode);
		//*****


		cout<< TimeUtil::GetCurDateTime()<<"开始-DVBS-频道扫描............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		
		for(int i = 0;i < sdvbsfreqinfoList.size();i++)
		{
			cout<< TimeUtil::GetCurDateTime()<<"扫描频点"<<sdvbsfreqinfoList[i].slocfreq<<"-"<<sdvbsfreqinfoList[i].sdownfreq<<"............."<<endl;
			string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&polar=0&tone=0&localfreq=";
			cmd+=sdvbsfreqinfoList[i].slocfreq;
			cmd+="&downfreq=";
			cmd+=sdvbsfreqinfoList[i].sdownfreq;
			cmd+="&symbol=";
			cmd+=sdvbsfreqinfoList[i].ssymbol;
			string strRet;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			std::string msg = "频道扫描:";
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			//
			if(strRet.size() <= 0)
			{
				cout<<"ChanScanThd-DVBS命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
			}
			else
			{
				string strtem = SkipHTTPHead(strRet);
				if (strtem == "")
				{
					cout<<"ChanScanThd-DVBS没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				}
				else
				{
					XmlParser psr;
					psr.Set_xml(strRet);
					pXMLNODE node=psr.GetNodeFromPath("html/body");
					pXMLNODELIST nodeList = psr.GetNodeList(node);
					int count = nodeList->Size();
					pXMLNODE scanNode = NULL;
					bool epgfreqchange = true;
					pXMLNODE epgNode = NULL;
					string Freq, OrgNetID, TsID, QAM, SymbolRate,nitname,Nitid,pronum;
					for(int k=0;k<count;k++)
					{
						pXMLNODE childNode = psr.GetNextNode(nodeList);
						string name = psr.GetNodeName(childNode);
						if(name == "TSDesc")
						{
							psr.GetAttrNode(childNode,"stmid",TsID);
							psr.GetAttrNode(childNode,"orgnitid",OrgNetID);
							psr.GetAttrNode(childNode,"nitid",Nitid);
							psr.GetAttrNode(childNode,"nitname",nitname);
							psr.GetAttrNode(childNode,"pronum",pronum);
							cout<< TimeUtil::GetCurDateTime()<<"扫描到节目:扫描频点"<<sdvbsfreqinfoList[i].slocfreq<<"-"<<sdvbsfreqinfoList[i].sdownfreq<<":"<<nitname<<"............."<<endl;
							if((pronum.size()==0)&&(nitname.find("pronum=") != -1))
							{
								pronum = nitname.substr(7,nitname.size());
							}
							//*****
							if(StrUtil::Str2Int(pronum) > 0)
							{
								if(StrUtil::Str2Int(pronum) != count-1)
								{
									cout<<"个数解析有问题....."<<sdvbsfreqinfoList[i].slocfreq<<"-"<<sdvbsfreqinfoList[i].sdownfreq<<endl;
									cout<<strtem<<endl;
								}
								scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");    //创建ChannelScan节点
								//pXPRet.SetAttrNode(string("LocFreq"),StrUtil::Long2Str(locfreq),scanNode);
								pXPRet.SetAttrNode(string("Freq"),sdvbsfreqinfoList[i].sdownfreq,scanNode);
								pXPRet.SetAttrNode(string("SymbolRate"),sdvbsfreqinfoList[i].ssymbol,scanNode);
								pXPRet.SetAttrNode(string("QAM"),string("3"),scanNode);
								pXPRet.SetAttrNode(string("OrgNetID"),OrgNetID,scanNode);
								pXPRet.SetAttrNode(string("TsID"),TsID,scanNode);
							}
							//*****
						}
						else if(name == "ProDesc")
						{
							string program = "",ProgramID = "",ServiceID = "",VideoPID = "",AudioPID = "",ca = "",pmt = "",pcr = "",cur = "",provider = "";
							psr.GetAttrNode(childNode,"sid",ServiceID);
							ProgramID = ServiceID;
							psr.GetAttrNode(childNode,"vid",VideoPID);
							int vidpid =  StrUtil::Str2Int(VideoPID);
							vidpid = vidpid&0x1fff;
							VideoPID = StrUtil::Int2Str(vidpid);
							psr.GetAttrNode(childNode,"aud0",AudioPID);
							int audpid = StrUtil::Str2Int(AudioPID);
							audpid = audpid&0x1fff;
							AudioPID = StrUtil::Int2Str(audpid);
							psr.GetAttrNode(childNode,"ca",ca);
							psr.GetAttrNode(childNode,"name",program);
							if(program.size()==0)
							{
								psr.GetAttrNode(childNode,"provider",provider);
								if(provider.find("name=") != -1)
								{
									program = provider.substr(5,provider.size());
								}
							}
							//改变XML特殊字符""-%22,<>-%3C%3E修改为()%28%29 &-%26修改为A-%41 '-%27修改为`-%60
							int t33indx = 0;
							for(int timce = 0;timce<100;timce++)
							{
								if(program.find("%22")!=-1 || program.find("%3C")!=-1 || program.find("%3E")!=-1
									|| program.find("%3c")!=-1 || program.find("%3e")!=-1 || program.find("%26")!=-1
									|| program.find("%27")!=-1)
								{
									int pt22 = program.find("%22");
									if(pt22!=-1)
									{
										if(t33indx%2==0)
										{
											program.replace(pt22,3,"%28");
										}
										else
										{
											program.replace(pt22,3,"%29");
										}
										t33indx++;
									}
									int pt3c = program.find("%3C");
									if(pt3c!=-1)
									{
										program.replace(pt3c,3,"%28");
									}
									pt3c = program.find("%3c");
									if(pt3c!=-1)
									{
										program.replace(pt3c,3,"%28");
									}
									int pt3e = program.find("%3E");
									if(pt3e!=-1)
									{
										program.replace(pt3e,3,"%29");
									}
									pt3e = program.find("%3e");
									if(pt3e!=-1)
									{
										program.replace(pt3e,3,"%29");
									}
									int pt26 = program.find("%26");
									if(pt26!=-1)
									{
										program.replace(pt26,3,"%41");
									}
									int pt27 = program.find("%27");
									if(pt27!=-1)
									{
										program.replace(pt27,3,"%60");
									}
								}
								else
								{
									break;
								}
							}
							//-----字符转化-----
							char* src = (char*)program.data();
							char *dest = new char[300];
							memset(dest,0,sizeof(char)*300);
							bw_encodestr(dest,src);
							program = dest;
							delete []dest;
							//----------------
							string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
							if(program.find(str) != -1)
							{
								program = program.substr(str.size(),program.size());
							}
							if (program == "")
							{
								program = "unknown";
								program += StrUtil::Int2Str(iUnknownCount);
								iUnknownCount++;
							}
							psr.GetAttrNode(childNode,"pmt",pmt);
							psr.GetAttrNode(childNode,"pcr",pcr);
							//*****
							if(scanNode)
							{
								pXMLNODE channelNode = pXPRet.CreateNodePtr(scanNode,"Channel");  //创建Channel节点
								pXPRet.SetAttrNode(string("ProgramID"),ServiceID,channelNode);
								pXPRet.SetAttrNode(string("Program"),program,channelNode);
								pXPRet.SetAttrNode(string("ServiceType"),string("1"), channelNode);					
								if (program.find("高清") != string::npos || program.find("HD") != string::npos)
								{
									pXPRet.SetAttrNode(string("HDTV"),string("1"),channelNode);
								}
								else
								{
									pXPRet.SetAttrNode(string("HDTV"),string("0"),channelNode);
								}
								pXPRet.SetAttrNode(string("PmtPid"),pmt,channelNode); 
								pXPRet.SetAttrNode(string("PcrPid"),pcr,channelNode);		 
								pXPRet.SetAttrNode(string("ServiceID"),ServiceID,channelNode);  
								pXPRet.SetAttrNode(string("VideoPID"),VideoPID,channelNode);
								pXPRet.SetAttrNode(string("AudioPID"),AudioPID,channelNode);
								int ica = StrUtil::Str2Int(ca);
								if((ica%2)==1)//只能区分清流和加扰流,加扰已授权和未授权不能区分,手动配置
								{
									pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
									pXPRet.SetAttrNode(string("Encryption"), string("1"), channelNode);
								}
								else
								{
									pXPRet.SetAttrNode(string("Encrypt"),string("1"), channelNode);
									pXPRet.SetAttrNode(string("Encryption"), string("0"), channelNode);
								}
								pXMLNODE StreamNode = pXPRet.CreateNodePtr(channelNode,"elementryPID");
								pXPRet.SetAttrNode("StreamType",string("2"),StreamNode);
								pXPRet.SetAttrNode("PID",VideoPID,StreamNode);
								pXMLNODE StreamNode1 = pXPRet.CreateNodePtr(channelNode,"elementryPID");
								pXPRet.SetAttrNode("StreamType",string("4"),StreamNode1);
								pXPRet.SetAttrNode("PID",AudioPID,StreamNode1);
								//******************************************EPG***************************
								if(bepg == 1||bepg == 2)
								{
									cmd = "";
									strRet = "";
									cmd = "POST /device/epgsearch  HTTP/1.1  \r\n\r\n";
									cmd+="chl=";
									cmd+=tdev.s_cardindex;
									cmd+="_";
									cmd+=tdev.s_chanindex;
									cmd+="&polar=0&tone=0&localfreq=";
									cmd+=sdvbsfreqinfoList[i].slocfreq;
									cmd+="&downfreq=";
									cmd+=sdvbsfreqinfoList[i].sdownfreq;
									cmd+="&symbol=";
									cmd+=sdvbsfreqinfoList[i].ssymbol;
									cmd+="&orgnitid=";
									cmd+=OrgNetID;
									cmd+="&nitid=";
									cmd+=Nitid;
									cmd+="&stmid=";
									cmd+=TsID;
									cmd+="&sid=";
									cmd+=ServiceID;
									pDeviceAccess->SendTaskMsg(cmd,strRet);
									//
									if(strRet.size() <= 0)
									{
										cout<<"ChanScanThread-DVBS-EPG命令失败:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
										continue;
									}
									string strtem = SkipHTTPHead(strRet);
									if (strtem == "")
									{
										cout<<"ChanScanThread-DVBS-EPG没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
										continue;
									}
									XmlParser epgpsr;
									epgpsr.Set_xml(strtem);
									pXMLNODE epgnode=epgpsr.GetNodeFromPath("html/body");
									pXMLNODELIST epgnodeList = epgpsr.GetNodeList(epgnode);
									int epgcount = epgnodeList->Size();
									for(int k=0;k<epgcount;k++)
									{
										pXMLNODE childNode = epgpsr.GetNextNode(epgnodeList);
										string evname = epgpsr.GetNodeName(childNode);
										if(evname == "Even")
										{
											if(epgfreqchange)
											{
												epgfreqchange = false;
												epgNode = pXPEpg.CreateNodePtr(ChEpgreportNode,"EPGInfo");    //创建ChannelScan节点
												pXPEpg.SetAttrNode(string("Freq"),sdvbsfreqinfoList[i].sdownfreq,epgNode);
											}
											string id = "",type = "",status = "",free = "",start = "",dura = "",name = "";
											epgpsr.GetAttrNode(childNode,"id",id);
											epgpsr.GetAttrNode(childNode,"typ",type);
											epgpsr.GetAttrNode(childNode,"sta",status);
											epgpsr.GetAttrNode(childNode,"fre",free);
											epgpsr.GetAttrNode(childNode,"cur",cur);
											epgpsr.GetAttrNode(childNode,"str",start);
											epgpsr.GetAttrNode(childNode,"dur",dura);
											epgpsr.GetAttrNode(childNode,"nm",name);
											if(epgNode)
											{
												string str = "";//特殊字符,不是空格,除掉节目名称前的无用特殊字符
												if(name.find(str) != -1)
												{
													name = name.substr(str.size(),name.size());
												}
												pXMLNODE epgchNode = pXPEpg.CreateNodePtr(epgNode,"EPG");
												pXPEpg.SetAttrNode(string("ProgramID"),ServiceID,epgchNode);
												pXPEpg.SetAttrNode(string("Program"),name,epgchNode);
												pXPEpg.SetAttrNode(string("ProgramType"),type,epgchNode);
												//
												string strymd = start.substr(2,4);
												char temp = 't';
												char *str123 = &temp; 
												long lmdj = (long)strtol(strymd.c_str(), &str123, 16);//
												int y = int(float(lmdj-15078.2)/365.25);
												int m = int(float(lmdj-14956.1-int(y*365.25))/30.6001);
												int d = lmdj-14956-int(y*365.25)-int(m*30.6001);
												int k=0;
												if(m==14||m==15)
												{
													k=1;
												}
												y = y+k+1900;
												m=m-1-k*12;
												string date = "";
												string time = "";
												char now[64];
												memset(now,0,64);
												sprintf(now, "%04d-%02d-%02d", y, m, d);
												date = now;
												time = start.substr(6,2);
												int iHour = StrUtil::Str2Int(time)+8;
												if (iHour >= 24)
												{
													iHour -= 24;
												}
												if (iHour >= 0)
												{
													char buf[3] = {0};
													sprintf(buf, "%02d", iHour);
													time = buf;
												}
												time+=":";
												time+=start.substr(8,2);
												time+=":";
												time+=start.substr(10,2);
												start = date+" "+time;
												//
												if(dura.size() == 4)
												{
													time="00:00:";
													time+=dura.substr(2,2);
												}
												else if(dura.size() == 3)
												{
													time="00:00:0";
													time+=dura.substr(2,1);
												}
												else if(dura.size() == 5)
												{
													time="00:0";
													time+=dura.substr(2,1);
													time+=":";
													time+=dura.substr(3,2);
												}
												else if(dura.size() == 6)
												{
													time="00:";
													time+=dura.substr(2,2);
													time+=":";
													time+=dura.substr(4,2);
												}
												else if(dura.size() == 8)
												{
													time = dura.substr(2,2);
													time+=":";
													time+=dura.substr(4,2);
													time+=":";
													time+=dura.substr(6,2);
												}
												else if(dura.size() == 7)
												{
													time = "0";
													time += dura.substr(2,1);
													time+=":";
													time+=dura.substr(3,2);
													time+=":";
													time+=dura.substr(5,2);
												}
												else
												{
													cout<<"dura:"<<dura<<endl;
												}
												dura = time;
												//
												pXPEpg.SetAttrNode(string("StartTime"),start,epgchNode);  //设置StartTime
												pXPEpg.SetAttrNode(string("ProgramLen"),dura,epgchNode);  //设置ProgramLen属性值
												pXPEpg.SetAttrNode(string("State"),status,epgchNode);         //设置State属性值
												pXPEpg.SetAttrNode(string("Encryption"),free,epgchNode);       //设置Encryption属性值
											}
										}
									}
								}	
								//******************************************EPG***************************
							}
						}
					}
				}
			}
		}
		//
		delete pDeviceAccess;
		pXPRet.SaveAsFile(filename.c_str());
		pXPEpg.SaveAsFile(epgfilename.c_str());
		cout<< TimeUtil::GetCurDateTime()<<"结束-DVBS-频道扫描............."<<endl;
		//
	}
	return true;
}
bool ChanScanThread::ChanScanForFM(std::string sDevID)
{
	string filename = string("c:/table/fm/ChannelScanQuery.xml");

	char* strXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	int idevID = StrUtil::Str2Int(sDevID);
	int iUnknownCount = 0;
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID, RADIO, tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0, RADIO, idevID, tunermonitorip, tunermonitorport);//tuner
    //客户监测频点配置字符串处理
    vector<string> Vfreq,VChanScanFreq;
    DoStringFreq(PROPMANAGER::instance()->GetFreqString(),Vfreq);/*各配置频点放入vector*/

	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		XmlParser pXPRet(strXML);
		pXMLNODE rootNode = pXPRet.GetRootNode();
		pXPRet.SetAttrNode(string("DVBType"),string("RADIO"),rootNode);
		pXMLNODE  returnNode = pXPRet.CreateNodePtr(rootNode,"Return");   //创建Return节点
		pXPRet.SetAttrNode(string("Type"),string("ChannelScanQuery"),returnNode);
		pXPRet.SetAttrNode(string("Value"),string("0"),returnNode);
		pXPRet.SetAttrNode(string("Desc"),string("成功"),returnNode);
		pXPRet.SetAttrNode(string("Comment"),string(""),returnNode);
		pXPRet.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),returnNode);
		pXMLNODE reportNode = pXPRet.CreateNodePtr(rootNode,"ChannelScanReport");
        pXMLNODE FreqScanNode = NULL;
		//*****
		cout<< TimeUtil::GetCurDateTime()<<"开始-FM-频道扫描............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		int lastlevel = 0;
		long curfreq=87000;
		long endFreq=curfreq+7000;
		for(long curfreq=86900; curfreq <= 108000;)
		{
			curfreq = curfreq + 100;
			cout<< TimeUtil::GetCurDateTime()<<"扫描频点：开始频点：--["<<curfreq<<"]............."<<endl;
			string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&startfre=";
			cmd+=StrUtil::Long2Str(curfreq);
			cmd+="&endfre=";
			cmd+=StrUtil::Long2Str(endFreq);
			//cmd+="108000";
			cmd+="&step=";
			cmd+="100";//步长
			string strFmRet;
			pDeviceAccess->SendTaskMsg(cmd,strFmRet); 
			std::string msg = "频道扫描:";
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_BOTH);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strFmRet,LOG_OUTPUT_BOTH);
			Sleep(1000);
			string strtem = SkipHTTPHead(strFmRet);
			if (strtem == "")
			{
				cout<<"ChanScanThd-FM没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				//
				curfreq = curfreq + 7000;
				continue;
				//return false;
			}
            XmlParser psr;
			psr.Set_xml(strtem);
			pXMLNODE node=psr.GetNodeFromPath("html/body");
			pXMLNODELIST nodeList = psr.GetNodeList(node);
			int count = nodeList->Size();
			pXMLNODE scanNode = NULL;
			bool epgfreqchange = true;
			pXMLNODE epgNode = NULL;
			string strStr, strModulation, strSnr, strAudLevel, strAudFreq,strCarrierSta;
			string strstation;
			for(int k=0;k<count;k++)
			{
				pXMLNODE childNode = psr.GetNextNode(nodeList);
				string name = psr.GetNodeName(childNode);
				if(name == "RFInfo")
				{
					psr.GetAttrNode(childNode,"str",strStr);
					psr.GetAttrNode(childNode,"modulation",strModulation);
					psr.GetAttrNode(childNode,"snr",strSnr);
					psr.GetAttrNode(childNode,"audLevel",strAudLevel);
					psr.GetAttrNode(childNode,"audFreq",strAudFreq);
					psr.GetAttrNode(childNode,"carrierSta",strCarrierSta);
					//
					if((StrUtil::Str2Int(strStr) > 20)&&(StrUtil::Str2Int(strStr) > lastlevel+3))
					{
						scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");    //创建ChannelScan节点
						pXPRet.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),scanNode);
						pXPRet.SetAttrNode(string("SymbolRate"),string(""),scanNode);
						pXPRet.SetAttrNode(string("QAM"),string(""),scanNode);
						pXPRet.SetAttrNode(string("OrgNetID"),string(""),scanNode);
						pXPRet.SetAttrNode(string("TsID"),string(""),scanNode);
					}
					lastlevel = StrUtil::Str2Int(strStr);
					//
				}
				else if(name == "err")
				{
					string er;
					psr.GetAttrNode(childNode,"code",er);
					if(er != "0")
					{
						cout<<"FM chanscan err code:"<<er<<endl;
						break;
					}
				}
				else if(name == "ProDesc")
				{
					psr.GetAttrNode(childNode,"station",strstation);
                    FreqScanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");
                    VChanScanFreq.push_back(strstation);
                    pXPRet.SetAttrNode("Freq",strstation,FreqScanNode);
				}
			}
			//
			endFreq = endFreq + 7000;
			curfreq = curfreq + 7000;
		}
        //开始匹配是否丢频
        if(Vfreq.size()>0)
        {
            for(vector<string>::iterator pIter = Vfreq.begin();pIter!=Vfreq.end();++pIter)
            {
                vector <string>:: iterator VIter;
                VIter = find(VChanScanFreq.begin(),VChanScanFreq.end(),*pIter);
                if( VIter != VChanScanFreq.end())
                {
                    cout<<"<--- 无丢频 --->"<<endl;
                }
                else
                {
                    //没匹配到
                    cout<<"<--- 频点add --->"<<*pIter<<endl;
                    FreqScanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");
                    pXPRet.SetAttrNode("Freq",*pIter,FreqScanNode);
                }
            }
        }
		delete pDeviceAccess;
		pXPRet.SaveAsFile(filename.c_str());
		cout<< TimeUtil::GetCurDateTime()<<"结束-FM-频道扫描............."<<endl;
	}
	return true;
}
bool ChanScanThread::ChanScanForAM(std::string sDevID)
{
	string filename = string("c:/table/am/ChannelScanQuery.xml");

	char* strXML = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	int idevID = StrUtil::Str2Int(sDevID);
	int iUnknownCount = 0;
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(idevID, AM, tdev);
	string tunermonitorip = "",tunermonitorport = "";
	PROPMANAGER::instance()->GetDevMonitorInfo(0, AM, idevID, tunermonitorip, tunermonitorport);//tuner
	if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
	{
		//*****
		XmlParser pXPRet(strXML);
		pXMLNODE rootNode = pXPRet.GetRootNode();
		pXPRet.SetAttrNode(string("DVBType"),string("AM"),rootNode);
		pXMLNODE  returnNode = pXPRet.CreateNodePtr(rootNode,"Return");   //创建Return节点
		pXPRet.SetAttrNode(string("Type"),string("ChannelScanQuery"),returnNode);
		pXPRet.SetAttrNode(string("Value"),string("0"),returnNode);
		pXPRet.SetAttrNode(string("Desc"),string("成功"),returnNode);
		pXPRet.SetAttrNode(string("Comment"),string(""),returnNode);
		pXPRet.SetAttrNode(string("ScanTime"),TimeUtil::GetCurDateTime(),returnNode);
		pXMLNODE reportNode = pXPRet.CreateNodePtr(rootNode,"ChannelScanReport");
		//*****
		cout<< TimeUtil::GetCurDateTime()<<"开始-AM-频道扫描............."<<endl;
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
		int lastlevel = 0;
		long curfreq = 531;
		long endFreq = curfreq + 357;
		for(long curfreq = 522; curfreq <= 1602;)
		{
			curfreq = curfreq + 9;
			cout<< TimeUtil::GetCurDateTime()<<"扫描频点:开始频点--["<<curfreq<<"]............."<<endl;
			
			string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&startfre=";
			cmd+=StrUtil::Long2Str(curfreq);
			cmd+="&endfre=";
			cmd+=StrUtil::Long2Str(endFreq);
			//cmd+="1602";
			cmd+="&step=";
			cmd+="9";//步长
			string strAmRet;
			pDeviceAccess->SendTaskMsg(cmd,strAmRet); 
			std::string msg = "频道扫描:";
			msg += "发送命令:";
			msg += cmd;
			
			cout<<"*********[ "<<curfreq <<" ]----> "<<msg<<endl;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_BOTH);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strAmRet,LOG_OUTPUT_BOTH);
			Sleep(1000);
			string strtem = SkipHTTPHead(strAmRet);
			if (strtem == "")
			{
				cout<<"ChanScanThd-AM没有<html>标记:"<<tunermonitorip<<"--"<<tunermonitorport<<endl;
				//
				curfreq = curfreq + 357;
				continue;
				//return false;
			}
			XmlParser psr;
			psr.Set_xml(strtem);
			pXMLNODE node=psr.GetNodeFromPath("html/body");
			pXMLNODELIST nodeList = psr.GetNodeList(node);
			int count = nodeList->Size();
			pXMLNODE scanNode = NULL;
			bool epgfreqchange = true;
			pXMLNODE epgNode = NULL;
			string strStr, strModulation, strSnr, strAudLevel, strAudFreq,strCarrierSta,strstation;
			for(int k=0;k<count;k++)
			{
				pXMLNODE childNode = psr.GetNextNode(nodeList);
				string name = psr.GetNodeName(childNode);
				if(name == "RFInfo")
				{
					psr.GetAttrNode(childNode,"str",strStr);
					psr.GetAttrNode(childNode,"modulation",strModulation);
					psr.GetAttrNode(childNode,"snr",strSnr);
					psr.GetAttrNode(childNode,"audLevel",strAudLevel);
					psr.GetAttrNode(childNode,"audFreq",strAudFreq);
					psr.GetAttrNode(childNode,"carrierSta",strCarrierSta);
					//
					if((StrUtil::Str2Int(strStr) > 10)&&(StrUtil::Str2Int(strStr) > lastlevel+3))
					{
						scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");    //创建ChannelScan节点
						pXPRet.SetAttrNode(string("Freq"),StrUtil::Long2Str(curfreq),scanNode);
						pXPRet.SetAttrNode(string("SymbolRate"),string(""),scanNode);
						pXPRet.SetAttrNode(string("QAM"),string(""),scanNode);
						pXPRet.SetAttrNode(string("OrgNetID"),string(""),scanNode);
						pXPRet.SetAttrNode(string("TsID"),string(""),scanNode);
					}
					lastlevel = StrUtil::Str2Int(strStr);
					//
				}
				else if(name == "err")
				{
					string er;
					psr.GetAttrNode(childNode,"code",er);
					if(er != "0")
					{
						cout<<"AM chanscan err code:"<<er<<endl;
						break;
					}
				}
				else if(name == "ProDesc")
				{
					psr.GetAttrNode(childNode,"station",strstation);
					
                    scanNode = pXPRet.CreateNodePtr(reportNode,"ChannelScan");
                    pXPRet.SetAttrNode("Freq",strstation,scanNode);
				}
			}
			//
			endFreq = endFreq + 357;

			curfreq = curfreq + 357;
		}
		delete pDeviceAccess;
		pXPRet.SaveAsFile(filename.c_str());
		
		cout<< TimeUtil::GetCurDateTime()<<"结束-AM-频道扫描............."<<endl;
	}
	return true;
}
//设置 即时采集 频道表 EPG 
void ChanScanThread::SetChanScanNow(string NoticeXml,string NowScanFreq,string NowScanDevid)
{
	if(m_bChanScanWorkType)
	{
		if(NoticeXml.find("ScanNoticeReport")!=-1&&m_sNoticeXml.find("ScanNoticeReport")!=-1)
		{
			string Type,Type1;
			XmlParser parser,parser1;
			parser.Set_xml(m_sNoticeXml);
			pXMLNODE pNode = parser.GetNodeFromPath("Msg/ScanNoticeReport");
			parser.GetAttrNode(pNode,"Type",Type);   
			//
			parser1.Set_xml(NoticeXml);
			pNode = parser1.GetNodeFromPath("Msg/ScanNoticeReport");
			parser1.GetAttrNode(pNode,"Type",Type1);   
			//
			if(Type != Type1)
			{
				m_sNoticeXmlTwo = NoticeXml;
			}
		}
	}
	else
	{
		m_sNowScanDevid = NowScanDevid;
		m_sNoticeXml = NoticeXml;
		m_sNowScanFreq = NowScanFreq;
		m_bChanScanWorkType = true;
	}
	return ;
}

string ChanScanThread::SkipHTTPHead(string http)
{
	string temp = http;

	int pos = temp.find("<html>");
	if (pos >= 0)
	{
		temp = temp.substr(pos, temp.size()-pos);
	}
	else
	{
		return "";
	}
	return temp;
}

string ChanScanThread::CreatePreChanScanRetTwo(eDVBType dvbType)
{
	string retXml;
	XmlParser parser;
	if((m_sOpVersion == "3.0")&&(m_sNoticeXml == ""))
	{
		string roundurl;
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
		{
			roundurl = PROPMANAGER::instance()->GetAgentInfoSet("19");
		}
		else
		{
			PROPMANAGER::instance()->GetUpStreamRoundUrl(dvbType,roundurl);
		}
		cout<<"roundurl:"<<roundurl<<endl;
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser headParser(cheader);
		pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg
		headParser.SetAttrNode( "Version",string("3.0"),headRootNode );//版本
		string MsgID;
		DBMANAGER::instance()->GetMsgID(MsgID);
		DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(StrUtil::Str2Int(MsgID)+1));
		headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//消息id属性
		if (dvbType == DVBC)
		{
			headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//消息类型
		}
		else if (dvbType == CTTB)
		{
			headParser.SetAttrNode( "Type",string("DVBTHUp"),headRootNode );//消息类型
		}
		else
		{
			headParser.SetAttrNode( "Type",string("UnKnown"),headRootNode );//消息类型
		}
		headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//当前时间
		headParser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetSrcCodeByType("SI"),headRootNode );//本机标识，可通过接口获得
		headParser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(dvbType),headRootNode );//目标机器标识
		headParser.SetAttrNode( "DstURL",roundurl,headRootNode);//目标URL
		headParser.SetAttrNode( "ReplyID",string("-1"),headRootNode );//回复的消息id
		headParser.SaveToString( retXml );	
		//
		parser.Set_xml(retXml);
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(parser.GetRootNode(),"ScanNoticeReport");//为返回的xml创建ChannelScanReport节点
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("Type",string("2"),channelScanQueryNode);
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
		cout<<"CreatePreChanScanRetTwo 定时采集epg预告:"<<retXml<<endl;
	}
	else
	{
		parser.Set_xml(m_sNoticeXmlTwo);
		if(m_sOpVersion == "3.0")
		{
			string roundurl;
			pXMLNODE rootNode = parser.GetRootNode();
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				roundurl = PROPMANAGER::instance()->GetAgentInfoSet("19");
				parser.SetAttrNode("DstURL",roundurl,rootNode);
			}
			if(m_sNoticeXmlTwo.find("ScanNoticeReport") != -1)
			{
				pXMLNODE pNode = parser.GetNodeFromPath("Msg/ScanNoticeReport");
				std::string scantime = TimeUtil::GetCurDateTime();
				parser.SetAttrNode("NoticeTime",scantime,pNode);
			}
			else if(m_sNoticeXmlTwo.find("EpgScanNotice") != -1)
			{
				pXMLNODE pNode = parser.GetNodeFromPath("Msg/ReturnInfo/EpgScanNotice");
				std::string scantime = TimeUtil::GetCurDateTime();
				parser.SetAttrNode("NoticeTime",scantime,pNode);
			}
		}
		else
		{
			if(m_sNoticeXmlTwo.find("ChannelScanNotice") != -1)//可能是epg
			{
				pXMLNODE pNode = parser.GetNodeFromPath("Msg/ReturnInfo/ChannelScanNotice");
				std::string scantime = TimeUtil::GetCurDateTime();
				parser.SetAttrNode("NoticeTime",scantime,pNode);
			}
		}
	}
	parser.SaveToString(retXml);
	return retXml;
}

string ChanScanThread::CreatePreChanScanRet(eDVBType dvbType)
{
	string retXml;
	if (m_sNoticeXml == "")
	{
		string roundurl;
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
		{
			roundurl = PROPMANAGER::instance()->GetAgentInfoSet("19");
		}
		else
		{
			PROPMANAGER::instance()->GetUpStreamRoundUrl(DVBC,roundurl);
		}
		cout<<"roundurl:"<<roundurl<<endl;
		if(m_sOpVersion == "3.0")
		{
			char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
			XmlParser headParser(cheader);
			pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg
			headParser.SetAttrNode( "Version",string("3.0"),headRootNode );//版本
			string MsgID;
			DBMANAGER::instance()->GetMsgID(MsgID);
			DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(StrUtil::Str2Int(MsgID)+1));
			headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//消息id属性
			if (dvbType == DVBC)
			{
				headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//消息类型
			}
			else if (dvbType == CTTB)
			{
				headParser.SetAttrNode( "Type",string("DVBTHUp"),headRootNode );//消息类型
			}
			else
			{
				headParser.SetAttrNode( "Type",string("UnKnown"),headRootNode );//消息类型
			}
			headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//当前时间
			headParser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetSrcCodeByType("SI"),headRootNode );//本机标识，可通过接口获得
			headParser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(dvbType),headRootNode );//目标机器标识
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
			cout<<"CreatePreChanScanRet 定时采集频道预告:"<<retXml<<endl;
		}
		else
		{
			char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";
			XmlParser headParser(cheader);
			pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//根节点msg
			headParser.SetAttrNode( "Version",string("2.6"),headRootNode );//版本
			string MsgID;
			DBMANAGER::instance()->GetMsgID(MsgID);
			DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(StrUtil::Str2Int(MsgID)+1));
			headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//消息id属性
			if (dvbType == DVBC)
			{
				headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//消息类型
			}
			else if (dvbType == CTTB)
			{
				headParser.SetAttrNode( "Type",string("DVBTHUp"),headRootNode );//消息类型
			}
			else
			{
				headParser.SetAttrNode( "Type",string("UnKnown"),headRootNode );//消息类型
			}
			headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//当前时间
			headParser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetSrcCodeByType("SI"),headRootNode );//本机标识，可通过接口获得
			headParser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(dvbType),headRootNode );//目标机器标识
			headParser.SetAttrNode( "DstURL",roundurl,headRootNode);//目标URL
			headParser.SetAttrNode( "ReplyID",string("-1"),headRootNode );//回复的消息id
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
			cout<<"send CreatePreChanScanRet:"<<retXml<<endl;
		}
	}
	else
	{
		if (dvbType == DVBC)
		{
			XmlParser parser;
			parser.Set_xml(m_sNoticeXml);
			if(m_sOpVersion == "3.0")
			{
				string roundurl;
				pXMLNODE rootNode = parser.GetRootNode();
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
				{
					roundurl = PROPMANAGER::instance()->GetAgentInfoSet("19");
					parser.SetAttrNode("DstURL",roundurl,rootNode);
				}
				if(m_sNoticeXml.find("ScanNoticeReport") != -1)
				{
					pXMLNODE pNode = parser.GetNodeFromPath("Msg/ScanNoticeReport");
					std::string scantime = TimeUtil::GetCurDateTime();
					parser.SetAttrNode("NoticeTime",scantime,pNode);
				}
				else if(m_sNoticeXml.find("EpgScanNotice") != -1)
				{
					pXMLNODE pNode = parser.GetNodeFromPath("Msg/ReturnInfo/EpgScanNotice");
					std::string scantime = TimeUtil::GetCurDateTime();
					parser.SetAttrNode("NoticeTime",scantime,pNode);
				}
			}
			else
			{
				if(m_sNoticeXml.find("ChannelScanNotice") != -1)//可能是epg
				{
					pXMLNODE pNode = parser.GetNodeFromPath("Msg/ReturnInfo/ChannelScanNotice");
					std::string scantime = TimeUtil::GetCurDateTime();
					parser.SetAttrNode("NoticeTime",scantime,pNode);
				}
			}
			parser.SaveToString(retXml);
		}
	}

	return retXml;
}

bool ChanScanThread::GetDVBTypeAndCardIndex(string sDevList, map<string, string>& dvbCardIndex)
{
	string str = sDevList;
	string str1 = "";
	int ipos1 = -1;
	int ipos2 = -1;
	while(1)
	{
		ipos1 = -1;
		ipos1 = str.find_first_of(";");
		str1 = "";
		ipos2 = -1;
		if (ipos1 > 0)
		{
			str1 = str.substr(0, ipos1);
			ipos2 = str1.find(":");
			if (ipos2 > 0)
			{
				string sDvbType = str1.substr(0, ipos2);
				string sChannel = str1.substr(ipos2+1, str1.length());
				dvbCardIndex.insert(make_pair(sDvbType, sChannel));
			}
			str = str.substr(ipos1+1, str.length());
		}
		else
		{
			if (str.length() > 0)
			{
				str1 = str;
				ipos2 = str1.find(":");
				if (ipos2 > 0)
				{
					string sDvbType = str1.substr(0, ipos2);
					string sChannel = str1.substr(ipos2+1, str1.length());
					dvbCardIndex.insert(make_pair(sDvbType, sChannel));
				}
			}

			break;
		}
	}
	
	return true;
}
bool ChanScanThread::ValidEpgDur(string strEpgDur)
{
	bool ret = true;
	char tem[20];
	memset(tem,0,sizeof(char)*20);
	memcpy(tem,strEpgDur.c_str(),strEpgDur.size());
	for(int i=0;i<strEpgDur.size()&&i<20;i++)
	{
		if(!((tem[i]>='0'&&tem[i]<='9')||(tem[i]==':')))
		{
			ret = false;
			break;
		}
	}
	return ret;
}
//节目名称中有空格的XML, 去掉空格
bool ChanScanThread::ValidProXml(string& strXml)
{
	if (strXml.length() <= 0)
		return false;
	string strtem = strXml;
	XmlParser psr;
	psr.Set_xml(strtem);
	pXMLNODE node=psr.GetNodeFromPath("html/body");
	pXMLNODELIST nodeList = psr.GetNodeList(node);
	int count = nodeList->Size();
	pXMLNODE scanNode = NULL;
	pXMLNODE childNode = psr.GetFirstNode(nodeList);
	string name = psr.GetNodeName(childNode);
	string pronum, nitname;
	bool bRet = false;
	map<int, string> mapStr;
	if (name == "TSDesc")
	{
		psr.GetAttrNode(childNode,"nitname",nitname);
		psr.GetAttrNode(childNode,"pronum",pronum);
		if((pronum.size()==0)&&(nitname.find("pronum=") != -1))
		{
			pronum = nitname.substr(7,nitname.size());
		}
		if(StrUtil::Str2Int(pronum) > 0)
		{
			if(StrUtil::Str2Int(pronum) != count-1)
			{
				string str = strXml;
				int ipos = -1;
				int i = 0;
				ipos = str.find("/>");
				string strTsDesc = str.substr(0, ipos+2);
				mapStr.insert(make_pair(i, strTsDesc));
				i = 1;
				str = str.substr(ipos+2, str.length());
				ipos = str.find("</ProDesc>");
				while(ipos > 0)
				{
					string str1 = str.substr(0, ipos+10);
					int ipos1 = str1.find("name=");
					string str2 = str1.substr(0, ipos1+5);
					string str3 = str1.substr(ipos1+5, str1.length());
					str1 = str2;//1
					int ipos2 = str3.find_first_of(">");
					str2 = str3.substr(0, ipos2);//2
					str3 = str3.substr(ipos2, str3.length());//3
					ipos2 = -1;
					ipos2 = str2.find(" ");
					while(ipos2 >= 0)
					{
						if (ipos2 == 0)
						{
							str2 = str2.substr(1, str2.length());
						}
						else
						{
							string str4 = str2.substr(0, ipos2);
							str2 = str2.substr(ipos2+1, str2.length());
							str4 += str2;
							str2 = str4;
						}
						ipos2 = -1;
						ipos2 = str2.find(" ");
					}

					str1 += str2;
					str1 += str3;
					mapStr.insert(make_pair(i, str1));
					i++;
					str = str.substr(ipos+10, str.length());
					ipos = -1;
					ipos = str.find("</ProDesc>");
				}

				string strRet = "";
				for(int k = 0; k < mapStr.size(); k++)
				{
					map<int, string>::iterator iter = mapStr.find(k);
					if (iter != mapStr.end())
					{
						strRet += iter->second;
					}
				}
				strRet += str;
				strXml = strRet;
			}
			bRet = true;
		}

	}


	return bRet;
}
//检测高清节目
bool ChanScanThread::CheckHDProgramForDVBC(string sRetXMl, string& sHDTV)
{
	string strVfmtXml = SkipHTTPHead(sRetXMl);
	if (strVfmtXml == "")
	{
		return false;
	}
	XmlParser psr;
	psr.Set_xml(strVfmtXml);
	pXMLNODE node=psr.GetNodeFromPath("html/body");
	pXMLNODELIST nodeList = psr.GetNodeList(node);
	pXMLNODE childNode = psr.GetFirstNode(nodeList);
	string sid, vfmt, width, height;
	psr.GetAttrNode(childNode, "sid", sid);
	psr.GetAttrNode(childNode, "vfmt", vfmt);
	if (sid == "" || vfmt == "")
		return false;
	int ipos = vfmt.find("_");
	if (ipos != string::npos)
	{
		width = vfmt.substr(0, ipos);
		height = vfmt.substr(ipos+1, vfmt.length());
	}
	if (StrUtil::Str2Int(width) >= 1280 && StrUtil::Str2Int(height) >= 720)
	{//HD 720p=1280*720; 1080p=1920*1080;
		sHDTV = "1";
	}
	else
	{
		sHDTV = "0";
	}
	return true;
}
bool  ChanScanThread::GetHDInfo(string freq,string servid,std::vector<HDInfoForScan>& HDInfoForScanList,string& hdi)
{
	bool ret = false;
	if(HDInfoForScanList.size()>0)
	{
		for(int i=0;i<HDInfoForScanList.size();i++)
		{
			if((freq == HDInfoForScanList[i].sFreq)&&(HDInfoForScanList[i].sServiceID == servid))
			{
				hdi = HDInfoForScanList[i].sHD;
				ret = true;
				break;
			}
		}

	}
	return ret;
}
bool ChanScanThread::GetDvbsFreqInfo(string freq,sdvbsfreqinfo& dvbsfreq)
{
	bool ret = false;
	if(sdvbsfreqinfoList.size()>0)
	{
		for(int i=0;i<sdvbsfreqinfoList.size();i++)
		{
			if(freq == sdvbsfreqinfoList[i].sdownfreq)
			{
				dvbsfreq = sdvbsfreqinfoList[i];
				ret = true;
				break;
			}
		}

	}
	return ret;
}
void ChanScanThread::DoStringFreq(string &strstr,vector<string>& vStr)
{
    int npos = strstr.find(":");
    string strTemp = strstr.substr(++npos,strstr.length());
	if(strTemp!="")
	{
		int fi = strTemp.find(",",0);
		int pos = 0;
		while(1)
		{
			string strFreq = strTemp.substr(pos,fi-pos);
			vStr.push_back(strFreq);
			if(fi==-1)
			{
				break;
			}
			pos = fi;
			pos++;
			fi = strTemp.find(",", fi + 1);
		}
	}
}
   