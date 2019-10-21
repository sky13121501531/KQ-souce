
#include <iostream>
#include "PropManager.h"
#include "StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../DBAccess/DBManager.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
long PropManager::AlarmID=time(0);

PropManager::PropManager(void)
{
	std::vector<int> Idlist;
	string src = "123";
	string str;
	InitiateBaseInfo();
	InitiateDeviceInfo();
	//
}

PropManager::~PropManager(void)
{
	std::map<std::string, sEquInfo*>::iterator iter = m_mapEquInfo.begin();
	for (; iter!=m_mapEquInfo.end(); iter++)
	{
		delete iter->second;
		iter->second = NULL;
	}

	m_mapEquInfo.clear();


	if (mConfig != NULL)
	{
		delete mConfig;
		mConfig = NULL;
	}
}

void PropManager::InitiateBaseInfo()
{
	mConfig=new PropConfig("c:/vscttb/properties.xml");
	if (!mConfig->isInitiated())
	{
		OSFunction::ExitProcess("系统初始化(配置文件)失败");
	}
	
	//数据库
	mDbType=mConfig->GetNodeAttribute("db","dbsource","type");
	if (mDbType.length()==0)
		mDbType="";

	mDbIp=mConfig->GetNodeAttribute("db","dbsource","localip");
	if (mDbIp.length()==0)
		mDbIp="127.0.0.1";

	mBandDbIp=mConfig->GetNodeAttribute("db","dbsource","banddevip");
	if (mBandDbIp.length()==0)
		mBandDbIp="";

	mDbPort=mConfig->GetNodeAttribute("db","dbsource","port");
	if (mDbPort.length()==0)
		mDbPort="";

	mDbUsername=mConfig->GetNodeAttribute("db","dbsource","username");
	if (mDbUsername.length()==0)
		mDbUsername="";

	mDbPwd=mConfig->GetNodeAttribute("db","dbsource","pwd");

	mDbName=mConfig->GetNodeAttribute("db","dbsource","dbname");
	if (mDbName.length()==0)
		mDbName="vsdvb";

	//CAS SMS信息
	CASIPAddr=mConfig->GetNodeAttribute("CASSMS","CAS","IP");
	if (CASIPAddr.length()==0)
		CASIPAddr="127.0.0.1";
	CASRequestURL=mConfig->GetNodeAttribute("CASSMS","CAS","path");
	if (CASRequestURL.length()==0)
		CASRequestURL="/";
	SMSRequestURL=mConfig->GetNodeAttribute("CASSMS","SMS","URL");
	if(SMSRequestURL.length()==0)
		SMSRequestURL="";
	SMSIPAddr=mConfig->GetNodeAttribute("CASSMS","SMS","IP");
	if (SMSIPAddr.length()==0)
		SMSIPAddr="127.0.0.1";
	CASPort=StrUtil::Str2Int(mConfig->GetNodeAttribute("CASSMS","CAS","port"));
	SMSPort=StrUtil::Str2Int(mConfig->GetNodeAttribute("CASSMS","SMS","port"));
	ShareDir=mConfig->GetNodeAttribute("CASSMS","share","directory");
	if(ShareDir.length()==0)
		ShareDir="c:/vscttb/cassms";
	
	HttpPath=mConfig->GetNodeAttribute("CASSMS","share","httppath");
	if(HttpPath.length()==0)
		HttpPath="/cassms/";
	
	mVirtualDevInfo.insert(make_pair(DVBC, 1003));
}


bool PropManager::InitInfoFromDB()
{
	string centerfreq,symbolrate,qam;
	sSysConfigParam SysConfig;
	vector<sDvbConfigParam> VecConfig;  
	DBMANAGER::instance()->QuerySystemConfig(UNKNOWN,SysConfig);//查询系统配置

	//服务器配置 IP，端口
	mHttpVideoIP = SysConfig.VideoHttpServerIp;
	if(mHttpVideoIP.length() == 0)
	mHttpVideoIP = "127.0.0.1";

	mHttpVideoPort = SysConfig.VideoHttpPort;
	if(mHttpVideoPort.length() == 0)
		mHttpVideoPort = "5050";
	
	mVideoMaxnum = SysConfig.VideoHttpMaxnum;
	if(mVideoMaxnum.length() == 0)
		mVideoMaxnum = "10";

	mRtspVideoIP = SysConfig.VideoRtspServerIp;
	if(mRtspVideoIP.length() == 0)
		mRtspVideoIP = "127.0.0.1";

	mRtspVideoPort = SysConfig.VideoRtspPort;
	if(mRtspVideoPort.length() == 0)
		mRtspVideoPort = "5555";

	mQualityIp = SysConfig.QualityServerIp;
	if(mQualityIp.length() == 0)
		mQualityIp = "127.0.0.1";
	
	mQualityPort = SysConfig.QualityPort;
	if (mQualityPort.length() == 0)
		mQualityPort = "8050";
	
	mQualityMaxnum = SysConfig.QualityMaxnum;
	if (mQualityMaxnum.length() == 0)
		mQualityMaxnum = "10";

	mRecordQualityIp = SysConfig.RecordQualityIp;
	if (mRecordQualityIp.length() ==0)
		mRecordQualityIp = "127.0.0.1";


	mRecordQualityPort = SysConfig.RecordQualityPort;
	if (mRecordQualityPort.length() ==0)
		mRecordQualityPort = "8070";

	mRecordQualityMaxnum = SysConfig.RecordQualityMaxnum;
	if (mRecordQualityMaxnum.length()==0)
		mRecordQualityMaxnum="10";

	mDeviceIp = SysConfig.DeviceServerIp;
	if (mDeviceIp.length() == 0)
		mDeviceIp = "172.16.10.250";

	mDevicePort = SysConfig.DevicePort;
	if (mDevicePort.length() == 0)
		mDevicePort = "5060";

	mDeviceMaxnum = SysConfig.DeviceMaxnum;
	if (mDeviceMaxnum.length() == 0)
		mDeviceMaxnum = "10";

	mXmlServerIP = SysConfig.XmlServerIp;
	if (mXmlServerIP.length() == 0)
		mXmlServerIP = "127.0.0.1";

	mXmlServerPort = SysConfig.XmlPort;
	if (mXmlServerPort.length() == 0)
		mXmlServerPort = "5061";


	mLogIp = SysConfig.LogServerIp;
	if (mLogIp.length() == 0)
		mLogIp = "172.16.10.250";

	mLogPort = SysConfig.LogPort;
	if (mLogPort.length() == 0)
		mLogPort = "9000";

	mLogMaxnum = SysConfig.LogMaxnum;
	if (mLogMaxnum.length() == 0)
		mLogMaxnum = "10";

	mHttpServerIP = SysConfig.HttpServerIp;
	if(mHttpServerIP.length() == 0)
		mHttpServerIP = "127.0.0.1";

	mHttpServerport = SysConfig.HttpPort;
	if(mHttpServerport.length() == 0)
		mHttpServerport = "81";

	mFtpServerIP = SysConfig.FtpServerIp;
	if(mFtpServerIP.length() == 0)
		mFtpServerIP = "127.0.0.1";

	mFtpServerPort = SysConfig.FtpPort;
	if(mFtpServerPort.length() == 0)
		mFtpServerPort = "21";

	//日志信息
	mLogPath = SysConfig.Log_Path;
  	if (mLogPath.length() == 0)
		mLogPath = "c:/VSCTTB/logs/";

	mLogExpire = SysConfig.Log_Expire;
	if (mLogExpire.length() == 0)
		mLogExpire = "7";

	mLogType = SysConfig.Log_Type;
	if (mLogType.length() == 0)
		mLogType = "debug";

	mLogOutputFile = SysConfig.Log_outputToFile;
	if (mLogOutputFile.length() == 0)
		mLogOutputFile="";

	mLogAnalyser = SysConfig.LogPathType_Analyser;
	if (mLogAnalyser.length() == 0)
		mLogAnalyser = "analyzer";

	mLogVideo  = SysConfig.LogPathType_Video;
	if (mLogVideo.length() == 0)
		mLogVideo = "video";

	mLogRecord = SysConfig.LogPathType_Record;
	if (mLogRecord.length() == 0)
		mLogRecord = "record";

	mLogDevice = SysConfig.LogPathType_Device;
	if (mLogDevice.length() == 0)
		mLogDevice = "device";

	mLogOther = SysConfig.LogPathType_Other;
	if (mLogOther.length() == 0)
		mLogOther = "other";

	mLogDefault = SysConfig.LogPathType_Default;
	if (mLogDefault.length() == 0)
		mLogDefault = "default";

	//录像存储路径
	string temploclist,tempsharelist,devicelist;
	temploclist = SysConfig.Record_FileLocation;
	if (temploclist.length() == 0)
		temploclist = "d:/tsrecord/";

	tempsharelist = SysConfig.Record_FileSharePath;
	if (tempsharelist.length() == 0)
		tempsharelist = "/dtsrecord/";

	SepLocShareAndInit(temploclist, tempsharelist, mRecLocToShare, mRecFileLocVec);

	devicelist = SysConfig.Record_DeviceId;
	if(devicelist.length() == 0)
		devicelist = "1-10";

	SeparateRecordDir(devicelist);

	mRecordPeriod = SysConfig.Record_Period;
	if(mRecordPeriod.length() == 0)
		mRecordPeriod = "5";

	mMaxAvailableSize = SysConfig.Record_MaxAvailableSize;
	if (mMaxAvailableSize.length() == 0)
		mMaxAvailableSize = "2";

	mDBCleanInterval = SysConfig.Record_DBCleanInterval;
	if (mDBCleanInterval.length() == 0)
		mDBCleanInterval = "30";

	mSystemCleanTime = SysConfig.Record_SystemCleanTime;
	if(mSystemCleanTime.length() == 0)
		mSystemCleanTime = "00:00:00";

	mStoreType = SysConfig.Record_StoreType;
	if (mStoreType.length()<1)
		mStoreType = "0";


	xmlSendTimes = StrUtil::Str2Long(SysConfig.XmlSendTime);
	if(xmlSendTimes == 0)
		xmlSendTimes = 3;
	xmlOvertime = StrUtil::Str2Long(SysConfig.XmlOverTime);
	if(xmlOvertime == 0)
		xmlOvertime = 3;


	mTempFileLocation = SysConfig.TempFileLocation;
	if (mTempFileLocation.length() == 0)
		mTempFileLocation = "c:/vscttb/temp/";

	mTempSharePath	  = SysConfig.TempSharePath;
	if (mTempSharePath.length() == 0)
		mTempSharePath = "/vstemp/";

	mTempFileExpire   = SysConfig.TempFileExpireTime;
	if (mTempFileExpire.length() == 0)
		mTempFileExpire = "1";

	mRecLocToShare.insert(make_pair(mTempFileLocation,mTempSharePath));

	DeviceManagetype = SysConfig.DeviceSchedulerTask_Type;
	if(DeviceManagetype.length() == 0)
		DeviceManagetype = "week";

	DeviceManageweekday = SysConfig.DeviceSchedulerTask_WeekDay;
	if(DeviceManageweekday.length() == 0)
		DeviceManageweekday = "2";

	DeviceManagesingleday = SysConfig.DeviceSchedulerTask_Date;
	if(DeviceManagesingleday.length() == 0)
		DeviceManagesingleday ="";

	DeviceManagetime = SysConfig.DeviceSchedulerTask_Time;
	if(DeviceManagetime.length() == 0)
		DeviceManagetime = "00:00:00";

	PsisiManagetype = SysConfig.PsisiSchedulerTask_Type;
	if(PsisiManagetype.length() == 0)
		PsisiManagetype = "week";

	PsisiManageweekday = SysConfig.PsisiSchedulerTask_WeekDay;
	if(PsisiManageweekday.length() == 0)
		PsisiManageweekday = "2";
	PsisiManagesingleday = SysConfig.PsisiSchedulerTask_Date;
	if(PsisiManagesingleday.length() == 0)
		PsisiManagesingleday ="";

	PsisiManagetime = SysConfig.PsisiSchedulerTask_Time;
	if(PsisiManagetime.length() == 0)
		PsisiManagetime = "00:00:00";

	
	DBMANAGER::instance()->QueryDvbConfig(DVBC,VecConfig);
	for(int i=0;i<VecConfig.size();i++)
	{
		sFreqScanInfo freqscaninfo;

		freqscaninfo.CenterFreq = VecConfig[i].CenterFreq;
		freqscaninfo.QAM = VecConfig[i].Qam;
		freqscaninfo.SymbolRate = VecConfig[i].Symbolrate;

		mFreqScanInfoMap.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),freqscaninfo));

		mScanFile.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].ChannelScanType));

		mRealTimeFromRecord.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].RealTimeFromRecord));
		OSDInfo osdinfo;

		osdinfo.FontSize = VecConfig[i].Osd_fontsize;
		osdinfo.Info = VecConfig[i].Osd_infoosd;
		osdinfo.InfoX = VecConfig[i].Osd_infoosdx;
		osdinfo.InfoY = VecConfig[i].Osd_infoosdy;
		osdinfo.TimeType = VecConfig[i].Osd_timeosdtype;
		osdinfo.TimeX = VecConfig[i].Osd_timeosdx;
		osdinfo.TimeY = VecConfig[i].Osd_timeosdy;
		osdinfo.ProgramX = VecConfig[i].Osd_programx;
		osdinfo.ProgramY = VecConfig[i].Osd_programy;
		mOSDInfoMap.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),osdinfo));

		mXmlTablePath.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].TablePath));
		mTableSharePath.insert( make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].SharePath) );

		VideoParam  HDRecordparam;
		VideoParam  SDRecordparam;
		VideoParam  HDRealStreamparam;
		VideoParam  SDRealStreamparam;

		HDRecordparam.width = VecConfig[i].HDRecordWidth;
		HDRecordparam.height = VecConfig[i].HDRecordHeight;
		HDRecordparam.bps   =  VecConfig[i].HDRecordBps;

		HDRealStreamparam.width = VecConfig[i].HDRealStreamWidth;
		HDRealStreamparam.height = VecConfig[i].HDRealStreamHeight;
		HDRealStreamparam.bps    = VecConfig[i].HDRealStreamBps;

		SDRecordparam.width  = VecConfig[i].SDRecordWidth;
		SDRecordparam.height = VecConfig[i].SDRecordHeight;
		SDRecordparam.bps    = VecConfig[i].SDRecordBps;

		SDRealStreamparam.width = VecConfig[i].SDRealStreamWidth;
		SDRealStreamparam.height = VecConfig[i].SDRealStreamHeight;
		SDRealStreamparam.bps    = VecConfig[i].SDRealStreamBps;

		mHDRecordVideoPram.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),HDRecordparam));
		mSDRecordVideoPram.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),SDRecordparam));
		mHDRealStreamVideoPram.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),HDRealStreamparam));
		mSDRealStreamVideoPram.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),SDRealStreamparam));
		
		mAudioBps.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].AudioBps));
		
		UpAlarmInfo alarminfo;
	
		alarminfo.type = VecConfig[i].AlarmType;
		alarminfo.interval = VecConfig[i].AlarmInterval;
		alarminfo.onceday = VecConfig[i].OnceDay;
		alarminfo.alarmurl = VecConfig[i].AlarmServer;
		alarminfo.runplanrecord = VecConfig[i].RunplanRecord;
		mUpAlarmInfo.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),alarminfo));

		UpStreamRoundInfo streamroundinfo;				//FRee_fanrong_20140103
		streamroundinfo.streamroundurl = VecConfig[i].StreamRoundUrl;
		mUpStreamRoundInfo.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),streamroundinfo));

		mLogPath_VIDEO = VecConfig[i].LogPath_VIDEO;	//FRee_fanrong_20140107
		mLogPath_SI = VecConfig[i].LogPath_SI;			//FRee_fanrong_20140107
		mLogPath_SMS = VecConfig[i].LogPath_SMS;		//FRee_fanrong_20140107
		mLogPath_CAS = VecConfig[i].LogPath_CAS;		//FRee_fanrong_20140107

		//cas sms si文件存储路径
		SmsDir = VecConfig[i].SmsDir;
		if(SmsDir.length()==0)
			SmsDir="c:/vscttb/cassms/";

		CasDir = VecConfig[i].CasDir;
		if(CasDir.length()==0)
			CasDir="c:/vscttb/cassms/";

		SIDir = VecConfig[i].SIDir;
		if(SIDir.length()==0)
			SIDir="c:/table/dvbc/";

		sVideoProcotol VideoProtocol;
		
		VideoProtocol.streamprotocol = VecConfig[i].VideoStreamProtocol;
		VideoProtocol.fileprotocol = VecConfig[i].VideoFileProtocol;
		VideoProtocol.fileurltype = VecConfig[i].VideoFileUrlType;
		VideoProtocol.fileoffset = VecConfig[i].VideoFileOffSet;
		mVideoProtocolMap.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type), VideoProtocol));				


		sRecordParamInfo tempRecParam;

	
		tempRecParam.recordexpire = VecConfig[i].RecordParam_recordexpire;
		tempRecParam.alarmrecordexpire = VecConfig[i].RecordParam_alarmrecordexpire;
		tempRecParam.alarmheadoffset = VecConfig[i].RecordParam_alarmheadoffset;
		tempRecParam.alarmtailoffset = VecConfig[i].RecordParam_alarmtailoffset;
		tempRecParam.alarmrecordmode = VecConfig[i].RecordParam_alarmrecordmode;

		mRecParamInfo.insert( make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),tempRecParam) );

		sRecordDownInfo recdowninfo;
		
		recdowninfo.protocol = VecConfig[i].RecordDown_protocol;
		recdowninfo.urltype = VecConfig[i].RecordDown_urltype;
		recdowninfo.offset = VecConfig[i].RecordDown_offset;

		mRecordDownMap.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),recdowninfo));


		mDefDstCode.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].GeneralDestCode));
		mDefSrcCode.insert(make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type),VecConfig[i].GeneralSrcCode));
	
		//获取异态录像的相关存储信息(开关、文件个数)
		sAlarmRecordStorageCfg AlarmRecCfgInfo;
		AlarmRecCfgInfo._switch = VecConfig[i].AlarmStorage_switch;
		AlarmRecCfgInfo._filenum = VecConfig[i].AlarmStorage_filenum;
		mAlarmRecStorageCfgMap.insert( make_pair(OSFunction::GetEnumDVBType(VecConfig[i].type), AlarmRecCfgInfo) );
	}

	return true;
}
bool PropManager::InitiateDeviceInfo()
{
	XmlParser mXmlProp;
	std::string nodePath;
	std::string nodeList[4] = {"device","center","task","scantype"}; 
	mXmlProp.LoadFromFile("c:/vscttb/properties.xml");

	m_nAllDeviceIndexCount = 0;
	m_nAllCardCount = 0;

	for (int j = 0;j<4;j++)
	{
		nodePath="properties/"+nodeList[j];
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if(node==NULL)
			return false;
		pXMLNODELIST nodeList=mXmlProp.GetNodeList(node);
		for (int i=0;i<nodeList->Size();i++)
		{
			pXMLNODE nodeNext=mXmlProp.GetNextNode(nodeList);
			switch ( j )
			{
			case 0:           //获取device节点相关信息
				{
					sDeviceInfo sDevInfo;
					mXmlProp.GetAttrNode(nodeNext,"index",sDevInfo.s_deviceindex);
					mXmlProp.GetAttrNode(nodeNext,"IP",sDevInfo.s_deviceip);//FRee_fanrong_20131212
					mXmlProp.GetAttrNode(nodeNext,"port",sDevInfo.s_deviceport);
					mMonitorDevInfo[StrUtil::Str2Int(sDevInfo.s_deviceport)] = sDevInfo;
					break;
				}
			case 1:          //获取监测中心相关信息 
				{
					string code,url,type;

					mXmlProp.GetAttrNode(nodeNext,string("type"),type);
					mXmlProp.GetAttrNode(nodeNext,string("url"),url);
					mXmlProp.GetAttrNode(nodeNext,string("srccode"),code);

					sEquInfo *pEquInfo = new sEquInfo;
					pEquInfo->strSrcCode = code;
					pEquInfo->strUrl = url;

					m_mapEquInfo.insert( make_pair(type,pEquInfo) );
					break;
				}
			case 2:           //获取task节点相关信息
				{
					string tasktype,devicelist,deviceshare;

					mXmlProp.GetAttrNode(nodeNext,"tasktype",tasktype);
					mXmlProp.GetAttrNode(nodeNext,"devicelist",devicelist);
					mXmlProp.GetAttrNode(nodeNext,"deviceshare",deviceshare);

					std::list<int> deviceidlist;
					SeparateStr(devicelist,deviceidlist);
					DeleteWrongDeviceID(deviceidlist);
					if (deviceidlist.empty() == false)
						mTaskInfo.insert(make_pair(tasktype,deviceidlist));
					if(deviceshare=="yes")
						mTaskShare.insert(make_pair(tasktype,true));
					else
						mTaskShare.insert(make_pair(tasktype,false));

					if(tasktype=="DvbLevelTask")
					{
						for (std::list<int>::iterator pList=deviceidlist.begin();pList!=deviceidlist.end();pList++)
						{
							int port=8888;
							QualityCard temp;
							enumDVBType type = UNKNOWN;
							GetDeviceCMDIP((*pList),temp.IP);
							GetDeviceCmdPort((*pList),port);
							GetDeviceType((*pList),type);
							temp.Port=StrUtil::Int2Str(port);
							mQualityCard.insert(make_pair(type,temp));
						}
						
					}
					break;
				}
			case 3:           
				{

					mXmlProp.GetAttrNode(nodeNext,"scan",chantypeinfo.s_scantype);
					mXmlProp.GetAttrNode(nodeNext,"freq",chantypeinfo.s_freq);
					mXmlProp.GetAttrNode(nodeNext,"weeklyswitch",chantypeinfo.s_weeklyswitch);
					break;
				}

			default:
				break;
			}
		}
	}

	if (RecordList.empty() == false)
	{
		mTaskInfo.insert(make_pair(string("AutoRecord"),RecordList));
		mTaskInfo.insert(make_pair(string("TaskRecord"),RecordList));
	}
	return true;
}
//**************************************************
bool PropManager::GetScanInfo(string& scantype,string& freq,string& weeklyswitch)
{
	freq = chantypeinfo.s_freq;
	scantype = chantypeinfo.s_scantype;
	weeklyswitch = chantypeinfo.s_weeklyswitch;
	return true;
}
bool PropManager::GetDeviceInfo()
{
	bool ret = true;
	int icodedeviceid = 0,itunerdeviceid = 0;
	std::map<int,sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	for (; iter!=mMonitorDevInfo.end(); iter++)
	{
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(iter->second.s_deviceindex),iter->second.s_deviceip,StrUtil::Str2Int(iter->second.s_deviceport));
		string strCMD = "GET /device/config HTTP/1.1 \r\n\r\n";//1.1设备配置查询
		//**************************************
		string cmd = "POST /device/prosearch  HTTP/1.1  \r\n\r\n";//1.3节目搜索
		cmd+="chl=5_0&fre=698000&sym=6875&qam=2";
		//**
		string cmd1 = "POST /io/info  HTTP/1.1  \r\n\r\n";
		cmd1+="chl=5_0";//2.1通道状态查询
		//**
		string cmd2 = "GET /device/info HTTP/1.1 \r\n\r\n";//1.2设备状态查询
		//**
		string cmd3 = "POST /tcoder/info  HTTP/1.1  \r\n\r\n";//3.1转码状态查询
		cmd3+="chl=0";
		//***************************************
		string strRet;
		pDeviceAccess->SendTaskMsg(strCMD,strRet);
		delete pDeviceAccess;
		if(strRet.size() <= 0)
		{
			cout<<"命令失败:"<<iter->second.s_deviceip<<"--"<<iter->second.s_deviceport<<endl;
			return false;
		}
		int pos = strRet.find("<html>");
		string strtem = strRet;
		if(pos >= 0)
		{
			strtem = strRet.substr(pos,strRet.size()-pos);
		}
		else
		{
			cout<<"没有<html>标记:"<<iter->second.s_deviceip<<"--"<<iter->second.s_deviceport<<endl;
			return false;
		}
		//
		string info,num;
		XmlParser psr;
		psr.Set_xml(strtem);
		pXMLNODE node=psr.GetNodeFromPath("html/body");
		pXMLNODELIST nodeList = psr.GetNodeList(node);
		int count = nodeList->Size();
		for(int k=0;k<count;k++)
		{
			pXMLNODE childNode = psr.GetNextNode(nodeList);
			string name = psr.GetNodeName(childNode);
			if(name == "IODesc")
			{
				psr.GetAttrNode(childNode,"desc",info);
				int pos = info.find("_");
				if(pos != -1)
				{
					string tunertype = info.substr(0,pos);
					if(tunertype == "DVBC")
					{
						string temp = info.substr(pos+1,info.size());
						pos = temp.find("_");
						if(pos != -1)
						{
							TunerDev tdev;
							tdev.s_monitorindex = iter->second.s_deviceindex;
							tdev.s_cardindex = temp.substr(0,pos);
							int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
							for(int i=0;i<cardnum;i++)
							{
								tdev.s_chanindex = StrUtil::Int2Str(i);
								tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid);
								tdev.b_user = false;
								tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerdeviceid);
								tdev.s_stsport = StrUtil::Int2Str(7000+itunerdeviceid);
								tdev.s_trport = StrUtil::Int2Str(8000+itunerdeviceid);
								mTunerDevVec.push_back(tdev);
								itunerdeviceid++;
							}
						}
					}
				}
			}
			else if(name == "TCoder")
			{
				psr.GetAttrNode(childNode,"num",num);
				CoderDev cdev;
				cdev.s_monitorindex = iter->second.s_deviceindex;
				for(int i=0;i<StrUtil::Str2Int(num);i++)
				{
					cdev.s_coderdeviceid = StrUtil::Int2Str(icodedeviceid);
					cdev.s_coderindex = StrUtil::Int2Str(i);
					cdev.b_user = false;
					cdev.s_dip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(100+icodedeviceid);
					cdev.s_dtsport = StrUtil::Int2Str(9000+icodedeviceid);
					cdev.s_code = "h264";
					cdev.s_ena = "1";
					cdev.s_fmt = "0";
					cdev.s_fre = "0";
					cdev.s_rate = "400";
					mCoderDevVec.push_back(cdev);
					icodedeviceid++;
				}
			}
			else if(name == "TSDesc")
			{
				string Freq, OrgNetID, TsID, QAM, SymbolRate,temp;
				psr.GetAttrNode(childNode,"stmid",TsID);
				psr.GetAttrNode(childNode,"orgnitid",OrgNetID);
				psr.GetAttrNode(childNode,"nitid",temp);
				psr.GetAttrNode(childNode,"nitname",temp);
				psr.GetAttrNode(childNode,"pronum",temp);
			}
			else if(name == "ProDesc")
			{
				//Program="CCTV高清综合" ProgramID="454" ServiceID="454" VideoPID="4540" AudioPID="4541" Encrypt="1" HDTV="1"  ServiceType=""
				string program,ProgramID,ServiceID,VideoPID,AudioPID,ca;
				psr.GetAttrNode(childNode,"sid",ServiceID);
				ProgramID = ServiceID;
				psr.GetAttrNode(childNode,"vid",VideoPID);
				psr.GetAttrNode(childNode,"aud0",AudioPID);
				psr.GetAttrNode(childNode,"ca",ca);
				psr.GetAttrNode(childNode,"name",program);
			}
		}
	}
	return ret;
}
//
bool PropManager::GetDevMonitorInfo(int devtype,int deviceid,string& monitorip,string& monitorport)
{
	bool ret = false;
	string monitorindex;
	if(devtype == 0)
	{
		std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
		for (; iter!=mTunerDevVec.end(); iter++)
		{
			if(deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))
			{
				monitorindex = iter->s_monitorindex;
			}
		}
	}
	else if(devtype == 1)
	{
		std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
		for (; iter!=mCoderDevVec.end(); iter++)
		{
			if(deviceid == StrUtil::Str2Int(iter->s_coderdeviceid))
			{
				monitorindex = iter->s_monitorindex;
			}
		}
	}
	if(monitorindex.size()>0)
	{
		ret = true;
		std::map<int,sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
		for (; iter!=mMonitorDevInfo.end(); iter++)
		{
			if(iter->second.s_deviceindex==monitorindex)
			{
				monitorip = iter->second.s_deviceip;
				monitorport = iter->second.s_deviceport;
				ret = true;
			}
		}
	}
	return ret;
}
//
bool PropManager::GetTunerInfo(int deviceid,TunerDev& TunerDevInfo)
{
	bool ret = false;
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))
		{
			TunerDevInfo = (*iter);
			ret = true;
		}
	}
	return ret;
}
bool PropManager::GetCoderInfo(int deviceid,CoderDev& CoderDevInfo)
{
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(deviceid == StrUtil::Str2Int(iter->s_coderdeviceid))
		{
			CoderDevInfo = (*iter);
			ret = true;
		}
	}
	return ret;
}
bool PropManager::GetDevID(int& devid,int priority,string freq,string taskid)
{
	bool ret = false;
	int tunid = -1,codid = -1;
	bool ISok = false;
	if(devid < 0)//自动分配通道
	{
		if(GetUnUseCoderDev(codid))
		{
			ISok = true;
		}
		else if(GetCoderDevFromPriority(codid,priority))
		{
			ISok = true;
		}
	}
	else
	{
		if(GetCoderDevFromPriorityDevID(devid,priority))
		{
			codid = devid;
			ISok = true;
		}
	}
	if(ISok)
	{
		if(GetTunerDevFromFreq(tunid,freq))
		{
			devid = codid;
			sDevLinkInfo DevLink;
			DevLink.Priority = StrUtil::Int2Str(priority);
			DevLink.s_coderdeviceid = StrUtil::Int2Str(codid);
			DevLink.s_tunerdeviceid = StrUtil::Int2Str(tunid);
			DevLink.Taskid = taskid;
			msDevLinkInfoVec.push_back(DevLink);
			ret = true;
		}
		else if(GetUnUseTunerDev(tunid,freq))
		{
			devid = codid;
			sDevLinkInfo DevLink;
			DevLink.Priority = StrUtil::Int2Str(priority);
			DevLink.s_coderdeviceid = StrUtil::Int2Str(codid);
			DevLink.s_tunerdeviceid = StrUtil::Int2Str(tunid);
			DevLink.Taskid = taskid;
			msDevLinkInfoVec.push_back(DevLink);
			ret = true;
		}
		else if(GetTunerDevFromPriority(tunid,priority,freq))
		{
			devid = codid;
			sDevLinkInfo DevLink;
			DevLink.Priority = StrUtil::Int2Str(priority);
			DevLink.s_coderdeviceid = StrUtil::Int2Str(codid);
			DevLink.s_tunerdeviceid = StrUtil::Int2Str(tunid);
			DevLink.Taskid = taskid;
			msDevLinkInfoVec.push_back(DevLink);
			ret = true;
		}
	}
	return ret;
}
bool PropManager::GetUnUseCoderDev(int& devid)
{
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if((!(iter->b_user))&&(StrUtil::Str2Int(iter->s_coderdeviceid) != 0))//0实时视频用
		{
			iter->b_user = true;
			devid = StrUtil::Str2Int(iter->s_coderdeviceid);
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::GetUnUseTunerDev(int& devid,string freq)
{
	bool ret = false;
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((!(iter->b_user))&&(StrUtil::Str2Int(iter->s_tunerdeviceid) != 0)/*&&(StrUtil::Str2Int(iter->s_tunerdeviceid) != 1)*/)//0实时视频用 1频道扫描用
		{
			iter->b_user = true;
			iter->s_freq = freq;
			devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::GetTunerDevFromFreq(int& devid,string freq)
{
	bool ret = false;
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(iter->s_freq == freq)
		{
			devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::GetTunerDevFromPriority(int& devid,int priority,string freq)//tuner存在共用要找单个资源的最大权限1.获取所有小于此权限资源 2.遍历每个资源获取每个资源的最大权限 3.替换最大权限是最小值的
{
	bool ret = false;
	int devpri[100];
	memset(devpri,-1,sizeof(int)*100);
	std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
	for (; DevLinkiter!=msDevLinkInfoVec.end(); DevLinkiter++)
	{
		int tid = StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid);
		int pri = StrUtil::Str2Int(DevLinkiter->Priority);
		if(pri > devpri[tid])
		{
			devpri[tid] = pri;
		}
	}
	int iminpri = priority;
	int did = -1;
	for(int i=0;i<100;i++)
	{
		if((devpri[i]>=0)&&(iminpri>devpri[i]))
		{
			iminpri = devpri[i];
			did = i;
			ret = true;
		}
	}
	devid = did;
	//
	if(ret)
	{
		std::vector<TunerDev>::iterator tuniter = mTunerDevVec.begin();
		for (; tuniter!=mTunerDevVec.end(); tuniter++)
		{
			if(StrUtil::Str2Int(tuniter->s_tunerdeviceid) == devid)
			{
				tuniter->s_freq = freq;
				break;
			}
		}
	}
	//
	return ret;
}
bool PropManager::GetTunerDevFromCoderDev(int& tunedevid,int codedevid)
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	for (; iter!=msDevLinkInfoVec.end(); iter++)
	{
		int cid = StrUtil::Str2Int(iter->s_coderdeviceid);
		if(cid == codedevid)
		{
			tunedevid = StrUtil::Str2Int(iter->s_tunerdeviceid);
			ret = true;
		}
	}
	return ret;
}
bool PropManager::GetCoderDevFromPriority(int& devid,int priority)//1.替换权限是最小值的且小于此权限资源
{
	bool ret = false;
	int iminpri = priority;
	int did = -1;
	string taskid;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	for (; iter!=msDevLinkInfoVec.end(); iter++)
	{
		int pri = StrUtil::Str2Int(iter->Priority);
		if(pri < iminpri)
		{
			did = StrUtil::Str2Int(iter->s_coderdeviceid);
			iminpri = pri;
			taskid = iter->Taskid;
			ret = true;
		}
	}
	if(ret)
	{
		FreeDevLink(taskid);
		devid = did;
	}
	return ret;
}
bool PropManager::GetCoderDevFromPriorityDevID(int devid,int priority)//指定通道任务判断
{
	bool ret = false;
	if(IsDeviceAvaiable(devid))
	{
		bool find = false;
		std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
		for (; iter!=msDevLinkInfoVec.end(); iter++)
		{
			if(devid == StrUtil::Str2Int(iter->s_coderdeviceid))
			{
				int pri = StrUtil::Str2Int(iter->Priority);
				if(priority > pri)
				{
					FreeDevLink(iter->Taskid);//任务替换删除原任务记录
					ret = true;
				}
				find = true;
			}
		}
		if(!find)
		{
			ret = true;
		}
	}
	return ret;
}
bool PropManager::FreeCoderDev(int devid)
{
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(StrUtil::Str2Int(iter->s_coderdeviceid) == devid)
		{
			iter->b_user = false;
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::FreeTunerDev(int devid)
{
	bool ret = false;
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(StrUtil::Str2Int(iter->s_tunerdeviceid) == devid)
		{
			iter->b_user = false;
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::FreeDevLink(string taskid)
{
	bool ret = false;
	std::vector<int> checktuneridlist;
	std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
	for (; DevLinkiter!=msDevLinkInfoVec.end();)
	{
		if(taskid == DevLinkiter->Taskid)
		{
			//
			FreeCoderDev(StrUtil::Str2Int(DevLinkiter->s_coderdeviceid));
			checktuneridlist.push_back(StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid));
			//
			DevLinkiter = msDevLinkInfoVec.erase(DevLinkiter);
			ret = true;
		}
		else
		{ 
			DevLinkiter++;
		}
	}
	//
	if(checktuneridlist.size() > 0)
	{
		std::vector<int>::iterator tuniter = checktuneridlist.begin();
		for (; tuniter!=checktuneridlist.end(); tuniter++)
		{
			if(!(TunerDevIsInDevLink(*tuniter)))
			{
				FreeTunerDev(*tuniter);
			}
		}
	}
	//
	return ret;
}
bool PropManager::TunerDevIsInDevLink(int tundevid)
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	for (; iter!=msDevLinkInfoVec.end();iter++)
	{
		if(tundevid == StrUtil::Str2Int(iter->s_tunerdeviceid))
		{
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::DevIsInList(int devid,vector<int>& devlist)
{
	bool ret = false;
	std::vector<int>::iterator iter = devlist.begin();
	for (; iter!=devlist.end(); iter++)
	{
		if(devid == (*iter))
		{
			ret = true;
			break;
		}
	}
	return ret;
}
//*********************************************************************
//日志
eLogType PropManager::GetLogType( void )
{
	if (mLogType=="debug")
		return LOG_EVENT_DEBUG;
	else if (mLogType=="warnning")
		return LOG_EVENT_WARNNING;
	else if (mLogType=="error")
		return LOG_EVENT_ERROR;
	else
		return LOG_EVENT_DEBUG;
}

eLogOutPut PropManager::GetOutputFile( void )
{
	if (mLogOutputFile=="screen")
		return LOG_OUTPUT_SCREEN;
	else if (mLogOutputFile=="file")
		return LOG_OUTPUT_FILE;
	else if (mLogOutputFile=="both")
		return LOG_OUTPUT_BOTH;
	else
		return LOG_OUTPUT_UNDEFINE;
}

std::string PropManager::GetPath(string path,std::vector<std::string>&velc)  
{
	string str;
	string temPath;
	int pos = 4;
	try			 //FRee_fanrong_20140110
	{
		if (path.at( path.length()-1 ) != '/')
		{
			path += "/";
		}
		str = path;
		while(pos != path.length()-1)
		{
			pos = path.find("/", ++pos);
			temPath = path.substr(0,pos);
			velc.push_back(temPath);
		}
	}
	catch (...)    //FRee_fanrong_20140110
	{
		cout << "传入参数为空,未读取到有效值"<<endl; 
	}
	return str;
}

void PropManager::Creatfolder(std::string path)
{
	std::vector<std::string> pathvelc;
	
	PROPMANAGER::instance()->GetPath(path,pathvelc);//处理多级目录
	
	for (int dirIndex=0; dirIndex<pathvelc.size(); dirIndex++)
	{
		if ( !CreateDirectory(pathvelc[dirIndex].c_str(), NULL)) 	
		{
			//std::cout<<"文件夹 ["<<vecVscttbDir[dirIndex]<<"] 已经存在,不用创建"<<std::endl;
		}
		else
			std::cout<<"文件夹 ["<<path<<"] 创建成功"<<std::endl;
	}
	return;
}


//硬件相关信息查询
bool PropManager::GetAllDeviceList( std::list<int>& devicedlist )
{
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		devicedlist.push_back((*pDeviceinfo).first);
	}
	return true;
}
bool PropManager::GetDVBDeviceList(eDVBType dvbtype,std::list<int>& devicedlist)
{
	string devicetype = OSFunction::GetStrDVBType(dvbtype);

	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if (devicetype == pDeviceinfo->second.devicetype)
		{
			devicedlist.push_back((*pDeviceinfo).first);
		}
	}
	return true;
}

bool PropManager::GetTaskDeviceList( string taskType,eDVBType dvbtype,std::list<int>& taskdeviceidlist )
{
	bool ret=true;
	string devicetype = OSFunction::GetStrDVBType(dvbtype);

	for (std::map<std::string,std::list<int> >::iterator pTaskinfo=mTaskInfo.begin();pTaskinfo!=mTaskInfo.end();pTaskinfo++)
	{
		if ((*pTaskinfo).first == taskType)
		{
			std::list<int>::iterator ptr = (*pTaskinfo).second.begin();
			if ((*pTaskinfo).second.size()>1)
			{
				for (;ptr!=(*pTaskinfo).second.end();ptr++)
				{
					std::map<int,sChanelInfo>::iterator in_ptr = mDeviceInfo.begin();
					for (;in_ptr!=mDeviceInfo.end();in_ptr++)
					{
						//if (( (*ptr) ==(*in_ptr).first )&&(devicetype ==(*in_ptr).second.devicetype))
						if (( (*ptr) ==(*in_ptr).first )&&(devicetype ==(*in_ptr).second.devicetype))   //通道号相同且devicetype相同
						{
							taskdeviceidlist.push_back(*ptr);
						}
					}
				}
			}
			else if ((*pTaskinfo).second.size() == 1)
			{
				std::map<int,sChanelInfo>::iterator in_ptr = mDeviceInfo.begin();
				for (;in_ptr!=mDeviceInfo.end();in_ptr++)
				{
					if (( (*ptr) ==(*in_ptr).first )&&(devicetype ==(*in_ptr).second.devicetype))
					{
						taskdeviceidlist.push_back(*ptr);
					}
				}

			}
		}
	}
	return ret;
}

bool PropManager::GetDeviceAddress( int deviceid,string& ip,int& cmdport,int &tsport,string &cmdprotocol,string &tsprotocol )
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			ip=(*pDeviceinfo).second.TSIP;
			cmdport=StrUtil::Str2Int((*pDeviceinfo).second.cmdport);
			tsport =StrUtil::Str2Int((*pDeviceinfo).second.tsport);
			cmdprotocol = (*pDeviceinfo).second.cmdprotocol;
			tsprotocol = (*pDeviceinfo).second.tsprotocol;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceCardIndex(int deviceid,int & iCardIndex)
{
	bool ret = false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret = true;
			iCardIndex = (*pDeviceinfo).second.CardIndex;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceTSIP(int deviceid,string& ip)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			ip=(*pDeviceinfo).second.TSIP;
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceCMDIP(int deviceid,string& ip)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			ip=(*pDeviceinfo).second.CMDIP;
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceCmdPort(int deviceid,int& cmdport)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			cmdport=StrUtil::Str2Int((*pDeviceinfo).second.cmdport);
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceCmdProtocol(int deviceid,std::string& cmdprotocol)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			cmdprotocol =(*pDeviceinfo).second.cmdprotocol;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceTsPort(int deviceid,unsigned int& tsport)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			tsport =StrUtil::Str2Int((*pDeviceinfo).second.tsport);
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceTunerID(int deviceid,int& tunerid)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			tunerid =StrUtil::Str2Int((*pDeviceinfo).second.tunerid);
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceLogIndex(int deviceid, int& logindex)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			logindex =StrUtil::Str2Int((*pDeviceinfo).second.logindex);
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceIndex(string & deviceid,string logindex,string devicetype)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).second.logindex==logindex &&(*pDeviceinfo).second.devicetype==devicetype)
		{
			ret=true;
			deviceid=StrUtil::Int2Str((*pDeviceinfo).first);
			break;
		}
	}
	return ret;
}
bool PropManager::GetDeviceTsProtocol(int deviceid,std::string& tsprotocol)
{
	bool ret=false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=true;
			tsprotocol = (*pDeviceinfo).second.tsprotocol;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceCardIndexbyIP(string ip,int & iCardIndex)	
{
	bool ret = false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if (ip == pDeviceinfo->second.CMDIP)
		{
			ret = true;
			iCardIndex =(*pDeviceinfo).second.CardIndex;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceIndexbyCardIndex(int iCardIndex,int &firstIndex)
{
	bool ret = false;
	firstIndex = -1;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if (iCardIndex == pDeviceinfo->second.CardIndex)
		{
			ret = true;
			firstIndex= (*pDeviceinfo).first;
			break;
		}
	}
	return ret;

}

bool PropManager::GetDeviceID(string ip,string tunerid,int & deviceid)
{
	bool ret = false;
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if (((*pDeviceinfo).second.TSIP==ip||(*pDeviceinfo).second.CMDIP==ip) && (*pDeviceinfo).second.tunerid == tunerid)
		{
			ret=true;
			deviceid=(*pDeviceinfo).first;
			break;
		}
	}
	return ret;

}

bool PropManager::GetDeviceIDByIP(string ip,std::list<int> & devicedlist)
{
	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if (ip == pDeviceinfo->second.TSIP||ip == pDeviceinfo->second.CMDIP)
		{
			devicedlist.push_back((*pDeviceinfo).first);
		}
	}
	return true;
}

bool PropManager::IsDeviceAvaiable( int deviceid,string tasktype,eDVBType dvbtype )
{
	bool ret=false;
	string devicetype = OSFunction::GetStrDVBType(dvbtype);

	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	{
		if ((*pDeviceinfo).first==deviceid)
		{
			ret=((*pDeviceinfo).second.devicetype==devicetype)&&(CheckDeviceInList(deviceid,mTaskInfo[tasktype]));
		}
	}
	return ret;
}
bool PropManager::IsDeviceAvaiable(int deviceid)
{ 
	//std::map<int,sChanelInfo>::iterator ptr = mDeviceInfo.begin();
	//for (;ptr!=mDeviceInfo.end();++ptr)
	//{
	//	if (deviceid == (*ptr).first)
	//	{
	//		return true;
	//	}
	//}
	//return false;
	//
	std::vector<CoderDev>::iterator ptr = mCoderDevVec.begin();
	for (;ptr!=mCoderDevVec.end();++ptr)
	{
		if (deviceid == StrUtil::Str2Int((*ptr).s_coderdeviceid))
		{
			return true;
		}
	}
	return false;
}
//根据通道号获得监测类型
bool PropManager::GetDeviceType(int deviceid,eDVBType& dvbtype)
{
	map<int,sChanelInfo>::iterator ptr = mDeviceInfo.begin();
	for (;ptr!= mDeviceInfo.end();ptr++)
	{
		if ((*ptr).first == deviceid)
		{ 
			dvbtype = OSFunction::GetEnumDVBType((*ptr).second.devicetype);
			break;
		}
	}
	return true;
}
//中心URL查询
std::string PropManager::GetUrl(const std::string& srcCode)
{
#ifdef ZONG_JU_ZHAO_BIAO
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.begin();

	for (; iter!=m_mapEquInfo.end(); iter++)
	{
		if (iter->second->strSrcCode == srcCode)
		{
			return iter->second->strUrl;
		}
	}

	return "";

#else
	std::map<std::string,sCenterInfo*>::iterator iter = centerInfoMap.find(srcCode);

	if (iter != centerInfoMap.end())
		return iter->second->centerUrl;
	else
		return "";
#endif
	
}

//其他函数
bool PropManager::CheckDeviceInList( int deviceid,std::list<int> deviceList )
{
	for (std::list<int>::iterator pList=deviceList.begin();pList!=deviceList.end();pList++)
	{
		if ((*pList)==deviceid)
		{
			return true;
		}
	}
	return false;
}

bool PropManager::DeleteWrongDeviceID(std::list<int>& deviceIDList)     //从任务的通道组中去掉不合理的deviceid
{
	std::list<int>::iterator ptr = deviceIDList.begin();
	std::map<int ,sChanelInfo>::iterator in_ptr;
	if (deviceIDList.size()>0)
	{  
		for (;ptr!=deviceIDList.end();)
		{ 
			in_ptr=mDeviceInfo.begin();
			for (;in_ptr!= mDeviceInfo.end();in_ptr++)
			{
				if ((*ptr) ==(*in_ptr).first)
				{
					break;
				}
			}
			if (in_ptr == mDeviceInfo.end())
			{
				ptr=deviceIDList.erase(ptr);
			}
			else
			{
				ptr++;
			}
		}
	}
	return true;
}

bool PropManager::SeparateStr( string src,list<int>& lis )//src要求：单独一个字符串没有分号；多个字符串之间以分号隔开且最后一个字符串后没有分号；
{
	char * context;
	char * id = strtok_s(const_cast<char*>(src.c_str()),";",&context);
	while(id!= NULL)
	{
		lis.push_back(atoi(id));
		id = strtok_s(NULL,";",&context);
	}
	return true;
}

bool PropManager::SeparateStrVec( string src,vector<int>& lis )//src要求：单独一个字符串没有分号；多个字符串之间以分号隔开且最后一个字符串后没有分号；
{
	char * context;
	char * id = strtok_s(const_cast<char*>(src.c_str()),";",&context);
	while(id!= NULL)
	{
		lis.push_back(atoi(id));
		id = strtok_s(NULL,";",&context);
	}
	return true;
}

bool PropManager::SeparateRecordDir( string src )//src要求：单独一个字符串没有分号；多个字符串之间以分号隔开且最后一个字符串后没有分号；
{

	vector<string> vec;
	char * context;
	char * id = strtok_s(const_cast<char*>(src.c_str()),";",&context);
	while(id!= NULL)
	{
		vec.push_back(id);
		id = strtok_s(NULL,";",&context);
	}

	for(int i=0;i<mRecFileLocVec.size();i++)
	{
		list<int> temp_list;
		char * str;
		char *head = strtok_s(const_cast<char*>(vec[i].c_str()),"-",&str);
		char *tail = strtok_s(NULL,"-",&str);
		for(int k=atoi(head);k<=atoi(tail);k++)
		{
			temp_list.push_back(k);
		}
		mDeviceIdToPath.insert( make_pair(mRecFileLocVec[i], temp_list) );
	}
	return true;
}


//add by jidushan 11.03.31
bool PropManager::SepLocShareAndInit(string srcloc, string srcshare, map<string ,string>& loctoshare,vector<string>& vecLoc)
{
	char* contextloc;
	char* contextshare;
	char* idloc = strtok_s(const_cast<char*>(srcloc.c_str()),";",&contextloc);
	char* idshare = strtok_s(const_cast<char*>(srcshare.c_str()),";",&contextshare);
	
	string loc,share;
	while (idloc != NULL && idshare != NULL)
	{
		loc = idloc;
		share = idshare;
		loctoshare.insert(make_pair(loc, share));
		vecLoc.push_back(loc);
		idloc = strtok_s(NULL, ";", &contextloc);
		idshare = strtok_s(NULL, ";", &contextshare);
	}

	return true;
}
bool PropManager::IsShareTask( std::string taskname )
{
	bool ret=false;

	std::map<std::string, bool>::iterator ptr=mTaskShare.find(taskname);
	if (ptr != mTaskShare.end())
	{
		ret = ptr->second;
	}
	return ret;
}

bool PropManager::SetPSISIInfo(eDVBType type, std::string text )
{
	if (type==CTTB)
	{
		mConfig->SetNodeText("psisi","cttb",text.c_str());
		mCTTBPsiSiValue=text;
	}
	else if(type==DVBC)
	{
		mConfig->SetNodeText("psisi","dvbc",text.c_str());
		mDVBCPsiSiValue=text;
	}
	else
		return false;
	return true;
}

bool PropManager::GetPSISIInfo( eDVBType type,std::string& valu )
{
	if(type==CTTB)
		valu=mCTTBPsiSiValue;
	else if(type==DVBC)
		valu=mDVBCPsiSiValue;
	else
		return false;
	return true;
}

bool PropManager::GetCASIP( std::string& CASIP )
{
	CASIP=CASIPAddr;
	return true;
}

bool PropManager::GetCASPort( int& port )
{
	port=CASPort;
	return true;
}

bool PropManager::GetSMSIP( std::string& SMSIP )
{
	SMSIP=SMSIPAddr;
	return true;
}

bool PropManager::GetSMSPort( int& port )
{
	port=SMSPort;
	return true;
}

bool PropManager::GetCASURL( std::string& path )
{
	path=CASRequestURL;
	return true;
}

bool PropManager::GetSMSURL( std::string& url )
{
	url=SMSRequestURL;
	return true;
}

bool PropManager::GetShareDir( std::string& dir )
{
	dir=ShareDir;
	return true;
}
bool PropManager::GetSmsDir( std::string& dir )
{
	dir=SmsDir;
	return true;
}

bool PropManager::GetCasDir( std::string& dir )
{
	dir=CasDir;
	return true;
}
bool PropManager::GetSIDir( std::string& dir )
{
	dir=SIDir;
	return true;
}
bool PropManager::GetHttpPath(std::string &path)
{
	path=HttpPath;
	return true;
}
bool PropManager::GetOSDInfo(eDVBType dvbtype,std::string hdtv,OSDInfo& info )
{
	info=mOSDInfoMap[dvbtype];
	OSFunction::SetOSDTimeMode(StrUtil::Str2Int(info.TimeType),info);

	if (hdtv == "1")
	{
		info.FontSize = StrUtil::Int2Str(24);
		info.InfoX = StrUtil::Int2Str(800);
		info.InfoY = StrUtil::Int2Str(480);
		info.ProgramX = StrUtil::Int2Str(20);
		info.ProgramY = StrUtil::Int2Str(20);

#ifdef ZONG_JU_ZHAO_BIAO
		info.TimeX = StrUtil::Int2Str(700);
#else
		info.TimeX = StrUtil::Int2Str(450);
#endif
		
		info.TimeY = StrUtil::Int2Str(20);

	}

	return true;
}

bool PropManager::SetOSDInfo(eDVBType dvbtype,OSDInfo info)
{
	OSDInfo  temposd;
	temposd=mOSDInfoMap[dvbtype];
	if(info.FontSize!="")
		temposd.FontSize=info.FontSize;
	if(info.InfoX!="")
		temposd.InfoX=info.InfoX;
	if(info.InfoY!="")
		temposd.InfoY=info.InfoY;
	if(info.ProgramX!="")
		temposd.ProgramX=info.ProgramX;
	if(info.ProgramY!="")
		temposd.ProgramY=info.ProgramY;
	if(info.TimeType!="")
		temposd.TimeType=info.TimeType;
	if(info.TimeX!="")
		temposd.TimeX=info.TimeX;
	if(info.TimeY!="")
		temposd.TimeY=info.TimeY;

	temposd.Info=info.Info;
	mOSDInfoMap[dvbtype]=temposd;
	string type;
	if(dvbtype==DVBC)
	{
		type="DVBC";
		if (info.TimeType == "0" || info.TimeType == "1" || info.TimeType == "3")
		{
			mOSDInfoMap[dvbtype].TimeType = "6";
		}
		else if (info.TimeType == "2")
		{
			mOSDInfoMap[dvbtype].TimeType = "8";
		}
	}
	else if(dvbtype==CTTB)
		type="CTTB";
	else if(dvbtype==ATV)
		type="ATV";
	else if(dvbtype==CTV)
		type="CTV";
	else if(dvbtype==DVBS)
	{
		type="DVBS";
		if (info.TimeType == "0" || info.TimeType == "1" || info.TimeType == "3")
		{
			mOSDInfoMap[dvbtype].TimeType = "6";
		}
		else if (info.TimeType == "2")
		{
			mOSDInfoMap[dvbtype].TimeType = "8";
		}
	}
	if(dvbtype==THREED)
	{
		type="THREED";
		if (info.TimeType == "0" || info.TimeType == "1" || info.TimeType == "3")
		{
			mOSDInfoMap[dvbtype].TimeType = "6";
		}
		else if (info.TimeType == "2")
		{
			mOSDInfoMap[dvbtype].TimeType = "8";
		}
	}

	mConfig->SetNodeAttribute(type,"osd","infoosd",mOSDInfoMap[dvbtype].Info.c_str());
	mConfig->SetNodeAttribute(type,"osd","infoosdx",mOSDInfoMap[dvbtype].InfoX.c_str());
	mConfig->SetNodeAttribute(type,"osd","infoosdy",mOSDInfoMap[dvbtype].InfoY.c_str());
	mConfig->SetNodeAttribute(type,"osd","timeosdtype",mOSDInfoMap[dvbtype].TimeType.c_str());
	mConfig->SetNodeAttribute(type,"osd","timeosdx",mOSDInfoMap[dvbtype].TimeX.c_str());
	mConfig->SetNodeAttribute(type,"osd","timeosdy",mOSDInfoMap[dvbtype].TimeY.c_str());
	return true;
}

bool PropManager::GetAlarmID( long& alarmid )
{
	alarmid=++AlarmID;
	return true;
}

//wz_0217
bool PropManager::GetVirDeviceId(eDVBType type, int& deviceid)
{
	std::map<eDVBType, int>::iterator iter = mVirtualDevInfo.begin();
	for (;iter != mVirtualDevInfo.end();++iter)
	{
		if (type == iter->first)
		{
			deviceid = iter->second;
			return true;
		}
	}

	return false;
}
//wz_0217
//wz_0217
bool PropManager::GetVirDevList( std::list<int>& devicelist )
{
	std::map<eDVBType,int>::iterator iter = mVirtualDevInfo.begin();
	for (;iter != mVirtualDevInfo.end();++iter)
	{
		devicelist.push_back(iter->second);
	}
	return true;
}
//wz_0217
//wz_0217
//deviceid为虚拟通道，返回true，为实际通道，返回false
bool PropManager::GetVirDevType(int deviceid, eDVBType& type)
{
	std::map<eDVBType,int>::iterator iter = mVirtualDevInfo.begin();
	for (;iter != mVirtualDevInfo.end();++iter)
	{
		if (deviceid == iter->second)
		{
			type = iter->first;
			return true;
		}
	}
	return false;
}
//wz_0217
//wz_0217
bool PropManager::IsRoundChannel(int deviceid)
{
	std::map<eDVBType,int>::iterator iter = mVirtualDevInfo.begin();
	for (;iter != mVirtualDevInfo.end();++iter)
	{
		if (deviceid == iter->second)
		{
			return true;
		}
	}
	return false;
}
//wz_0217

bool PropManager::GetQualityCardInfo(eDVBType dvbtype,std::string &ip,int &port)
{
	std::map<eDVBType,QualityCard>::iterator iter=mQualityCard.begin();
	for(;iter != mQualityCard.end();iter++)
	{
		if(dvbtype == iter->first)
		{
			ip=iter->second.IP;
			port=StrUtil::Str2Int(iter->second.Port);
			return true;
		}
	}
	return false;
}

//wz_110309
bool PropManager::GetStreamProtocol(eDVBType dvbtype,std::string& Protocol)
{
	//map中可能没有dvbtype
	if (mVideoProtocolMap.find(dvbtype) == mVideoProtocolMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]流传送协议类型失败!\n",strDvbType.c_str()));
		return false;
	}
	Protocol = mVideoProtocolMap[dvbtype].streamprotocol;
	return true;
}

//wz_110309
bool PropManager::GetFileProtocol(eDVBType dvbtype,std::string& Protocol)
{
	//map中可能没有dvbtype
	if (mVideoProtocolMap.find(dvbtype) == mVideoProtocolMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]文件传送协议类型失败!\n",strDvbType.c_str()));
		return false;
	}
	Protocol = mVideoProtocolMap[dvbtype].fileprotocol;
	return true;
}

//wz_110309
bool PropManager::GetFileUrlType(eDVBType dvbtype,std::string& urltype)
{
	//map中可能没有dvbtype
	if (mVideoProtocolMap.find(dvbtype) == mVideoProtocolMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]文件urltype失败!\n",strDvbType.c_str()));
		return false;
	}
	urltype = mVideoProtocolMap[dvbtype].fileurltype;
	return true;
}

//wz_110309
bool PropManager::GetFileOffset(eDVBType dvbtype,std::string& offset)
{
	//map中可能没有dvbtype
	if (mVideoProtocolMap.find(dvbtype) == mVideoProtocolMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s] : offset失败!\n",strDvbType.c_str()));
		return false;
	}
	offset = mVideoProtocolMap[dvbtype].fileoffset;
	return true;
}

//wz_110309
bool PropManager::GetRecDownProtocol(eDVBType dvbtype,std::string& Protocol)
{
	//map中可能没有dvbtype
	if (mRecordDownMap.find(dvbtype) == mRecordDownMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]文件下载协议类型失败!\n",strDvbType.c_str()));
		return false;
	}
	Protocol = mRecordDownMap[dvbtype].protocol;
	return true;
}

//wz_110309
bool PropManager::GetRecDownUrlType(eDVBType dvbtype,std::string& urltype)
{
	//map中可能没有dvbtype
	if (mRecordDownMap.find(dvbtype) == mRecordDownMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]文件下载urltype失败!\n",strDvbType.c_str()));
		return false;
	}
	urltype = mRecordDownMap[dvbtype].urltype;
	return true;
}

//wz_110309
bool PropManager::GetRecDownOffset(eDVBType dvbtype,std::string& offset)		
{
	//map中可能没有dvbtype
	if (mRecordDownMap.find(dvbtype) == mRecordDownMap.end())
	{
		string strDvbType = OSFunction::GetStrDVBType(dvbtype);
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 获取[%s]文件下载offset失败!\n",strDvbType.c_str()));
		return false;
	}
	offset = mRecordDownMap[dvbtype].offset;
	return true;
}

bool PropManager::SetUpAlarmInfo(eDVBType dvbtype,UpAlarmInfo alarminfo)
{
	mUpAlarmInfo[dvbtype] = alarminfo;

// 	string type;
// 	if(dvbtype==DVBC)
// 		type="DVBC";
// 	else if(dvbtype==CTTB)
// 		type="CTTB";
// 	else if(dvbtype==ATV)
// 		type="ATV";
// 	else if(dvbtype==CTV)
// 		type="CTV";
// 	else if(dvbtype==DVBS)
// 		type="DVBS";
// 	else if(dvbtype==THREED)
// 		type="THREED";
// 
// 	mConfig->SetNodeAttribute(type,"alarm","type",alarminfo.type.c_str());
// 	mConfig->SetNodeAttribute(type,"alarm","interval",alarminfo.interval.c_str());
// 	mConfig->SetNodeAttribute(type,"alarm","alarmserver",alarminfo.alarmurl.c_str());
	
	return true;
}

bool PropManager::GetUpAlarmInfo (eDVBType dvbtype,UpAlarmInfo& alarminfo)
{
	alarminfo=mUpAlarmInfo[dvbtype];
	return true;
}

bool PropManager::GetUpStreamRoundUrl(eDVBType dvbtype,std::string& strUpUrl)
{
	strUpUrl=mUpStreamRoundInfo[dvbtype].streamroundurl;
	return true;
}


std::string PropManager::GetTableSharePath(eDVBType dvbtype, bool ret)
{
	if (mTableSharePath.find(dvbtype) == mTableSharePath.end())
	{
		ret = false;
		return "";
	}
	ret = true;
	return mTableSharePath[dvbtype];
}

bool PropManager::SetTempFileExpire(std::string expire)
{
	mTempFileExpire = expire;
	bool ret = mConfig->SetNodeAttribute("other", "temppath", "fileexpire", expire.c_str());
	return ret;
}

std::string PropManager::GetSharePathByLoc(std::string loc)
{
	string temppath = "";
	map<string, string>::iterator iter = mRecLocToShare.find(loc);
	if (iter != mRecLocToShare.end())
	{
		temppath = iter->second;
	}
	return temppath;
}


bool PropManager::GetSharePathByLoc(std::string loc, std::string& sharepath)
{
	map<string, string>::iterator iter = mRecLocToShare.find(loc);
	if (iter == mRecLocToShare.end())
	{
		return false;
	}
	sharepath = iter->second;
	return true;
}


std::string PropManager::GetRecPathByDevId(int deviceid)	//通过通道号获取录像文件存放路径
{
	map<string, list<int>>::iterator mapIter = mDeviceIdToPath.begin();
	for (;mapIter!=mDeviceIdToPath.end();++mapIter)
	{
		if ( !(mapIter->second).empty() )
		{
			list<int>::iterator listIter = (mapIter->second).begin();
			for (;listIter!=(mapIter->second).end();++listIter)
			{
				if (deviceid == *listIter)
				{
					return mapIter->first;
				}
			}
		}
		else
			continue;
	}

	return "";
}


std::string PropManager::GetStoreType()
{
	return mStoreType;
}

	
//获取异态录像文件头偏移秒数
std::string PropManager::GetAlarmHeadOffset(enumDVBType eDvbtype)	
{ 
	if (mRecParamInfo.find(eDvbtype)==mRecParamInfo.end())
		return string("20");	//默认值为20

	return mRecParamInfo[eDvbtype].alarmheadoffset; 
}		
//获取异态录像文件尾偏移秒数
std::string PropManager::GetAlarmTailOffset(enumDVBType eDvbtype)	
{
	if (mRecParamInfo.find(eDvbtype)==mRecParamInfo.end())
		return string("20");	//默认值为20

	return mRecParamInfo[eDvbtype].alarmtailoffset;
}	
	
//获取异态录像过期天数
std::string PropManager::GetAlarmRecordExpire(enumDVBType eDvbtype)	
{
	if (mRecParamInfo.find(eDvbtype)==mRecParamInfo.end())
		return string("30");	//默认值为30天

	return mRecParamInfo[eDvbtype].alarmrecordexpire; 
}	

//获取普通录像过期天数
std::string PropManager::GetRecordExpiredays(enumDVBType eDvbtype)
{
	if (mRecParamInfo.find(eDvbtype)==mRecParamInfo.end())
		return string("7");		//默认值为7天

	return mRecParamInfo[eDvbtype].recordexpire; 
}
//获取异态录像mode
std::string PropManager::GetAlarmRecordMode(enumDVBType eDvbtype)	
{
	if (mRecParamInfo.find(eDvbtype)==mRecParamInfo.end())
		return string("all");	//默认值为all

	return mRecParamInfo[eDvbtype].alarmrecordmode; 
}


#ifdef ZONG_JU_ZHAO_BIAO
bool PropManager::GetDevListOfSameIp( int deviceid, std::list<int>& deviceidlist )
{
	//配置文件中没有传进来的通道号
	if ( mDeviceInfo.find(deviceid)==mDeviceInfo.end() )
		return false;

	std::string cardip = mDeviceInfo[deviceid].CMDIP;
	std::map<int,sChanelInfo>::iterator devIter = mDeviceInfo.begin();
	for (; devIter!=mDeviceInfo.end(); devIter++)
	{
		if ( (*devIter).second.CMDIP == cardip && (*devIter).first!=deviceid )
		{
			deviceidlist.push_back( devIter->first );
		}
	}
	//该ip只有一个通道被使用
	if ( deviceidlist.empty() )
		return false;

	return true;
}

bool PropManager::SetUpUrlByType( std::string strType, std::string strUpUrl )
{
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.find( strType );
	if (iter != m_mapEquInfo.end())
	{
		iter->second->strUrl = strUpUrl;
	}

	mConfig->SetNodeAttribute("center", "type", strType, "url", strUpUrl);
	
	return true;
}


bool PropManager::SetUpCodeByType( std::string strType, std::string strCode )
{
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.find( strType );
	if ( iter != m_mapEquInfo.end() )
	{
		iter->second->strSrcCode = strCode;
	}
	else
		return false;

	mConfig->SetNodeAttribute("center", "type", strType, "srccode", strCode);
	return true;
}

void PropManager::SetUpAlarmUrl( eDVBType dvbtype, std::string strUpUrl )
{
	mUpAlarmInfo[dvbtype].alarmurl = strUpUrl;

	return;
}

void  PropManager::SetUpStreamRoundUrl(eDVBType dvbtype,std::string strUpUrl)
{
	mUpStreamRoundInfo[dvbtype].streamroundurl = strUpUrl;
	return;
}

bool PropManager::SetCasIp( std::string strCasIp )
{
	CASIPAddr = strCasIp;

	mConfig->SetNodeAttribute("CASSMS", "CAS", "IP", strCasIp.c_str());
	return true;
}

bool PropManager::SetCasPort( std::string strCasPort )
{
	CASPort = StrUtil::Str2Int( strCasPort );

	mConfig->SetNodeAttribute("CASSMS", "CAS", "port", strCasPort.c_str());
	return true;
}

bool PropManager::SetCasUrl( std::string strCasUrl )
{
	CASRequestURL= strCasUrl;

	mConfig->SetNodeAttribute("CASSMS", "CAS", "path", strCasUrl.c_str());
	return true;
}

bool PropManager::SetSmsIp( std::string strSmsIp )
{
	SMSIPAddr = strSmsIp;

	mConfig->SetNodeAttribute("CASSMS", "SMS", "IP", strSmsIp.c_str());
	return true;
}

bool PropManager::SetSmsPort( std::string strSmsPort )
{
	SMSPort = StrUtil::Str2Int( strSmsPort );  

	mConfig->SetNodeAttribute("CASSMS", "SMS", "port", strSmsPort.c_str());
	return true;
}

bool PropManager::SetSmsUrl( std::string strSmsUrl )
{
	SMSRequestURL = strSmsUrl; 

	mConfig->SetNodeAttribute("CASSMS", "SMS", "URL", strSmsUrl.c_str());
	return true;
}

std::string PropManager::GetUrlByType( const std::string& strEquType )
{
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.find( strEquType );
	if ( iter != m_mapEquInfo.end() )
	{
		return iter->second->strUrl;
	}
	else
		return "";
}

void PropManager::SetHDRecordVideoParam(eDVBType dvbtype, RateParam param)
{
	mHDRecordVideoPram[dvbtype].bps = param.bps;
	mHDRecordVideoPram[dvbtype].height = param.height;
	mHDRecordVideoPram[dvbtype].width = param.width;

	return;
}

void PropManager::SetSDRecordVideoParam(eDVBType dvbtype, RateParam param)
{
	mSDRecordVideoPram[dvbtype].bps = param.bps;
	mSDRecordVideoPram[dvbtype].height = param.height;
	mSDRecordVideoPram[dvbtype].width = param.width;

	return;
}


#endif