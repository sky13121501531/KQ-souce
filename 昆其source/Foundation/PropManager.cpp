
#include <iostream>
#include "PropManager.h"
#include "StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/C99IRsetSerial.h"
#include "../Alarm/AlarmMgr.h"
#include "../Foundation/ParamSetManager.h"
#include "../Communications/DeviceServer.h"
extern vector<sAgentSetInfo> g_VecAgentSetInfo;
long PropManager::AlarmID=time(0);
ACE_Thread_Mutex g_CoderDevVecMutex;
ACE_Thread_Mutex g_TunerDevVecMutex;
PropManager::PropManager(void)
{
	m_itunerdeviceid = 0;
	m_icodedeviceid = 0;
	//
	std::vector<int> Idlist;
	string src = "123";
	string str;
	InitiateBaseInfo();
	InitiateDeviceInfo();
	InitiateVirtualKeySetInfo();
	InitiateUnUseRecDev();
	LoadHDInfo();
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
	
	std::map<std::string, VirtualKeyInfo*>::iterator ite = m_mapVirtualKeyInfo.begin();
	for (ite; ite != m_mapVirtualKeyInfo.end(); ite++)
	{
		delete (*ite).second;
		(*ite).second = NULL;
	}
	m_mapVirtualKeyInfo.clear();

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
	DBMANAGER::instance()->QueryDvbConfig(CTTB,VecConfig);
	DBMANAGER::instance()->QueryDvbConfig(ATV,VecConfig);
	DBMANAGER::instance()->QueryDvbConfig(CTV,VecConfig);
	DBMANAGER::instance()->QueryDvbConfig(AM,VecConfig);
	DBMANAGER::instance()->QueryDvbConfig(RADIO,VecConfig);
	DBMANAGER::instance()->QueryDvbConfig(DVBS,VecConfig);
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
	const int iListSize = 6;
	std::string nodeList[iListSize] = {"device","center","task","scantype", "work", "alarmParam"}; 
	mXmlProp.LoadFromFile("c:/vscttb/properties.xml");

	m_nAllDeviceIndexCount = 0;
	m_nAllCardCount = 0;

	for (int j = 0;j<iListSize;j++)
	{
		nodePath="properties/"+nodeList[j];
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if(node==NULL)
		{
			continue;
		}
		pXMLNODELIST nodeList=mXmlProp.GetNodeList(node);
		for (int i=0;i<nodeList->Size();i++)
		{
			pXMLNODE nodeNext=mXmlProp.GetNextNode(nodeList);
			switch ( j )
			{
			case 0:           //获取device节点相关信息
				{
					sDeviceInfo sDevInfo;
					sDevInfo.s_unuseditem = "";
					mXmlProp.GetAttrNode(nodeNext,"index",sDevInfo.s_deviceindex);
					mXmlProp.GetAttrNode(nodeNext,"IP",sDevInfo.s_deviceip);//FRee_fanrong_20131212
					mXmlProp.GetAttrNode(nodeNext,"port",sDevInfo.s_deviceport);
					mXmlProp.GetAttrNode(nodeNext,"campronum",sDevInfo.s_devicecampronum);
					mXmlProp.GetAttrNode(nodeNext,"unuseditem",sDevInfo.s_unuseditem);
					if(sDevInfo.s_devicecampronum.size()<=0)
					{
						sDevInfo.s_devicecampronum = "8";
					}
					sDevInfo.i_HDnum = 0;
					sDevInfo.i_SDnum = 0;
					mMonitorDevInfo.push_back(sDevInfo);
					break;
				}
			case 1:          //获取监测中心相关信息 
				{
					string srccode,dstcode,url,type;

					mXmlProp.GetAttrNode(nodeNext,string("type"),type);
					mXmlProp.GetAttrNode(nodeNext,string("url"),url);
					mXmlProp.GetAttrNode(nodeNext,string("srccode"),srccode);
					mXmlProp.GetAttrNode(nodeNext,string("dstcode"),dstcode);

					sEquInfo *pEquInfo = new sEquInfo;
					pEquInfo->strSrcCode = srccode;
					pEquInfo->strDesCode = dstcode;
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

					if (devicelist.empty() == false)
						mTaskInfo.insert(make_pair(tasktype,devicelist));
					if(deviceshare=="yes")
						mTaskShare.insert(make_pair(tasktype,true));
					else
						mTaskShare.insert(make_pair(tasktype,false));
					break;
				}
			case 3:           
				{

					mXmlProp.GetAttrNode(nodeNext,"scan",chantypeinfo.s_scantype);
					mXmlProp.GetAttrNode(nodeNext,"freq",chantypeinfo.s_freq);
					mXmlProp.GetAttrNode(nodeNext,"weeklyswitch",chantypeinfo.s_weeklyswitch);
					mXmlProp.GetAttrNode(nodeNext,"epg",chantypeinfo.s_epg);
					mXmlProp.GetAttrNode(nodeNext,"scanTime",chantypeinfo.s_scanTime);
					mXmlProp.GetAttrNode(nodeNext,"checkHD",chantypeinfo.s_checkHD);
					mXmlProp.GetAttrNode(nodeNext,"ScanVersion",chantypeinfo.s_scanversion);
                    //string strtemp;
                    mXmlProp.GetAttrNode(nodeNext,"adjustval",tempFreq);//m_AlarmTimeRelease
                    mXmlProp.GetAttrNode(nodeNext,"TimeVal",m_AlarmTimeRelease);//AddFlag
					mXmlProp.GetAttrNode(nodeNext,"AddFlag",m_moreVideoadd);
					mXmlProp.GetAttrNode(nodeNext,"AddMoreTime",m_strtimeMore);
					//userinfo---m_username  m_passwd
					mXmlProp.GetAttrNode(nodeNext,"UserName",m_username);
					mXmlProp.GetAttrNode(nodeNext,"passwd",m_passwd);
					break;
				}
			case 4:
				{
					mXmlProp.GetAttrNode(nodeNext,"workVersion",m_sCurWorkVersion);
					mXmlProp.GetAttrNode(nodeNext,"AMNum",m_sAMNum);
					if(m_sAMNum.size() == 0)
					{
						m_sAMNum = "0";
					}
					break;
				}
			case 5:
				{
					string dvbtype, mutehold, bfhold, sthold;
					mXmlProp.GetAttrNode(nodeNext,"dvbtype",dvbtype);
					mXmlProp.GetAttrNode(nodeNext,"mutehold",mutehold);
					mXmlProp.GetAttrNode(nodeNext,"bfhold",bfhold);
					mXmlProp.GetAttrNode(nodeNext,"sthold",sthold);
					AlarmParamConfig alarmParam;
					alarmParam.dvbType = OSFunction::GetEnumDVBType(dvbtype);
					alarmParam.s_mutehold = mutehold;
					alarmParam.s_bfhold = bfhold;
					alarmParam.s_sthold = sthold;
					m_mapAlarmParamConfig.insert(make_pair(alarmParam.dvbType, alarmParam));
					break;
				}
			default:
				break;
			}
		}
	}
	return true;
}

bool PropManager::LoadHDInfo()
{
	XmlParser xml;
	if(xml.LoadFromFile("c:/vscttb/HDInfoSet.xml"))
	{
		pXMLNODE node = xml.GetNodeFromPath("HDInfoSet");
		pXMLNODELIST nodeList = xml.GetNodeList(node);
		for (int i = 0; i < nodeList->Size(); i++)
		{
			pXMLNODE nodeNext=xml.GetNextNode(nodeList);
			HDInfo hdInfo;
			xml.GetAttrNode(nodeNext,"Freq", hdInfo.sFreq);
			xml.GetAttrNode(nodeNext,"ServiceID", hdInfo.sServiceID);
			xml.GetAttrNode(nodeNext,"HDType", hdInfo.sHDType);
			m_vHDInfos.push_back(hdInfo);
		}
	}
	return true;
}
//获取节目的高清信息
bool PropManager::GetHDInfo(std::string freq, std::string serviceID, std::string& hdType)
{
	bool bRet = false;
	if (m_vHDInfos.size() == 0)
		return bRet;
	vector<HDInfo>::iterator iter = m_vHDInfos.begin();
	for (iter; iter != m_vHDInfos.end(); iter++)
	{
		if (((*iter).sFreq == freq) && ((*iter).sServiceID == serviceID))
		{
			hdType = (*iter).sHDType;
			bRet = true;
			break;
		}
	}
	return bRet;
}


//获取 虚拟遥控器按键信息
bool PropManager::InitiateVirtualKeySetInfo()
{
	XmlParser xml;
	if(xml.LoadFromFile("c:/vscttb/VirtualRemoteControlKeySet.xml"))
	{
		pXMLNODE node = xml.GetNodeFromPath("VirtualRemoteControlKey");
		pXMLNODELIST nodeList = xml.GetNodeList(node);
		for (int i = 0; i < nodeList->Size(); i++)
		{
			pXMLNODE nodeNext=xml.GetNextNode(nodeList);
			VirtualKeyInfo* p = new VirtualKeyInfo();
			int i_Index = 0;
			xml.GetAttrNode(nodeNext,"Index", i_Index);
			xml.GetAttrNode(nodeNext,"Name", p->s_Name);
			xml.GetAttrNode(nodeNext,"Key", p->s_Key);
			char buf[5] = {0};
			sprintf(buf, "%04x", i_Index);
			p->s_Index = buf;
			m_mapVirtualKeyInfo.insert(make_pair(p->s_Key, p));
		}
	}
	return true;
}
VirtualKeyInfo* PropManager::GetVirtualKeyInfo(std::string sVirtualKey)
{
	if (sVirtualKey == "")
		return NULL;
 	std::map<std::string, VirtualKeyInfo*>::iterator ite;
	ite = m_mapVirtualKeyInfo.find(sVirtualKey);	
	VirtualKeyInfo* p = NULL;
	if (ite != m_mapVirtualKeyInfo.end())
	{
		return ite->second;
	}
	return NULL;
}
bool PropManager::InitiateUnUseRecDev()
{
	std::list<int> tundevlist;
	//实时视频,频道轮播,频道扫描,MER-BER测量使用解调设备;实时视频,频道轮播使用转码设备
	GetMER_BERDeviceList(DVBC,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",DVBC,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",DVBC,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",DVBC,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",DVBC,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",DVBC,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(DVBC,tundevlist));
	//
	tundevlist.clear();
	GetMER_BERDeviceList(CTTB,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",CTTB,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",CTTB,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",CTTB,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",CTTB,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",CTTB,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(CTTB,tundevlist));
	//
	tundevlist.clear();
	GetMER_BERDeviceList(RADIO,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",RADIO,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",RADIO,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",RADIO,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",RADIO,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",RADIO,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(RADIO,tundevlist));
	//
	tundevlist.clear();
	GetMER_BERDeviceList(ATV,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",ATV,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",ATV,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",ATV,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",ATV,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",ATV,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(ATV,tundevlist));
	//
	tundevlist.clear();
	GetMER_BERDeviceList(AM,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",AM,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",AM,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",AM,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",AM,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",AM,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(AM,tundevlist));
	//
	tundevlist.clear();
	GetMER_BERDeviceList(CTV,tundevlist);
	GetTaskDeviceList("ChannelScanQueryTask",CTV,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",CTV,tundevlist);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",CTV,tundevlist);
	GetTaskDeviceList("StreamRealtimeQueryTask",CTV,m_unUseRecCoder);
	GetTaskDeviceList("StreamRealtimeRoundQueryTask",CTV,m_unUseRecCoder);
	m_unUseRecTuner.insert(make_pair(CTV,tundevlist));
	//
	return true;
}
bool PropManager::IsUnUseRecDev(int devtype,eDVBType dvbtype,int deviceid)
{
	bool ret = false;
	if(devtype == 0)
	{
		map<eDVBType,list<int>>::iterator MPtr = m_unUseRecTuner.find(dvbtype);
		if(MPtr != m_unUseRecTuner.end())
		{
			list<int>::iterator Ptr = MPtr->second.begin();
			for(;Ptr != MPtr->second.end();Ptr++)
			{
				if(deviceid == (*Ptr))
				{
					ret = true;
					break;
				}
			}
		}
	}
	else if(devtype == 1)
	{
		list<int>::iterator Ptr = m_unUseRecCoder.begin();
		for(;Ptr != m_unUseRecCoder.end();Ptr++)
		{
			if(deviceid == (*Ptr))
			{
				ret = true;
				break;
			}
		}
	}
	return ret;
}
//**************************************************
bool PropManager::GetScanInfo(string& scantype,string& freq,string& weeklyswitch,string& epg, string& scantime, string& checkHD,string& ScanVer)
{
	freq = chantypeinfo.s_freq;
	scantype = chantypeinfo.s_scantype;
	weeklyswitch = chantypeinfo.s_weeklyswitch;
	epg = chantypeinfo.s_epg;
	scantime = chantypeinfo.s_scanTime;
	checkHD = chantypeinfo.s_checkHD;
	ScanVer = chantypeinfo.s_scanversion;
	return true;
}
bool PropManager::GetDeviceInfo()
{
	bool ret = true;
	int icodedeviceid = 0,itunerindex = 0,itunerdeviceid[11] = {0,0,0,0,0,0,0,0,0,0,0};//itunerdeviceid[10]是存储HDMI
	std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	int AMNum = StrUtil::Str2Int(GetAMNum());
	int curamnum = 0;
	for (; iter!=mMonitorDevInfo.end(); iter++)
	{
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(iter->s_deviceindex),iter->s_deviceip,StrUtil::Str2Int(iter->s_deviceport));
		//
		string strSetTimeCMD = "POST /device/info  HTTP/1.1  \r\n\r\n";
		strSetTimeCMD+="time=";
		strSetTimeCMD+=StrUtil::Long2Str(time(0) + 3);
		string strSetTimeRet;
		//pDeviceAccess->SendTaskMsg(strSetTimeCMD,strSetTimeRet);
		/*if(strSetTimeRet.size() <= 0)
		{
			cout<<"授时命令失败:"<<iter->s_deviceip<<"--"<<iter->s_deviceport<<endl;
		}*/
		//
		string strCMD = "GET /device/config HTTP/1.1 \r\n\r\n";//1.1设备配置查询
		//**************************************
		string cmd;
		//***************************************
		string strRet = "";
		bool isok = pDeviceAccess->SendTaskMsg(strCMD,strRet);
		/*<html><body><IOSlot  num=0/><TCoder  num=32/><MShow  num=32/><IODesc  desc=DTMB_0_4/><MShowSize size=1280*720></body></html>*/
		/*string strIo = "<html><body><IOSlot  num=0/><TCoder  num=33/><MShow  num=1/><IODesc  desc=DVBC_0_5/><MShowSize  size=1280_720/></body></html>";
		if(strRet!=strIo)
		{
			strRet = strIo;
		}*/
		//*************故障报警开始******************
		if(isok)
		{
			sCheckParam sCheck;
			sCheck.AlarmType	= ALARM_EQUIPMENT;
			sCheck.DVBType		= DVBC;
			sCheck.ChannelID	= "";
			sCheck.Freq			= "";
			sCheck.STD			= "";
			sCheck.SymbolRate	= "";
			sCheck.TypedValue	= "";
			sCheck.DeviceID		= iter->s_deviceindex;
			sCheck.mode = "1";
			sCheck.TypeDesc		= "解调解扰设备故障报警解除:"+iter->s_deviceip;
			sCheck.TypeID		= "60";
			sCheck.CheckTime	= time(0);
			//ALARMMGR::instance()->CheckAlarm(sCheck,false);
		}
		else
		{
			sCheckParam sCheck;
			sCheck.AlarmType	= ALARM_EQUIPMENT;
			sCheck.DVBType		= DVBC;
			sCheck.ChannelID	= "";
			sCheck.Freq			= "";
			sCheck.STD			= "";
			sCheck.SymbolRate	= "";
			sCheck.TypedValue	= "";
			sCheck.DeviceID		= iter->s_deviceindex;
			sCheck.mode = "0";
			sCheck.TypeDesc		= "解调解扰设备故障报警:"+iter->s_deviceip;
			sCheck.TypeID		= "60";
			sCheck.CheckTime	= time(0);
			//ALARMMGR::instance()->CheckAlarm(sCheck,false);
		}
		//*************故障报警结束******************
		if(strRet.size() <= 0)
		{
			cout<<"命令失败:"<<iter->s_deviceip<<"--"<<iter->s_deviceport<<endl;
			continue;
		}
		else
		{
			cout<<"收到结果:"<<strRet<<endl;
		}
		int pos = strRet.find("<html>");
		string strtem = strRet;
		if(pos >= 0)
		{
			strtem = strRet.substr(pos,strRet.size()-pos);
		}
		else
		{
			cout<<"没有<html>标记:"<<iter->s_deviceip<<"--"<<iter->s_deviceport<<endl;
			continue;
		}
		//
		string strtunerserial = "jsdvbct201707";
		string strcoderserial = "jsdvbcc201707";
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
							tdev.s_monitorindex = iter->s_deviceindex;
							tdev.s_cardindex = temp.substr(0,pos);
							int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
							for(int i=0;i<cardnum;i++)
							{
								ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
								tdev.s_chanindex = StrUtil::Int2Str(i);
								if((iter->s_unuseditem.size()<=0)||(iter->s_unuseditem.find(tdev.s_cardindex+"_"+tdev.s_chanindex)==-1))
								{
									tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[DVBC]);
									tdev.b_user = false;
									tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
									tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
									tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
									tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
									tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
									tdev.e_tunerdvbtype = DVBC;
									tdev.s_devicecampronum = iter->s_devicecampronum;
									tdev.s_serialnum = strtunerserial+tdev.s_tunerdeviceid;
									string pos = tdev.s_monitorindex+"_"+tdev.s_cardindex+"_"+tdev.s_chanindex;
									tdev.s_ICcardNO = "";
									PARAMSETMANAGER::instance()->GetCardNoByPosition(pos,tdev.s_ICcardNO);
									mTunerDevVec.push_back(tdev);
									itunerdeviceid[DVBC]++;
									itunerindex++;
								}
								else
								{
									cout<<"配置不使用:"<<tdev.s_cardindex<<"_"<<tdev.s_chanindex<<endl;
								}
							}
						}
					}
					else if(tunertype == "DTMB")
					{
						string temp = info.substr(pos+1,info.size());
						pos = temp.find("_");
						if(pos != -1)
						{
							TunerDev tdev;
							tdev.s_monitorindex = iter->s_deviceindex;
							tdev.s_cardindex = temp.substr(0,pos);
							int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
							for(int i=0;i<cardnum;i++)
							{
								ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
								tdev.s_chanindex = StrUtil::Int2Str(i);
								tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[CTTB]);
								tdev.b_user = false;
								tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
								tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
								tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
								tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
								tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
								tdev.e_tunerdvbtype = CTTB;
								tdev.s_devicecampronum = iter->s_devicecampronum;
								mTunerDevVec.push_back(tdev);
								itunerdeviceid[CTTB]++;
								itunerindex++;
							}
						}
					}
					else if(tunertype == "DVBS")
					{
						string temp = info.substr(pos+1,info.size());
						pos = temp.find("_");
						if(pos != -1)
						{
							TunerDev tdev;
							tdev.s_monitorindex = iter->s_deviceindex;
							tdev.s_cardindex = temp.substr(0,pos);
							int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
							for(int i=0;i<cardnum;i++)
							{
								ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
								tdev.s_chanindex = StrUtil::Int2Str(i);
								tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[DVBS]);
								tdev.b_user = false;
								tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
								tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
								tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
								tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
								tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
								tdev.e_tunerdvbtype = DVBS;
								tdev.s_devicecampronum = iter->s_devicecampronum;
								mTunerDevVec.push_back(tdev);
								itunerdeviceid[DVBS]++;
								itunerindex++;
							}
						}
					}
					else if (tunertype == "BNC")
					{
						string temp = info.substr(pos+1,info.size());
						pos = temp.find("_");
						if (pos != -1)
						{
							string s_HDMICardIndex = temp.substr(0, pos);
							//
							TunerDev tdev;
							tdev.s_monitorindex = iter->s_deviceindex;
							tdev.s_cardindex = temp.substr(0,pos);
							int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
							for(int i=0;i<cardnum;i++)
							{
								ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
								tdev.s_chanindex = StrUtil::Int2Str(i);
								tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[HDMI]);
								tdev.b_user = false;
								tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
								tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
								tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
								tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
								tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
								tdev.e_tunerdvbtype = HDMI;
								tdev.s_devicecampronum = iter->s_devicecampronum;
								mTunerDevVec.push_back(tdev);
								itunerdeviceid[HDMI]++;
								itunerindex++;
							}
						}
					}
					else if (tunertype == "FMAM")
					{
						if(curamnum < AMNum)
						{
							curamnum++;
							//
							string temp = info.substr(pos+1,info.size());
							pos = temp.find("_");
							if (pos != -1)
							{
								string s_HDMICardIndex = temp.substr(0, pos);
								//
								TunerDev tdev;
								tdev.s_monitorindex = iter->s_deviceindex;
								tdev.s_cardindex = temp.substr(0,pos);
								int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
								for(int i=0;i<cardnum;i++)
								{
									ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
									tdev.s_chanindex = StrUtil::Int2Str(i);
									tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[AM]);
									tdev.b_user = false;
									tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
									tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
									tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
									tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
									tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
									tdev.e_tunerdvbtype = AM;
									tdev.s_devicecampronum = iter->s_devicecampronum;
									mTunerDevVec.push_back(tdev);
									itunerdeviceid[AM]++;
									itunerindex++;
								}
							}
						}
						else
						{
							string temp = info.substr(pos+1,info.size());
							pos = temp.find("_");
							if (pos != -1)
							{
								string s_HDMICardIndex = temp.substr(0, pos);
								//
								TunerDev tdev;
								tdev.s_monitorindex = iter->s_deviceindex;
								tdev.s_cardindex = temp.substr(0,pos);
								int cardnum = StrUtil::Str2Int(temp.substr(pos+1,temp.size()));
								for(int i=0;i<cardnum;i++)
								{
									ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
									tdev.s_chanindex = StrUtil::Int2Str(i);
									tdev.s_tunerdeviceid = StrUtil::Int2Str(itunerdeviceid[RADIO]);
									tdev.b_user = false;
									tdev.s_sip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(20+itunerindex);
									tdev.s_stsport = StrUtil::Int2Str(7000+itunerindex);
									tdev.s_trport = StrUtil::Int2Str(8000+itunerindex);
									tdev.s_290ip = "239.0.0."+StrUtil::Int2Str(20+itunerindex);
									tdev.s_290port = StrUtil::Int2Str(8500+itunerindex);
									tdev.e_tunerdvbtype = RADIO;
									tdev.s_devicecampronum = iter->s_devicecampronum;
									mTunerDevVec.push_back(tdev);
									itunerdeviceid[RADIO]++;
									itunerindex++;
								}
							}
						}
					}
				}
			}
			else if(name == "TCoder")
			{
				psr.GetAttrNode(childNode,"num",num);
				if (GetCurWorkVersion() == "3.0"||GetCurWorkVersion() == "4.0"||GetCurWorkVersion() == "3.9")
				{
					num = "33";
				}
				else
				{
					int temnum = StrUtil::Str2Int(num) + 1;//录像60路用
					num = StrUtil::Int2Str(temnum);
				}
				//num = "30";//test 
				iter->i_TcoderNum = StrUtil::Str2Int(num);
				CoderDev cdev;
				cdev.s_monitorindex = iter->s_deviceindex;
				for(int i=0;i<StrUtil::Str2Int(num);i++)
				{
					ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
					//
					cdev.s_coderdeviceid = StrUtil::Int2Str(icodedeviceid);
					cdev.s_coderindex = StrUtil::Int2Str(i);
					cdev.s_cardindex = string("6");
					cdev.s_modindex = StrUtil::Int2Str(i/8);
					cdev.s_modcoderindex = StrUtil::Int2Str(i%8);
					cdev.b_user = false;
					cdev.s_dip = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(100+icodedeviceid);
					cdev.s_dtsport = StrUtil::Int2Str(9000+icodedeviceid);
					cdev.s_code = "h264";
					cdev.s_ena = "1";
					cdev.s_fmt = "0";
					cdev.s_fre = "0";
					cdev.s_rate = "400";
					cdev.s_alarmip = "239.0.0."+StrUtil::Int2Str(100+icodedeviceid);
					cdev.s_alarmport = StrUtil::Int2Str(9500+icodedeviceid);
					cdev.s_serialnum = strcoderserial+cdev.s_coderdeviceid;
					mCoderDevVec.push_back(cdev);
					icodedeviceid++;
					//关闭转码资源
					cmd = "POST /tcoder/config  HTTP/1.1  \r\n\r\n";
					strRet = "";
					cmd+="chl=";
					cmd+=cdev.s_coderindex;
					cmd+="&ena=";
					cmd+="0";
					cmd+="&chk=";
					cmd+="0";
					cmd+="&sip=";
					cmd+="0.0.0.0";
					cmd+="&spt=";
					cmd+="0";
					cmd+="&spro=";
					cmd+="0";
					cmd+="&svid=";
					cmd+="0";
					cmd+="&saud0=";
					cmd+="0";
					cmd+="&saud1=";
					cmd+="0";
					cmd+="&saud2=";
					cmd+="0";
					cmd+="&vcode=";
					cmd+="h264";
					cmd+="&acode=";
					cmd+="mpeg";
					cmd+="&vfmt=";
					cmd+="2";//1 D1 2 CIF 0 自动
					cmd+="&vfre=";
					cmd+="0";
					cmd+="&v1v1=";
					cmd+="2";
					cmd+="&mutehold=";
					cmd+="-70";
					cmd+="&bfhold=";
					cmd+="20";
					cmd+="&sthold=";
					cmd+="1.5";
					cmd+="&vrate=";
					cmd+="1350";
					cmd+="&arate=";
					cmd+="32";
					cmd+="&srate=";
					cmd+="1500";
					cmd+="&osd0=";
					cmd+="150_40_16_time";
					cmd+="&osd1=";
					cmd+="50_50_16_test";
					cmd+="&osd2=";
					cmd+="150_200_16_test";
					cmd+="&dip=";
					cmd+="0.0.0.0";
					cmd+="&dpt=";
					cmd+="0";
					cmd+="&evip=";
					cmd+="0.0.0.0";
					cmd+="&evpt=";
					cmd+="0";
					cout<<"发送命令:"<<cmd<<endl;
					pDeviceAccess->SendTaskMsg(cmd,strRet);
					cout<<"收到结果:"<<strRet<<endl;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
					//
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
		delete pDeviceAccess;
	}
	return ret;
}
//
bool PropManager::GetDevMonitorIndex(int devtype,eDVBType tunerDVB,int deviceid,string& monitorindex)
{
	bool ret = false;
	if(devtype == 0)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
		std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
		for (; iter!=mTunerDevVec.end(); iter++)
		{
			if((deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))&&(tunerDVB == iter->e_tunerdvbtype))
			{
				monitorindex = iter->s_monitorindex;
				ret = true;
				break;
			}
		}
	}
	else if(devtype == 1)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
		std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
		for (; iter!=mCoderDevVec.end(); iter++)
		{
			if(deviceid == StrUtil::Str2Int(iter->s_coderdeviceid))
			{
				monitorindex = iter->s_monitorindex;
				ret = true;
				break;
			}
		}
	}
	return ret;
}
//
bool PropManager::GetDevMonitorInfo(int monitorindex,string& monitorip,string& monitorport)
{
	bool ret = false;
	std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	for (; iter!=mMonitorDevInfo.end(); iter++)
	{
		if(StrUtil::Str2Int(iter->s_deviceindex)==monitorindex)
		{
			monitorip = iter->s_deviceip;
			monitorport = iter->s_deviceport;
			ret = true;
			break;
		}
	}
	return ret;
}
//
bool PropManager::GetDevMonitorInfo(int devtype,eDVBType tunerDVB,int deviceid,string& monitorip,string& monitorport)
{
	bool ret = false;
	string monitorindex,slotindex,modindex;
	if(GetCurWorkVersion() == "5.0")
	{
		if(devtype == 0)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
			std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
			for (; iter!=mTunerDevVec.end(); iter++)
			{
				if((deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))&&(tunerDVB == iter->e_tunerdvbtype))
				{
					std::vector<sDeviceInfo>::iterator monitoriter = mMonitorDevInfo.begin();
					for (; monitoriter!=mMonitorDevInfo.end(); monitoriter++)
					{
						if(monitoriter->s_deviceindex == iter->s_monitorindex
							&&monitoriter->s_cardindex == iter->s_cardindex
							&&monitoriter->ucharBoardType == 0)
						{
							monitorip = monitoriter->s_deviceip;
							monitorport = monitoriter->s_deviceport;
							ret = true;
							break;
						}
					}
					break;
				}
			}
		}
		else if(devtype == 1)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
			std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
			for (; iter!=mCoderDevVec.end(); iter++)
			{
				if(deviceid == StrUtil::Str2Int(iter->s_coderdeviceid))
				{
					std::vector<sDeviceInfo>::iterator monitoriter = mMonitorDevInfo.begin();
					for (; monitoriter!=mMonitorDevInfo.end(); monitoriter++)
					{
						if(monitoriter->s_deviceindex == iter->s_monitorindex
							&&monitoriter->s_cardindex == iter->s_cardindex
							&&monitoriter->s_modindex == iter->s_modindex)
						{
							monitorip = monitoriter->s_deviceip;
							monitorport = monitoriter->s_deviceport;
							ret = true;
							break;
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		if(devtype == 0)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
			std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
			for (; iter!=mTunerDevVec.end(); iter++)
			{
				if((deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))&&(tunerDVB == iter->e_tunerdvbtype))
				{
					monitorindex = iter->s_monitorindex;
					break;
				}
			}
		}
		else if(devtype == 1)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
			std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
			for (; iter!=mCoderDevVec.end(); iter++)
			{
				if(deviceid == StrUtil::Str2Int(iter->s_coderdeviceid))
				{
					monitorindex = iter->s_monitorindex;
					break;
				}
			}
		}
		if(monitorindex.size()>0)
		{
			ret = true;
			std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
			for (; iter!=mMonitorDevInfo.end(); iter++)
			{
				if(iter->s_deviceindex==monitorindex)
				{
					monitorip = iter->s_deviceip;
					monitorport = iter->s_deviceport;
					ret = true;
					break;
				}
			}
		}
	}
	return ret;
}
bool PropManager::GetDevMonitorInfo(int index,sDeviceInfo& DeviceInfo)
{
	bool ret = false;
	if(index<mMonitorDevInfo.size())
	{
		DeviceInfo = mMonitorDevInfo[index];
		ret = true;
	}
	return ret;
}
bool PropManager::IncMonitorHDSD(int monitorindex,bool bIsHD)
{
	bool ret = false;
	std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	for (; iter!=mMonitorDevInfo.end(); iter++)
	{
		if(((PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")&&(iter->ucharBoardType==1))||(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
		{
			if(StrUtil::Str2Int(iter->s_deviceindex)==monitorindex)
			{
				int number = 0;
				if(bIsHD)
				{
					number = ((iter->i_HDnum+1)*4)+iter->i_SDnum;
					if(number<=iter->i_TcoderNum)
					{
						iter->i_HDnum++;
						ret = true;
						break;
					}
				}
				else
				{
					number = (iter->i_HDnum*4)+iter->i_SDnum+1;
					if(number<=iter->i_TcoderNum)
					{
						iter->i_SDnum++;
						ret = true;
						break;
					}
				}
			}
		}
	}
	return ret;
}
bool PropManager::DecMonitorHDSD(int monitorindex,bool bIsHD)
{
	bool ret = false;
	std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	for (; iter!=mMonitorDevInfo.end(); iter++)
	{
		if(StrUtil::Str2Int(iter->s_deviceindex)==monitorindex)
		{
			if(bIsHD)
			{
				if(iter->i_HDnum > 0)
				{
					iter->i_HDnum--;
				}
				ret = true;
			}
			else
			{
				if(iter->i_SDnum > 0)
				{
					iter->i_SDnum--;
				}
				ret = true;
			}
			break;
		}
	}
	return ret;
}
//
bool PropManager::GetTunerInfo(int tunerindex,TunerDev& TunerDevInfo)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (int i = 0; iter!=mTunerDevVec.end(); iter++,i++)
	{
		if(i == tunerindex)
		{
			TunerDevInfo = (*iter);
			ret = true;
		}
	}
	return ret;
}
bool PropManager::ExistTunerCardFromCardindex(int ChassisID,int Cardindex)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((Cardindex == StrUtil::Str2Int(iter->s_cardindex))&&(ChassisID == StrUtil::Str2Int(iter->s_monitorindex)))
		{
			ret = true;
			break;
		}
	}
	return ret;
}
int  PropManager::GetTunerDeviceID(eDVBType tunerDVB,int ChassisID,int Position,int Demodulate)
{
	int iret = -1;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((ChassisID == StrUtil::Str2Int(iter->s_monitorindex))&&(tunerDVB == iter->e_tunerdvbtype)
			&&(Position == StrUtil::Str2Int(iter->s_cardindex))&&(Demodulate == StrUtil::Str2Int(iter->s_chanindex)))
		{
			iret = StrUtil::Str2Int(iter->s_tunerdeviceid);
			break;
		}
	}
	return iret;
}
float PropManager::GetChassisBandWidth(int ChassisID)//Mbps
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	int iBandWidth = 1000;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(ChassisID == StrUtil::Str2Int(iter->s_monitorindex))
		{
			iBandWidth += StrUtil::Str2Int(iter->s_rate);
			break;
		}
	}
	return ((float)iBandWidth)/1000.0f;
}
int  PropManager::GetCoderDeviceID(int ChassisID,int Position,int ModuleID,int Transcode)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	int iret = -1;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if((ChassisID == StrUtil::Str2Int(iter->s_monitorindex))&&(ModuleID == StrUtil::Str2Int(iter->s_modindex))
			&&(Position == StrUtil::Str2Int(iter->s_cardindex))&&(Transcode == StrUtil::Str2Int(iter->s_modcoderindex)))
		{
			iret = StrUtil::Str2Int(iter->s_coderdeviceid);
			break;
		}
	}
	return iret;
}

bool PropManager::GetTunerInfo(int deviceid,eDVBType tunerDVB,TunerDev& TunerDevInfo)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((deviceid == StrUtil::Str2Int(iter->s_tunerdeviceid))&&(tunerDVB == iter->e_tunerdvbtype))
		{
			TunerDevInfo = (*iter);
			ret = true;
			break;
		}
		else if(tunerDVB == UNKNOWN)
		{
			TunerDevInfo = mTunerDevVec[deviceid];
			ret = true;
			break;
		}
	}
	return ret;
}

bool PropManager::GetDeviceInfo(string ChassisID,string Position,string ModuleID,sDeviceInfo& DevInfo)
{
	bool ret = false;
	std::vector<sDeviceInfo>::iterator iter = mMonitorDevInfo.begin();
	for (;iter!=mMonitorDevInfo.end(); iter++)
	{
		if (ModuleID.length()<1)
		{
			if((ChassisID == iter->s_deviceindex)&&(Position == iter->s_cardindex))
			{
				DevInfo = (*iter);
				ret = true;
				break;
			}
		}
		else
		{
			if((ChassisID == iter->s_deviceindex)&&(Position == iter->s_cardindex)&&(ModuleID == iter->s_modindex))
			{
				DevInfo = (*iter);
				ret = true;
				break;
			}
		}
	}
	return ret;
}
bool PropManager::GetCoderInfo(int deviceid,CoderDev& CoderDevInfo)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	int codersize = mCoderDevVec.size();
	for (int i=0; i<codersize; i++)
	{
		if(deviceid == StrUtil::Str2Int(mCoderDevVec[i].s_coderdeviceid))
		{
			CoderDevInfo = mCoderDevVec[i];
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::GetCoderVecFromFreqSerID(string Freq,string Serviceid, vector<CoderDev>& CoderDevInfo)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if((Freq == iter->s_freq)&&(Serviceid == iter->s_spro))
		{
			CoderDevInfo.push_back(*iter);
			ret = true;
		}
	}
	return ret;
}
int PropManager::GetUnUsedCoderDevNum()
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	int unusenum = 0;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if((iter->b_user == false)&&(!IsUnUseRecDev(1,UNKNOWN,StrUtil::Str2Int(iter->s_coderdeviceid))))
		{
			unusenum++;
		}
	}
	return unusenum;
}

bool PropManager::IsHadUnUsedTunerOrCoder(eDVBType dvbtype, map<string, vector<string>> mapFreqAndServID)
{
	int unusenum = 0;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((!(iter->b_user))&&(dvbtype == iter->e_tunerdvbtype)&&(!IsUnUseRecDev(0,dvbtype,StrUtil::Str2Int(iter->s_tunerdeviceid))))
		{
			unusenum++;
		}
	}
	if (unusenum > 0)
	{
		int iMonDevNum = GetMonitorDevNum();
		map<string, vector<string>> freq2ServIDs =  mapFreqAndServID;
		for (int i = 0; i < iMonDevNum; i++)
		{
			ACE_Guard<ACE_Thread_Mutex> tguard(g_TunerDevVecMutex);
			int iMonUnUsedTunerNum = 0;//同设备未使用 tunner 个数
			int iMonUnUsedCoderNum = 0;//同设备未使用 coder 个数
			std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
			for (; iter!=mTunerDevVec.end(); iter++)
			{
				if((!(iter->b_user))&&(dvbtype == iter->e_tunerdvbtype)&&(!IsUnUseRecDev(0,dvbtype,StrUtil::Str2Int(iter->s_tunerdeviceid))))
				{
					if (StrUtil::Str2Int(iter->s_monitorindex) == i)
					{
						iMonUnUsedTunerNum++;
					}
				}
			}
			ACE_Guard<ACE_Thread_Mutex> cguard(g_CoderDevVecMutex);
			std::vector<CoderDev>::iterator coderIter = mCoderDevVec.begin();
			for (coderIter; coderIter != mCoderDevVec.end(); coderIter++)
			{
				if((coderIter->b_user == false)&&(!IsUnUseRecDev(1,UNKNOWN,StrUtil::Str2Int(coderIter->s_coderdeviceid))))
				{
					if (StrUtil::Str2Int(coderIter->s_monitorindex) == i)
					{
						iMonUnUsedCoderNum++;
					}
				}
			}
			if (iMonUnUsedTunerNum == 0 || iMonUnUsedCoderNum == 0)
				continue;
			vector<string> vFreqs;
			map<string, vector<string>>::iterator mapIter = freq2ServIDs.begin();
			for (mapIter; mapIter != freq2ServIDs.end(); mapIter++)
			{
				string freq = (*mapIter).first;
				vector<string> vServID = (*mapIter).second;
				iMonUnUsedTunerNum--;
				iMonUnUsedCoderNum -= vServID.size();
				if ((iMonUnUsedTunerNum >= 0) && (iMonUnUsedCoderNum >= 0))
				{//设备有能力解 新加频点下的所有节目
					vFreqs.push_back(freq);
				}
			}
			if (vFreqs.size() > 0)
			{
				vector<string>::iterator vIter = vFreqs.begin();
				map<string, vector<string>> mapFreqServIDs =  freq2ServIDs;
				for (vIter; vIter != vFreqs.end(); vIter++)
				{
					string freq = (*vIter);
					map<string, vector<string>>::iterator mIter = mapFreqServIDs.find(freq);
					if (mIter != mapFreqServIDs.end())
					{
						mapFreqServIDs.erase(mIter);
					}
				}
				freq2ServIDs = mapFreqServIDs;
			}
		}

		if (freq2ServIDs.size() > 0)
			return false;
	}
	else if (unusenum == 0)
	{
		map<string, vector<string>>::iterator ite = mapFreqAndServID.begin();
		for (ite; ite != mapFreqAndServID.end(); ite++)
		{
			bool bRet = false;
			ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
			string freq = (*ite).first;
			std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
			for (; iter!=mTunerDevVec.end(); iter++)
			{
				if(dvbtype == iter->e_tunerdvbtype)
				{
					if (freq == iter->s_freq)
					{
						bRet = true;
						break;
					}
				}
			}
			if (!bRet)//map中的频点在 mTunerDevVec 中如果有一个没有 return false
				return false;
		}
		ite = mapFreqAndServID.begin();
		for (ite; ite != mapFreqAndServID.end(); ite++)
		{
			bool bRet = false;
			ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
			string freq = (*ite).first;
			std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
			for (; iter!=mTunerDevVec.end(); iter++)
			{
				if(dvbtype == iter->e_tunerdvbtype)
				{
					if (freq == iter->s_freq)
					{
						vector<string> vServID = (*ite).second;
						string sServList = iter->s_servidList;
						int iDevCamProNum = StrUtil::Str2Int(iter->s_devicecampronum);
						int iUsedServIDNum = 0;
						CHANNELINFOMGR::instance()->GetServIdNumfromServIdList(iUsedServIDNum, iter->s_servidList);
						if ((iDevCamProNum - iUsedServIDNum) < vServID.size())
						{//新加的节目个数 超出了 CAM 卡的解扰个数 
							bRet = true;
							break;
						}
					}
				}
			}
			if (bRet)//
				return false;
		}
	}

	return true;
}

int PropManager::GetTunerDevNumByDVBType(eDVBType dvbType)
{
	int iTunerNum = 0;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(iter->e_tunerdvbtype == dvbType)
		{
			iTunerNum++;
		}
	}
	return iTunerNum;
}



bool PropManager::GetDevID(int& devid,eDVBType tunerDVB,int priority,string freq,string servid,string taskid,bool bIsHD)
{
	//
	if ((freq == strHDMIFreq)||(freq.find("Channel")!=-1))
	{
		tunerDVB = HDMI;
	}
	//
	bool ret = false;
	int tunid = -1,codid = -1;
	bool ISok = false;
	if(devid < 0)//自动分配通道
	{
		if(GetUnUseDev(codid,tunid,tunerDVB,freq,servid,priority,bIsHD))
		{
			ret = true;
		}
		else if(GetDevFromPriority(codid,tunid,tunerDVB,freq,servid,priority,bIsHD))
		{
			ret = true;
		}
	}
	else
	{
		if(GetDevFromPriorityDevID(devid,tunid,tunerDVB,freq,servid,priority,bIsHD))
		{
			codid = devid;
			ret = true;
		}
	}
	if(ret)
	{
		devid = codid;
		sDevLinkInfo DevLink;
		DevLink.Priority = StrUtil::Int2Str(priority);
		DevLink.s_coderdeviceid = StrUtil::Int2Str(codid);
		DevLink.s_tunerdeviceid = StrUtil::Int2Str(tunid);
		DevLink.Taskid = taskid;
		DevLink.e_tunerdvbtype = tunerDVB;
		msDevLinkInfoVec.push_back(DevLink);
	}
	return ret;
}
bool PropManager::FreeSetCoderDev(int coderdevid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(coderdevid == StrUtil::Str2Int(iter->s_coderdeviceid))
		{
			if(iter->s_maincoderdeviceid.size()<=0)
			{
				DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),iter->b_ishd);
			}
			//
			iter->s_spro = "";
			iter->s_freq = "";
			iter->s_desc = "";
			iter->b_user = false;
			iter->s_maincoderdeviceid = "";
			break;
		}
	}
	return true;
}
bool PropManager::FreeSetTunerDev(int tundevid,eDVBType tunerDVB,string freq,string servid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator titer = mTunerDevVec.begin();
	for (; titer!=mTunerDevVec.end(); titer++)
	{
		if((StrUtil::Str2Int(titer->s_tunerdeviceid) == tundevid)&&(tunerDVB == titer->e_tunerdvbtype))
		{
			if(((titer->vecAllServid.size()==1)&&(servid.size()>0))||(freq == "All")||((titer->vecAllServid.size() < 1)&&(servid.size()==0)))
			{
				titer->vecAllServid.clear();
				titer->b_user = false;
				titer->s_freq = "";
				titer->s_servidList = "";
				titer->s_desc = "";
			}
			else if(titer->vecAllServid.size()>1)
			{
				std::vector<string>::iterator iter = titer->vecAllServid.begin();
				for(; iter!=titer->vecAllServid.end(); iter++)
				{
					if(servid == (*iter))
					{
						titer->vecAllServid.erase(iter);
						break;
					}
				}
				//
				iter = titer->vecAllServid.begin();
				titer->s_servidList = "";
				for(; iter!=titer->vecAllServid.end(); iter++)
				{
					if(!(CHANNELINFOMGR::instance()->FindServIdInServIdList((*iter), titer->s_servidList)))
					{
						if(titer->s_servidList.size()>0)
						{
							titer->s_servidList += "_";
							titer->s_servidList += (*iter);
						}
						else
						{
							titer->s_servidList = (*iter);
						}
					}
				}
				//
				if(servid.size()==0)//指标测量
				{
					string quaflag = "信道指标测量中...";
					int quaflagsize = quaflag.size();
					int pos = titer->s_desc.find(quaflag);
					int des_size = titer->s_desc.size();
					if(pos != -1)
					{
						if(pos == 0)
						{
							if(des_size > quaflagsize)
							{
								string temp = titer->s_desc.substr(quaflagsize+1,titer->s_desc.size()-quaflagsize-1);
								titer->s_desc = temp;
							}
							else
							{
								titer->s_desc = "";
							}
						}
						else
						{
							string temp = titer->s_desc.substr(0,pos-1);
							string temp1 = titer->s_desc.substr(pos+quaflagsize,titer->s_desc.size()-pos-quaflagsize);
							titer->s_desc = temp;
							titer->s_desc += temp1;
						}
					}
				}
			}
			break;
		}
	}
	return true;
}

bool PropManager::FlagCoderInfo(int coderdevid,string freq,string servid,eDVBType tunerDVB,bool bIsHD,string desc,string maincoderdeviceid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(coderdevid == StrUtil::Str2Int(iter->s_coderdeviceid))
		{
			iter->s_spro = servid;
			iter->s_freq = freq;
			iter->b_ishd = bIsHD;
			iter->s_desc = desc;
			iter->e_tunerdvbtype = tunerDVB;
			iter->s_maincoderdeviceid = maincoderdeviceid;
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::SetTunerinfo(int tundevid,eDVBType tunerDVB,string freq,string servid,string desc)
{
	bool tret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator titer = mTunerDevVec.begin();
	for (; titer!=mTunerDevVec.end(); titer++)
	{
		if((StrUtil::Str2Int(titer->s_tunerdeviceid) == tundevid)&&(titer->e_tunerdvbtype == tunerDVB))
		{
			if((freq != titer->s_freq)||(freq == "All"))
			{
				titer->b_user = true;
				titer->s_freq = freq;
				if(freq == "All")
				{
					titer->s_servidList = "All";
				}
				else if(servid.size()>0)
				{
					if(CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid))
					{
						titer->s_servidList = servid;
					}
					if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
					{
						titer->vecAllServid.push_back(servid);
					}
				}
				else
				{
					titer->vecAllServid.clear();
					titer->s_servidList = "";
				}
				tret = true;
				titer->s_desc = desc;
			}
			else
			{
				if(servid.size()>0)
				{
					if((!(CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid)))&&(titer->s_freq == freq))//清流直接返回
					{
						if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
						{
							titer->vecAllServid.push_back(servid);
						}
						tret = true;
						if(titer->s_desc.find(desc) == -1)
						{
							titer->s_desc += "+"+desc;
						}
					}
					else if((CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid))&&(titer->s_freq == freq))//扰流
					{
						if(CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->s_servidList))
						{
							if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
							{
								titer->vecAllServid.push_back(servid);
							}
							tret = true;
							if(titer->s_desc.find(desc) == -1)
							{
								titer->s_desc += "+"+desc;
							}
							break;
						}
						else
						{
							int servidnum = 0;
							CHANNELINFOMGR::instance()->GetServIdNumfromServIdList(servidnum,titer->s_servidList);
							if(StrUtil::Str2Int(titer->s_devicecampronum) > servidnum)
							{
								if(titer->s_servidList.size()>0)
								{
									titer->s_servidList += "_";
									titer->s_servidList += servid;
								}
								else
								{
									titer->s_servidList = servid;
								}
								if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
								{
									titer->vecAllServid.push_back(servid);
								}
								if(titer->s_desc.find(desc) == -1)
								{
									titer->s_desc += "+"+desc;
								}
								tret = true;
							}
						}
					}
				}
				else
				{
					if(titer->s_desc.find(desc) == -1)
					{
						titer->s_desc += "+"+desc;
					}
				}
			}
			break;
		}
	}
	return tret;
}
bool PropManager::SetExistCmd(int tundevid,eDVBType tunerDVB,string cmdinfo)
{
	bool tret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator titer = mTunerDevVec.begin();
	for (; titer!=mTunerDevVec.end(); titer++)
	{
		if((StrUtil::Str2Int(titer->s_tunerdeviceid) == tundevid)&&(titer->e_tunerdvbtype == tunerDVB))
		{
			titer->s_ExistCmd = cmdinfo;
			tret = true;
			break;
		}
	}
	return tret;
}

bool PropManager::SetCoderLinkTunerinfo(int coderdevid,int tundevid,eDVBType tunerDVB,string freq,string servid,bool bIsHD,string bps,string desc)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(coderdevid == StrUtil::Str2Int(iter->s_coderdeviceid))
		{
			bool cret = false;
			//if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				cret = true;
			}
			//else
			//{
			//	if(iter->b_user)
			//	{
			//		if(bIsHD==iter->b_ishd)
			//		{
			//			cret = true;
			//		}
			//		else
			//		{
			//			DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),iter->b_ishd);
			//			cret = IncMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
			//		}
			//	}
			//	else
			//	{
			//		cret = IncMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
			//	}
			//}

			if(cret)
			{
				bool tret = false;
				ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
				std::vector<TunerDev>::iterator titer = mTunerDevVec.begin();
				for (; titer!=mTunerDevVec.end(); titer++)
				{
					if((StrUtil::Str2Int(titer->s_tunerdeviceid) == tundevid)&&(titer->e_tunerdvbtype == tunerDVB))
					{
						if(freq != titer->s_freq)
						{
							titer->b_user = true;
							titer->s_freq = freq;
							if(CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid))
							{
								titer->s_servidList = servid;
							}
							if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
							{
								titer->vecAllServid.push_back(servid);
							}
							tret = true;
							titer->s_desc = desc;
						}
						else
						{
							if((!(CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid)))&&(titer->s_freq == freq))//清流直接返回
							{
								if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
								{
									titer->vecAllServid.push_back(servid);
								}
								tret = true;
								if(titer->s_desc.find(desc) == -1)
								{
									titer->s_desc += "+"+desc;
								}
							}
							else if((CHANNELINFOMGR::instance()->IsEncrypt(titer->e_tunerdvbtype,freq,servid))&&(titer->s_freq == freq))//扰流
							{
								if(CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->s_servidList))
								{
									if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
									{
										titer->vecAllServid.push_back(servid);
									}
									tret = true;
									if(titer->s_desc.find(desc) == -1)
									{
										titer->s_desc += "+"+desc;
									}
									break;
								}
								else
								{
									int servidnum = 0;
									CHANNELINFOMGR::instance()->GetServIdNumfromServIdList(servidnum,titer->s_servidList);
									if(StrUtil::Str2Int(titer->s_devicecampronum) >= servidnum)
									{
										if(titer->s_servidList.size()>0)
										{
											titer->s_servidList += "_";
											titer->s_servidList += servid;
										}
										else
										{
											titer->s_servidList = servid;
										}
										if(!CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, titer->vecAllServid))
										{
											titer->vecAllServid.push_back(servid);
										}
										if(titer->s_desc.find(desc) == -1)
										{
											titer->s_desc += "+"+desc;
										}
									}
									else
									{
										cout<<"!!!注意解扰节目超过路数..."<<endl;
									}
									tret = true;
								}
							}
						}
						break;
					}
				}
				if(tret)
				{
					iter->b_user = true;
					iter->s_spro = servid;
					iter->e_tunerdvbtype = tunerDVB;
					iter->s_freq = freq;
					iter->b_ishd = bIsHD;
					iter->s_rate = bps;
					iter->s_desc = desc;
					ret = true;
				}
				else
				{
					DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
					iter->b_user = false;
				}
			}
			else
			{
				cout<<"没有资源"<<endl;
			}
			break;
		}
	}
	return ret;
}
bool PropManager::GetUnUseDev(int& coderdevid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(IncMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD))
		{
			if((!(iter->b_user))&&(!IsUnUseRecDev(1,UNKNOWN,StrUtil::Str2Int(iter->s_coderdeviceid))))//实时视频用 轮播辅助使用通道不分配
			{
				bool bfindtuner = false;
				//
				int tunid = -1;
				eDVBType edvtype;
				if(GetTunerDevFromFreq(tunid,edvtype,iter->s_monitorindex,freq,servid))
				{
					if(edvtype == tunerDVB)
					{
						bfindtuner = true;
					}
					else
					{
						cout<<"非常奇怪-频点一样类型不一样:"<<freq<<"-"<<OSFunction::GetStrDVBType(tunerDVB)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
					}
				}
				else if(GetUnUseTunerDev(tunid,tunerDVB,iter->s_monitorindex,freq,servid))
				{
					bfindtuner = true;
				}
				else if(GetTunerDevFromPriority(tunid,tunerDVB,iter->s_monitorindex,priority,freq,servid))
				{
					bfindtuner = true;
				}
				//
				if(bfindtuner)
				{
					iter->b_user = true;
					iter->s_spro = servid;
					iter->b_ishd = bIsHD;
					iter->s_freq = freq;
					tundevid = tunid;
					coderdevid = StrUtil::Str2Int(iter->s_coderdeviceid);
					ret = true;
					break;
				}
				else
				{
					DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
				}
			}
			else
			{
				DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
			}
		}
	}
	return ret;
}
bool PropManager::GetDevFromPriority(int& coderdevid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD)//1.替换权限是小于此权限资源
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	for (; iter!=msDevLinkInfoVec.end(); iter++)
	{
		int pri = StrUtil::Str2Int(iter->Priority);
		if(pri < priority)
		{
			CoderDev cdev;
			PROPMANAGER::instance()->GetCoderInfo(StrUtil::Str2Int(iter->s_coderdeviceid),cdev);
			//DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),cdev.b_ishd);
			DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
			if(IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD))
			{
				//
				bool bfindtuner = false;
				//
				int tunid = -1;
				eDVBType edvtype;
				if(GetTunerDevFromFreq(tunid,edvtype,cdev.s_monitorindex,freq,servid))
				{
					if(edvtype == tunerDVB)
					{
						bfindtuner = true;
					}
					else
					{
						cout<<"1非常奇怪-频点一样类型不一样:"<<freq<<"-"<<OSFunction::GetStrDVBType(tunerDVB)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
					}
				}
				else if(GetUnUseTunerDev(tunid,tunerDVB,cdev.s_monitorindex,freq,servid))
				{
					bfindtuner = true;
				}
				else if(GetTunerDevFromPriority(tunid,tunerDVB,cdev.s_monitorindex,priority,freq,servid))
				{
					bfindtuner = true;
				}
				//
				if(bfindtuner)
				{
					coderdevid = StrUtil::Str2Int(iter->s_coderdeviceid);
					tundevid = tunid;
					ret = true;
					//
					ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
					std::vector<CoderDev>::iterator coditer = mCoderDevVec.begin();
					for (; coditer!=mCoderDevVec.end(); coditer++)
					{
						if(StrUtil::Str2Int(coditer->s_coderdeviceid) == coderdevid)
						{
							coditer->s_spro = servid;
							coditer->b_ishd = bIsHD;
							coditer->s_freq = freq;
							break;
						}
					}
					//
					break;
				}
				else
				{
					DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
					//IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),cdev.b_ishd);
					IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
				}
			}
		}
	}
	return ret;
}
bool PropManager::GetDevFromPriorityDevID(int devid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD)//指定通道任务判断
{
	bool ret = false;
	if(IsDeviceAvaiable(devid))
	{
		bool bfind = false;
		std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
		for (; iter!=msDevLinkInfoVec.end(); iter++)
		{
			if(devid == StrUtil::Str2Int(iter->s_coderdeviceid))
			{
				bfind = true;
				int pri = StrUtil::Str2Int(iter->Priority);
				if(priority > pri)
				{
					CoderDev cdev;
					PROPMANAGER::instance()->GetCoderInfo(StrUtil::Str2Int(iter->s_coderdeviceid),cdev);
					//
					//DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),cdev.b_ishd);
					DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
					if(IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD))
					{
						bool bfindtuner = false;
						//
						int tunid = -1;
						eDVBType edvtype;
						if(GetTunerDevFromFreq(tunid,edvtype,cdev.s_monitorindex,freq,servid))
						{
							if(edvtype == tunerDVB)
							{
								bfindtuner = true;
							}
							else
							{
								cout<<"2非常奇怪-频点一样类型不一样:"<<freq<<"-"<<OSFunction::GetStrDVBType(tunerDVB)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
							}
						}
						else if(GetUnUseTunerDev(tunid,tunerDVB,cdev.s_monitorindex,freq,servid))
						{
							bfindtuner = true;
						}
						else if(GetTunerDevFromPriority(tunid,tunerDVB,cdev.s_monitorindex,priority,freq,servid))
						{
							bfindtuner = true;
						}
						//
						if(bfindtuner)
						{
							tundevid = tunid;
							ret = true;
							//
							ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
							std::vector<CoderDev>::iterator coditer = mCoderDevVec.begin();
							for (; coditer!=mCoderDevVec.end(); coditer++)
							{
								if(StrUtil::Str2Int(coditer->s_coderdeviceid) == devid)
								{
									coditer->s_spro = servid;
									coditer->b_ishd = bIsHD;
									coditer->s_freq = freq;
									break;
								}
							}
							//
							break;
						}
						else
						{
							DecMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
							//IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),cdev.b_ishd);
							IncMonitorHDSD(StrUtil::Str2Int(cdev.s_monitorindex),bIsHD);
						}
					}
				}
			}
		}
		if(!bfind)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
			std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
			for (; iter!=mCoderDevVec.end(); iter++)
			{
				if(StrUtil::Str2Int(iter->s_coderdeviceid) == devid)
				{
					//
					if(IncMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD))
					{
						bool bfindtuner = false;
						//
						int tunid = -1;
						eDVBType edvtype;
						if(GetTunerDevFromFreq(tunid,edvtype,iter->s_monitorindex,freq,servid))
						{
							if(edvtype == tunerDVB)
							{
								bfindtuner = true;
							}
							else
							{
								cout<<"3非常奇怪-频点一样类型不一样:"<<freq<<"-"<<OSFunction::GetStrDVBType(tunerDVB)<<"**"<<OSFunction::GetStrDVBType(edvtype)<<endl;
							}
						}
						else if(GetUnUseTunerDev(tunid,tunerDVB,iter->s_monitorindex,freq,servid))
						{
							bfindtuner = true;
						}
						else if(GetTunerDevFromPriority(tunid,tunerDVB,iter->s_monitorindex,priority,freq,servid))
						{
							bfindtuner = true;
						}
						//
						if(bfindtuner)
						{
							iter->b_user = true;
							iter->s_spro = servid;
							iter->s_freq = freq;
							tundevid = tunid;
							ret = true;
						}
						else
						{
							DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),bIsHD);
						}
					}
				}
			}
		}
	}
	return ret;
}

bool PropManager::GetUnUseTunerDev(int& devid,eDVBType tunerDVB,string codermntindex,string freq,string servid)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((iter->s_monitorindex == codermntindex)&&(iter->e_tunerdvbtype == tunerDVB))
		{
			if((!(iter->b_user))&&(!IsUnUseRecDev(0,tunerDVB,StrUtil::Str2Int(iter->s_tunerdeviceid))))//实时视频用 频道扫描用 轮播辅助等不分配
			{
				//
				iter->b_user = true;
				iter->s_freq = freq;
				devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
				if(CHANNELINFOMGR::instance()->IsEncrypt(iter->e_tunerdvbtype,freq,servid))
				{
					iter->s_servidList = servid;
				}
				ret = true;
				break;
			}
		}
	}
	return ret;
}
bool PropManager::GetTunerDevFromFreq(int& devid,eDVBType& tunerDVB,string codermntindex,string freq,string servid)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(codermntindex == "quality")
		{
			if(iter->s_freq == freq)
			{
				devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
				tunerDVB = iter->e_tunerdvbtype;
				ret = true;
				break;
			}
		}
		else if(iter->s_monitorindex == codermntindex)
		{
			if((!(CHANNELINFOMGR::instance()->IsEncrypt(iter->e_tunerdvbtype,freq,servid)))&&(iter->s_freq == freq))//清流直接返回
			{
				devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
				tunerDVB = iter->e_tunerdvbtype;
				ret = true;
				break;
			}
			else if((CHANNELINFOMGR::instance()->IsEncrypt(iter->e_tunerdvbtype,freq,servid))&&(iter->s_freq == freq))//扰流
			{
				if(CHANNELINFOMGR::instance()->FindServIdInServIdList(servid, iter->s_servidList))
				{
					devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
					tunerDVB = iter->e_tunerdvbtype;
					ret = true;
					break;
				}
				else
				{
					int servidnum = 0;
					CHANNELINFOMGR::instance()->GetServIdNumfromServIdList(servidnum,iter->s_servidList);
					if(StrUtil::Str2Int(iter->s_devicecampronum) > servidnum)
					{
						if(iter->s_servidList.size()>0)
						{
							iter->s_servidList += "_";
							iter->s_servidList += servid;
						}
						else
						{
							iter->s_servidList = servid;
						}
						devid = StrUtil::Str2Int(iter->s_tunerdeviceid);
						tunerDVB = iter->e_tunerdvbtype;
						ret = true;
						break;
					}
				}
			}
		}
	}
	return ret;
}
bool PropManager::GetTunerDevFromPriority(int& devid,eDVBType tunerDVB,string codermntindex,int priority,string freq,string servid)//tuner存在共用要找单个资源的最大权限1.获取所有小于此权限资源 2.遍历每个资源获取每个资源的最大权限 3.替换最大权限是最小值的
{
	bool ret = false;
	int devpri[100];
	memset(devpri,-1,sizeof(int)*100);
	std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
	for (; DevLinkiter!=msDevLinkInfoVec.end(); DevLinkiter++)
	{
		int tid = StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid);
		int pri = StrUtil::Str2Int(DevLinkiter->Priority);
		if((pri > devpri[tid])&&(tunerDVB == DevLinkiter->e_tunerdvbtype))
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
			TunerDev tdev;
			PROPMANAGER::instance()->GetTunerInfo(i,tunerDVB,tdev);
			if((tdev.s_monitorindex == codermntindex)&&(tunerDVB == tdev.e_tunerdvbtype))
			{
				iminpri = devpri[i];
				did = i;
				ret = true;
			}
		}
	}
	devid = did;
	//
	if(ret)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
		std::vector<TunerDev>::iterator tuniter = mTunerDevVec.begin();
		for (; tuniter!=mTunerDevVec.end(); tuniter++)
		{
			if((StrUtil::Str2Int(tuniter->s_tunerdeviceid) == devid)&&(tunerDVB == tuniter->e_tunerdvbtype))
			{
				string servlist = "";
				if(tuniter->s_freq == freq)
				{
					string existservlist;
					GetExistServListFromFreqInTunerDev(freq,codermntindex,existservlist);
					if(CHANNELINFOMGR::instance()->IsEncrypt(tuniter->e_tunerdvbtype,freq,servid))
					{
						CHANNELINFOMGR::instance()->GetServIdListByFreq(DVBC,freq,servid,existservlist,tuniter->s_devicecampronum,servlist);
					}
					else
					{
						servlist = existservlist;
					}
				}
				else
				{
					if(CHANNELINFOMGR::instance()->IsEncrypt(tuniter->e_tunerdvbtype,freq,servid))
					{
						servlist = servid;
					}
				}
				tuniter->s_freq = freq;
				tuniter->s_servidList = servlist;
				break;
			}
		}
	}
	//
	return ret;
}
bool PropManager::GetExistServListFromFreqInTunerDev(string freq,string codermntindex,std::string& ServIdList)//记录同频点下已经设置过的节目
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((iter->s_freq == freq)&&(iter->b_user))
		{
			if(ServIdList.size()<=0)
			{
				ServIdList = iter->s_servidList;
			}
			else
			{
				ServIdList += "_";
				ServIdList += iter->s_servidList;
			}
			ret = true;
		}
	}
	return ret;
}
bool PropManager::GetTunerDevFromCoderDev(int& tunedevid,eDVBType& tunerDVB,int codedevid)
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	if(iter!=msDevLinkInfoVec.end())
	{
		for (; iter!=msDevLinkInfoVec.end(); iter++)
		{
			int cid = StrUtil::Str2Int(iter->s_coderdeviceid);
			if(cid == codedevid)
			{
				tunedevid = StrUtil::Str2Int(iter->s_tunerdeviceid);
				tunerDVB = iter->e_tunerdvbtype;
				ret = true;
			}
		}
	}
	return ret;
}
bool PropManager::GetCoderDevFromTunerDev(vector<int>& vCoderDev, int iTunerId)
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	if(iter!=msDevLinkInfoVec.end())
	{
		for (; iter!=msDevLinkInfoVec.end(); iter++)
		{
			int tid = StrUtil::Str2Int(iter->s_tunerdeviceid);
			if(tid == iTunerId)
			{
				int icoderId = StrUtil::Str2Int(iter->s_coderdeviceid);
				vCoderDev.push_back(icoderId);
				ret = true;
			}
		}
	}
	return ret;
}

int PropManager::FreeDev(string freq,string servid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	int coderdevid = -1;
	bool ret = false;
	string coderid;
	eDVBType tunerDVB;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(iter->s_spro==servid)
		{
			std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
			for (; DevLinkiter!=msDevLinkInfoVec.end();DevLinkiter++)
			{
				if(iter->s_coderdeviceid == DevLinkiter->s_coderdeviceid)
				{
					TunerDev tdev;
					PROPMANAGER::instance()->GetTunerInfo(StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid),DevLinkiter->e_tunerdvbtype,tdev);
					if(tdev.s_freq == freq)
					{
						tunerDVB = DevLinkiter->e_tunerdvbtype;
						ret = true;
						break;
					}
				}
			}
			if(ret)
			{
				coderid = iter->s_coderdeviceid;
				break;
			}
		}
	}
	if(ret)
	{
		coderdevid = StrUtil::Str2Int(coderid);
		FreeDevLink(0,tunerDVB,coderdevid);
	}
	return coderdevid;
}
bool PropManager::FreeCoderDev(int devid)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	std::vector<CoderDev>::iterator iter = mCoderDevVec.begin();
	for (; iter!=mCoderDevVec.end(); iter++)
	{
		if(StrUtil::Str2Int(iter->s_coderdeviceid) == devid)
		{
			iter->b_user = false;
			iter->s_spro = "";
			DecMonitorHDSD(StrUtil::Str2Int(iter->s_monitorindex),iter->b_ishd);
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::FreeTunerDev(eDVBType tunerDVB,int devid)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((StrUtil::Str2Int(iter->s_tunerdeviceid) == devid)&&(tunerDVB == iter->e_tunerdvbtype))
		{
			iter->b_user = false;
			iter->s_freq = "";
			iter->s_servidList = "";
			iter->vecAllServid.clear();
			ret = true;
			break;
		}
	}
	return ret;
}
bool PropManager::DeleteTunerDevServId(eDVBType tunerDVB, int tunerId, string sServId)
{
	if (sServId == "")
		return false;
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((StrUtil::Str2Int(iter->s_tunerdeviceid) == tunerId)&&(tunerDVB == iter->e_tunerdvbtype))
		{
			string sServList = iter->s_servidList;
			int ipos = sServList.find(sServId);
			//把ServList 中的 sServId 剔除
			if (ipos == 0)
			{
				if (sServList == sServId)
				{
					iter->s_servidList = "";
					iter->vecAllServid.clear();
				}
				else
				{
					string slist = sServList.substr(sServId.length()+1, sServList.length());
					iter->s_servidList = slist;
					std::vector<string>::iterator vsiter = iter->vecAllServid.begin();
					for(; vsiter!=iter->vecAllServid.end(); vsiter++)
					{
						if(sServId == (*vsiter))
						{
							iter->vecAllServid.erase(vsiter);
							break;
						}
					}
				}
			}
			else if (ipos > 0)
			{
				string slist = sServList.substr(0, ipos-1);
				slist += sServList.substr(ipos+sServId.length(), sServList.length());
				iter->s_servidList = slist;
				std::vector<string>::iterator vsiter = iter->vecAllServid.begin();
				for(; vsiter!=iter->vecAllServid.end(); vsiter++)
				{
					if(sServId == (*vsiter))
					{
						iter->vecAllServid.erase(vsiter);
						break;
					}
				}
			}
			ret = true;
			break;
		}
	}
	return ret;
}


bool PropManager::FreeDevLink(string taskid)
{
	bool ret = false;
	eDVBType tunerDVB;
	std::vector<int> checktuneridlist;
	std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
	for (; DevLinkiter!=msDevLinkInfoVec.end();)
	{
		if(taskid == DevLinkiter->Taskid)
		{
			tunerDVB = DevLinkiter->e_tunerdvbtype;
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
			if(!(TunerDevIsInDevLink(tunerDVB,*tuniter)))
			{
				FreeTunerDev(tunerDVB,*tuniter);
			}
		}
	}
	//
	return ret;
}
bool PropManager::FreeRecTunerCoder(eDVBType tunerDVB)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> tguard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if((iter->s_desc.find("录像") != -1)&&(iter->e_tunerdvbtype == tunerDVB))
		{
			iter->b_user = false;
			iter->s_freq = "";
			iter->s_servidList = "";
			iter->vecAllServid.clear();
		}
		ret = true;
	}
	ACE_Guard<ACE_Thread_Mutex> cguard(g_CoderDevVecMutex);
	std::vector<CoderDev>::iterator icter = mCoderDevVec.begin();
	for (; icter!=mCoderDevVec.end(); icter++)
	{
		if((icter->s_desc.find("录像") != -1)&&(icter->e_tunerdvbtype == tunerDVB))
		{
			icter->b_user = false;
			icter->s_spro = "";
			icter->s_freq = "";
			DecMonitorHDSD(StrUtil::Str2Int(icter->s_monitorindex),icter->b_ishd);
			//
		}
		ret = true;
	}
	return ret;
}

bool PropManager::FreeDevLink(int type,eDVBType tunerDVB,int devid)
{
	bool ret = false;
	std::vector<int> checktuneridlist;
	std::vector<sDevLinkInfo>::iterator DevLinkiter = msDevLinkInfoVec.begin();
	for (; DevLinkiter!=msDevLinkInfoVec.end();)
	{
		if(devid == StrUtil::Str2Int(DevLinkiter->s_coderdeviceid)&&(type == 0))
		{
			//
			int tunid = StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid);
			tunerDVB = DevLinkiter->e_tunerdvbtype;
			CoderDev cDev;
			GetCoderInfo(StrUtil::Str2Int(DevLinkiter->s_coderdeviceid), cDev);
			DeleteTunerDevServId(tunerDVB, tunid, cDev.s_spro);

			FreeCoderDev(StrUtil::Str2Int(DevLinkiter->s_coderdeviceid));
			//
			DevLinkiter = msDevLinkInfoVec.erase(DevLinkiter);
			if(!(TunerDevIsInDevLink(tunerDVB,tunid)))
			{
				FreeTunerDev(tunerDVB,tunid);
			}
			ret = true;
			break;
		}
		else if(devid == StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid)&&(type == 1)&&(tunerDVB == DevLinkiter->e_tunerdvbtype))
		{
			FreeCoderDev(StrUtil::Str2Int(DevLinkiter->s_coderdeviceid));
			FreeTunerDev(tunerDVB,StrUtil::Str2Int(DevLinkiter->s_tunerdeviceid));
			//
			DevLinkiter = msDevLinkInfoVec.erase(DevLinkiter);
			ret = true;
		}
		else
		{ 
			DevLinkiter++;
		}
	}
	return ret;
}
bool PropManager::TunerDevIsInDevLink(eDVBType tunerDVB,int tundevid)
{
	bool ret = false;
	std::vector<sDevLinkInfo>::iterator iter = msDevLinkInfoVec.begin();
	for (; iter!=msDevLinkInfoVec.end();iter++)
	{
		if((tundevid == StrUtil::Str2Int(iter->s_tunerdeviceid))&&(iter->e_tunerdvbtype == tunerDVB))
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
		if(path.find("/") != -1)
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
		else if(path.find("\\") != -1)
		{
			if (path.at( path.length()-1 ) != '\\')
			{
				path += "\\";
			}
			str = path;
			while(pos != path.length()-1)
			{
				pos = path.find("\\", ++pos);
				temPath = path.substr(0,pos);
				velc.push_back(temPath);
			}
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
	
	for (int dirIndex=1; dirIndex<pathvelc.size(); dirIndex++)//首级目录不建
	{
		if( INVALID_FILE_ATTRIBUTES == GetFileAttributes(pathvelc[dirIndex].c_str()))
		{
			CreateDirectory(pathvelc[dirIndex].c_str(), NULL);
		}
	}
	return;
}


//硬件相关信息查询
//bool PropManager::GetAllDeviceList( std::list<int>& devicedlist )
//{
//	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
//	{
//		devicedlist.push_back((*pDeviceinfo).first);
//	}
//	return true;
//}
//bool PropManager::GetDVBDeviceList(eDVBType dvbtype,std::list<int>& devicedlist)
//{
//	string devicetype = OSFunction::GetStrDVBType(dvbtype);
//
//	for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
//	{
//		if (devicetype == pDeviceinfo->second.devicetype)
//		{
//			devicedlist.push_back((*pDeviceinfo).first);
//		}
//	}
//	return true;
//}
bool PropManager::GetMER_BERDeviceList(eDVBType dvbtype,std::list<int>& devicedlist)//目前一种类型仅适合配置1个通道DVBC:2;CTTB:1
{
	bool ret=true;
	vector<Quality_DevInfo>::iterator iter = m_VecQuaDevInfo.begin();
	for (iter; iter!= m_VecQuaDevInfo.end(); iter++)
	{
		if (iter->s_devType == "Mer-Ber")
		{
			map<string, string>::iterator iter_map;
			if (dvbtype == DVBC)
			{
				iter_map = iter->mapDvbTypeChls.find(string("DVBC"));
				if (iter_map != iter->mapDvbTypeChls.end())
				{
					devicedlist.push_back(StrUtil::Str2Int(iter_map->second));//如果多通道StrUtil::Str2Int会出问题,如:DVBC:2,0;CTTB:1,3
				}
			}
			else if (dvbtype == CTTB)
			{
				iter_map = iter->mapDvbTypeChls.find(string("CTTB"));
				if (iter_map != iter->mapDvbTypeChls.end())
				{
					devicedlist.push_back(StrUtil::Str2Int(iter_map->second));//如果多通道StrUtil::Str2Int会出问题,如:DVBC:2,0;CTTB:1,3
				}
			}
			break;
		}
	}
	return ret;
}
bool PropManager::GetTaskDeviceList(string taskType,string& strdlist)
{
	bool ret=false;
	for (std::map<std::string,std::string >::iterator pTaskinfo=mTaskInfo.begin();pTaskinfo!=mTaskInfo.end();pTaskinfo++)
	{
		if ((*pTaskinfo).first == taskType)
		{
			strdlist = (*pTaskinfo).second;
			ret=true;
			break;
		}
	}
	return ret;
}
bool PropManager::GetTaskDeviceList( string taskType,eDVBType dvbtype,std::list<int>& taskdeviceidlist )
{
	bool ret=true;
	string devicetype = OSFunction::GetStrDVBType(dvbtype);

	for (std::map<std::string,std::string >::iterator pTaskinfo=mTaskInfo.begin();pTaskinfo!=mTaskInfo.end();pTaskinfo++)
	{
		if ((*pTaskinfo).first == taskType)
		{
			std::string strdev = (*pTaskinfo).second;
			int pos = strdev.find(devicetype);
			if(pos != -1)
			{
				string strdevtem = strdev.substr(pos+devicetype.size()+1,strdev.size()-pos-devicetype.size()-1);
				int pt = strdevtem.find(";");
				if(pt==-1)
				{
					for(int i=0;i<strdevtem.size();i++)
					{
						char tem[2] = {0,0};
						tem[0] = strdevtem.at(i);
						string st = tem;
						if(tem[0] != ',')
						{
							taskdeviceidlist.push_back(StrUtil::Str2Int(st));
						}
					}
				}
				else
				{
					string strdevtemsub = strdevtem.substr(0,pt);
					for(int i=0;i<strdevtemsub.size();i++)
					{
						char tem[2] = {0,0};
						tem[0] = strdevtem.at(i);
						string st = tem;
						if(tem[0] != ',')
						{
							taskdeviceidlist.push_back(StrUtil::Str2Int(string(tem)));
						}
					}
				}
				break;
			}
		}
	}
	return ret;
}

bool PropManager::GetDeviceAddress( int deviceid,string& ip,int& cmdport,int &tsport,string &cmdprotocol,string &tsprotocol )
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		ip=(*pDeviceinfo).second.TSIP;
	//		cmdport=StrUtil::Str2Int((*pDeviceinfo).second.cmdport);
	//		tsport =StrUtil::Str2Int((*pDeviceinfo).second.tsport);
	//		cmdprotocol = (*pDeviceinfo).second.cmdprotocol;
	//		tsprotocol = (*pDeviceinfo).second.tsprotocol;
	//		break;
	//	}
	//}
	return ret;
}

bool PropManager::GetDeviceCardIndex(int deviceid,int & iCardIndex)
{
	bool ret = false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret = true;
	//		iCardIndex = (*pDeviceinfo).second.CardIndex;
	//		break;
	//	}
	//}
	return ret;
}

bool PropManager::GetDeviceTSIP(int deviceid,string& ip)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		ip=(*pDeviceinfo).second.TSIP;
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceCMDIP(int deviceid,string& ip)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		ip=(*pDeviceinfo).second.CMDIP;
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceCmdPort(int deviceid,int& cmdport)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		cmdport=StrUtil::Str2Int((*pDeviceinfo).second.cmdport);
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceCmdProtocol(int deviceid,std::string& cmdprotocol)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		cmdprotocol =(*pDeviceinfo).second.cmdprotocol;
	//		break;
	//	}
	//}
	return ret;
}

bool PropManager::GetDeviceTsPort(int deviceid,unsigned int& tsport)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		tsport =StrUtil::Str2Int((*pDeviceinfo).second.tsport);
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceTunerID(int deviceid,int& tunerid)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		tunerid =StrUtil::Str2Int((*pDeviceinfo).second.tunerid);
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceLogIndex(int deviceid, int& logindex)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		logindex =StrUtil::Str2Int((*pDeviceinfo).second.logindex);
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceIndex(string & deviceid,string logindex,string devicetype)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).second.logindex==logindex &&(*pDeviceinfo).second.devicetype==devicetype)
	//	{
	//		ret=true;
	//		deviceid=StrUtil::Int2Str((*pDeviceinfo).first);
	//		break;
	//	}
	//}
	return ret;
}
bool PropManager::GetDeviceTsProtocol(int deviceid,std::string& tsprotocol)
{
	bool ret=false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=true;
	//		tsprotocol = (*pDeviceinfo).second.tsprotocol;
	//		break;
	//	}
	//}
	return ret;
}

bool PropManager::GetDeviceCardIndexbyIP(string ip,int & iCardIndex)	
{
	bool ret = false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if (ip == pDeviceinfo->second.CMDIP)
	//	{
	//		ret = true;
	//		iCardIndex =(*pDeviceinfo).second.CardIndex;
	//		break;
	//	}
	//}
	return ret;
}

bool PropManager::GetDeviceIndexbyCardIndex(int iCardIndex,int &firstIndex)
{
	bool ret = false;
	firstIndex = -1;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if (iCardIndex == pDeviceinfo->second.CardIndex)
	//	{
	//		ret = true;
	//		firstIndex= (*pDeviceinfo).first;
	//		break;
	//	}
	//}
	return ret;

}

bool PropManager::GetDeviceID(string ip,string tunerid,int & deviceid)
{
	bool ret = false;
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if (((*pDeviceinfo).second.TSIP==ip||(*pDeviceinfo).second.CMDIP==ip) && (*pDeviceinfo).second.tunerid == tunerid)
	//	{
	//		ret=true;
	//		deviceid=(*pDeviceinfo).first;
	//		break;
	//	}
	//}
	return ret;

}

bool PropManager::GetDeviceIDByIP(string ip,std::list<int> & devicedlist)
{
	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if (ip == pDeviceinfo->second.TSIP||ip == pDeviceinfo->second.CMDIP)
	//	{
	//		devicedlist.push_back((*pDeviceinfo).first);
	//	}
	//}
	return true;
}

bool PropManager::IsDeviceAvaiable( int deviceid,string tasktype,eDVBType dvbtype )
{
	bool ret=false;
	string devicetype = OSFunction::GetStrDVBType(dvbtype);

	//for (std::map<int,sChanelInfo>::iterator pDeviceinfo=mDeviceInfo.begin();pDeviceinfo!=mDeviceInfo.end();pDeviceinfo++)
	//{
	//	if ((*pDeviceinfo).first==deviceid)
	//	{
	//		ret=((*pDeviceinfo).second.devicetype==devicetype)&&(CheckDeviceInList(deviceid,mTaskInfo[tasktype]));
	//	}
	//}
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
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
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
	//map<int,sChanelInfo>::iterator ptr = mDeviceInfo.begin();
	//for (;ptr!= mDeviceInfo.end();ptr++)
	//{
	//	if ((*ptr).first == deviceid)
	//	{ 
	//		dvbtype = OSFunction::GetEnumDVBType((*ptr).second.devicetype);
	//		break;
	//	}
	//}
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
bool PropManager::CheckDeviceInList( int deviceid,string deviceList )
{
	//for (std::list<int>::iterator pList=deviceList.begin();pList!=deviceList.end();pList++)
	//{
	//	if ((*pList)==deviceid)
	//	{
	//		return true;
	//	}
	//}
	return false;
}

bool PropManager::DeleteWrongDeviceID(std::list<int>& deviceIDList)     //从任务的通道组中去掉不合理的deviceid
{
	//std::list<int>::iterator ptr = deviceIDList.begin();
	//std::map<int ,sChanelInfo>::iterator in_ptr;
	//if (deviceIDList.size()>0)
	//{  
	//	for (;ptr!=deviceIDList.end();)
	//	{ 
	//		in_ptr=mDeviceInfo.begin();
	//		for (;in_ptr!= mDeviceInfo.end();in_ptr++)
	//		{
	//			if ((*ptr) ==(*in_ptr).first)
	//			{
	//				break;
	//			}
	//		}
	//		if (in_ptr == mDeviceInfo.end())
	//		{
	//			ptr=deviceIDList.erase(ptr);
	//		}
	//		else
	//		{
	//			ptr++;
	//		}
	//	}
	//}
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
		//info.FontSize = StrUtil::Int2Str(24);
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
	//d:/tsrecord/8/
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
	//if ( mDeviceInfo.find(deviceid)==mDeviceInfo.end() )
	//	return false;

	//std::string cardip = mDeviceInfo[deviceid].CMDIP;
	//std::map<int,sChanelInfo>::iterator devIter = mDeviceInfo.begin();
	//for (; devIter!=mDeviceInfo.end(); devIter++)
	//{
	//	if ( (*devIter).second.CMDIP == cardip && (*devIter).first!=deviceid )
	//	{
	//		deviceidlist.push_back( devIter->first );
	//	}
	//}
	////该ip只有一个通道被使用
	//if ( deviceidlist.empty() )
	//	return false;

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
bool PropManager::GetAlarmUpInfoByType(const std::string& strEquType,std::string& srccode,std::string& dstcode,std::string& url)
{
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.find( strEquType );
	if ( iter != m_mapEquInfo.end() )
	{
		dstcode = iter->second->strDesCode;
		srccode = iter->second->strSrcCode;
		url = iter->second->strUrl;
		return true;
	}
	return false;
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
std::string PropManager::GetSrcCodeByType(string BZType)
{
	std::map<std::string,sEquInfo*>::iterator iter = m_mapEquInfo.find( BZType );
	if ( iter != m_mapEquInfo.end() )
	{
		return iter->second->strSrcCode;
	}
	else
	{
		return "";
	}
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
//获取 Quality_DevInfo 结构体信息
Quality_DevInfo* PropManager::GetQuaDevInfo(int Index)
{
	vector<Quality_DevInfo>::iterator iter = m_VecQuaDevInfo.begin();
	for (iter; iter!= m_VecQuaDevInfo.end(); iter++)
	{
		if (iter->i_index == Index)
		{
			return &(*iter);
		}
	}
	return NULL;
}

//校准 频点电平
bool PropManager::AdjustingFreqLevel(std::string sFreq, std::string& sLevel)
{
	if (sFreq.length() <= 0)
		return false;
	std::map<std::string, std::string>::iterator iter = m_mapFreqLevels.find(sFreq);
	if (iter != m_mapFreqLevels.end())
	{
		if ((sLevel.length() <= 0) || (StrUtil::Str2Float(sLevel) <= 0))
		{
			sLevel = iter->second;
		}
		else
		{
			m_mapFreqLevels.erase(iter);
			m_mapFreqLevels.insert(make_pair(sFreq, sLevel));	
		}
	}
	else
	{
		if ((sLevel.length() <= 0) || (StrUtil::Str2Float(sLevel) <= 0))
		{
			std::string str = "50.81";
			sLevel = str;
		}
		m_mapFreqLevels.insert(make_pair(sFreq, sLevel));	

	}

	return true;
}
std::string PropManager::GetAgentInfoSet(string AgentType)
{
	string ret;
	for(int i = 0; i < g_VecAgentSetInfo.size(); i++)
	{
		if(AgentType == g_VecAgentSetInfo[i].s_strType)
		{
			ret = g_VecAgentSetInfo[i].s_strValue;
			break;
		}
	}
	return ret;
}
//
bool PropManager::SetStandardChResourceInfo(standardChannelResource chres)
{
	string strtunerserial = "jsdvbct201709";
	string strcoderserial = "jsdvbcc201709";
	//
	sDeviceInfo sDevInfo;
	sDevInfo.s_deviceindex = StrUtil::Int2Str(chres.ucharChassis);
	sDevInfo.s_deviceip = StrUtil::Int2Str(chres.ucharIPAddr[0]);
	sDevInfo.s_deviceip += string(".");
	sDevInfo.s_deviceip += StrUtil::Int2Str(chres.ucharIPAddr[1]);
	sDevInfo.s_deviceip += string(".");
	sDevInfo.s_deviceip += StrUtil::Int2Str(chres.ucharIPAddr[2]);
	sDevInfo.s_deviceip += string(".");
	sDevInfo.s_deviceip += StrUtil::Int2Str(chres.ucharIPAddr[3]);
	sDevInfo.s_deviceport = StrUtil::Int2Str(chres.ushortPort);
	sDevInfo.s_devicecampronum = string("8");
	sDevInfo.i_HDnum = 0;
	sDevInfo.i_SDnum = 0;
	sDevInfo.s_cardindex = StrUtil::Int2Str(chres.ucharSlot);
	sDevInfo.s_manufacturer = string("金石");//待获取板卡信息实现
	sDevInfo.s_modindex = StrUtil::Int2Str(chres.ucharIPAddr[3]);
	sDevInfo.ucharAVSNeeds = chres.ucharAVSNeeds;
	sDevInfo.ucharBoardType = chres.ucharBoardType;
	sDevInfo.ucharChanNum = chres.ucharChanNum;
	sDevInfo.ucharH264Needs = chres.ucharH264Needs;
	sDevInfo.ucharH265Needs = chres.ucharH265Needs;
	sDevInfo.ucharMpeg2Needs = chres.ucharH265Needs;
	if(sDevInfo.ucharBoardType == 1)
	{
		sDevInfo.i_TcoderNum = chres.ucharChanNum;
	}
	mMonitorDevInfo.push_back(sDevInfo);
	//
	string OEM="",CardID="";
	//
	if((sDevInfo.ucharBoardType == 0)||(sDevInfo.ucharBoardType == 1))
	{
		//查询板卡基本信息
		string  MsgID ;
		DBMANAGER::instance()->GetMsgID(MsgID);
		int msgid=StrUtil::Str2Int(MsgID);
		DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
		//
		std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
		std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
		string Surl = "http://"+DeviceServerIP+":"+DeviceServerPort;
		//
		string strRet;
		string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
		cmd += "<Msg Version=\"1.0\" MsgID=\"";
		cmd += MsgID;
		cmd += "\" SrcUrl=\"";
		cmd += Surl;
		cmd += "\">";
		cmd += "<Type>GetDeviceInfo</Type>";
		cmd += "</Msg>";
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
		cout<<"发送命令地址:"<<sDevInfo.s_deviceip<<":"<<sDevInfo.s_deviceport<<endl;
		cout<<"发送命令:"<<cmd<<endl;
		pDeviceAccess->SendTaskMsg(cmd,strRet);
		cout<<"收到结果:"<<strRet<<endl;
		string sendaddress = "发送命令地址:"+sDevInfo.s_deviceip+":"+sDevInfo.s_deviceport;
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		//
		if(strRet.find("200") != -1)
		{
			time_t curt = time(0);
			string DevRetval;
			while(((time(0)-curt)<30))
			{
				DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
				if(DevRetval.size()>0)
				{
					cout<<"收到结果**:"<<DevRetval<<endl;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
					break;
				}
				else
				{
					Sleep(200);
				}
			}
			if(DevRetval.size()>0)
			{
				int spos = DevRetval.find("<OEM>");
				int epos = DevRetval.find("</OEM>");
				OEM = DevRetval.substr(spos+5,epos-spos-5);
				spos = DevRetval.find("<CardID>");
				epos = DevRetval.find("</CardID>");
				if(spos != -1 && epos != -1)
				{
					CardID = DevRetval.substr(spos+8,epos-spos-8);
				}
			}
		}
	}
	//
	if(sDevInfo.ucharBoardType == 0)
	{
		TunerDev tdev;
		tdev.s_monitorindex = sDevInfo.s_deviceindex;
		tdev.s_cardindex = sDevInfo.s_cardindex;
		for(int i=1;i<=sDevInfo.ucharChanNum;i++)
		{
			tdev.s_chanindex = StrUtil::Int2Str(i);
			tdev.s_tunerdeviceid = StrUtil::Int2Str(m_itunerdeviceid);
			tdev.b_user = false;
			tdev.s_sip = "235.";
			tdev.s_sip += StrUtil::Int2Str(chres.ucharIPAddr[1]-16);
			tdev.s_sip += string(".");
			tdev.s_sip += StrUtil::Int2Str(chres.ucharIPAddr[2]);
			tdev.s_sip += string(".");
			tdev.s_sip += tdev.s_chanindex;
			tdev.s_stsport = string("10000");
			tdev.s_trport = string("18000");
			tdev.s_290ip = "236.";
			tdev.s_290ip += StrUtil::Int2Str(chres.ucharIPAddr[1]-16);
			tdev.s_290ip += string(".");
			tdev.s_290ip += StrUtil::Int2Str(chres.ucharIPAddr[2]);
			tdev.s_290ip += string(".");
			tdev.s_290ip += tdev.s_chanindex;
			tdev.s_290port = string("20000");
			tdev.e_tunerdvbtype = DVBC;
			tdev.s_devicecampronum = sDevInfo.s_devicecampronum;
			tdev.s_manufacturer = OEM;
			tdev.s_serialnum = CardID;
			tdev.s_ICcardNO = "";
			//获取小卡信息
			string  MsgID ;
			DBMANAGER::instance()->GetMsgID(MsgID);
			int msgid=StrUtil::Str2Int(MsgID);
			DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
			//
			std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
			std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
			string Surl = "http://"+DeviceServerIP+":"+DeviceServerPort;
			//
			string strRet;
			string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
			cmd += "<Msg Version=\"1.0\" MsgID=\"";
			cmd += MsgID;
			cmd += "\" SrcUrl=\"";
			cmd += Surl;
			cmd += "\">";
			cmd += "<Type>GetICCardInfo</Type>";
			cmd += "<Data>";
			cmd += "<Channel>";
			cmd += 	StrUtil::Int2Str(i);
			cmd += "</Channel>";
			cmd += "</Data>";
			cmd += "</Msg>";
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
			cout<<"发送命令地址:"<<sDevInfo.s_deviceip<<":"<<sDevInfo.s_deviceport<<endl;
			cout<<"发送命令:"<<cmd<<endl;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			cout<<"收到结果:"<<strRet<<endl;
			string sendaddress = "发送命令地址:"+sDevInfo.s_deviceip+":"+sDevInfo.s_deviceport;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
			//
			if(strRet.find("200") != -1)
			{
				time_t curt = time(0);
				string DevRetval;
				while(((time(0)-curt)<30))
				{
					DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
					if(DevRetval.size()>0)
					{
						cout<<"收到结果**:"<<DevRetval<<endl;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
						break;
					}
					else
					{
						Sleep(200);
					}
				}
				if(DevRetval.size()>0)
				{
					int spos = DevRetval.find("<ICCard>");
					int epos = DevRetval.find("</ICCard>");
					if((spos != -1)&&(epos != -1))
					{
						tdev.s_ICcardNO = DevRetval.substr(spos+8,epos-spos-8);
					}
				}
			}
			//
			ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
			mTunerDevVec.push_back(tdev);
			m_itunerdeviceid++;
			//
		}
	}
	else if(sDevInfo.ucharBoardType == 1)
	{
		CoderDev cdev;
		cdev.s_monitorindex = sDevInfo.s_deviceindex;
		cdev.s_cardindex = sDevInfo.s_cardindex;
		cdev.s_modindex = sDevInfo.s_modindex;
		for(int i=1;i<=sDevInfo.ucharChanNum;i++)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
			cdev.s_coderdeviceid = StrUtil::Int2Str(m_icodedeviceid);
			cdev.s_coderindex = StrUtil::Int2Str(i);
			cdev.s_modcoderindex = StrUtil::Int2Str(i);
			cdev.b_user = false;
			cdev.b_ishd = false;
			cdev.s_dip = "235.";
			cdev.s_dip += StrUtil::Int2Str(chres.ucharIPAddr[1]-16);
			cdev.s_dip += string(".");
			cdev.s_dip += StrUtil::Int2Str(chres.ucharIPAddr[2]);
			cdev.s_dip += string(".");
			cdev.s_dip += StrUtil::Int2Str(i+((chres.ucharIPAddr[3]-1)*sDevInfo.ucharChanNum));
			cdev.s_dtsport = string("10000");
			cdev.s_code = "h264";
			cdev.s_ena = "1";
			cdev.s_fmt = "0";
			cdev.s_fre = "0";
			cdev.s_rate = "400";
			cdev.s_alarmip = "236.";
			cdev.s_alarmip += StrUtil::Int2Str(chres.ucharIPAddr[1]-16);
			cdev.s_alarmip += string(".");
			cdev.s_alarmip += StrUtil::Int2Str(chres.ucharIPAddr[2]);
			cdev.s_alarmip += string(".");
			cdev.s_alarmip += StrUtil::Int2Str(i+((chres.ucharIPAddr[3]-1)*sDevInfo.ucharChanNum));
			cdev.s_alarmport = string("20000");
			cdev.s_manufacturer = OEM;
			cdev.s_serialnum = CardID;
			mCoderDevVec.push_back(cdev);
			m_icodedeviceid++;
			//***
			//关闭转码
			string  MsgID ;
			DBMANAGER::instance()->GetMsgID(MsgID);
			int msgid=StrUtil::Str2Int(MsgID);
			DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
			//
			std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
			std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
			string Surl = "http://"+DeviceServerIP+":"+DeviceServerPort;
			//
			string strRet;
			string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
			cmd += "<Msg Version=\"1.0\" MsgID=\"";
			cmd += MsgID;
			cmd += "\" SrcUrl=\"http://";
			cmd += Surl;
			cmd += "\">";
			cmd += "<Type>SetOutputInfo</Type>";
			cmd += "<Data>";
			cmd += "<Channel>";
			cmd += 	cdev.s_coderindex;
			cmd += "</Channel>";
			cmd += "<TSOut>";
			cmd += "<Switch>0</Switch>";
			cmd += "</TSOut>";
			cmd += "<Monitor>";
			cmd += "<Switch>0</Switch>";
			cmd += "</Monitor>";
			cmd += "</Data>";
			cmd += "</Msg>";
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(sDevInfo.s_deviceindex),sDevInfo.s_deviceip,StrUtil::Str2Int(sDevInfo.s_deviceport));
			cout<<"发送命令地址:"<<sDevInfo.s_deviceip<<":"<<sDevInfo.s_deviceport<<endl;
			cout<<"发送命令:"<<cmd<<endl;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			cout<<"收到结果:"<<strRet<<endl;
			string sendaddress = "发送命令地址:"+sDevInfo.s_deviceip+":"+sDevInfo.s_deviceport;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
		}
	}
	//
	return true;
}

int PropManager::GetAutoModelID(int ChassisID,int Slot)
{
	int num = 1;
	for(int i=0;i<mMonitorDevInfo.size();i++)
	{
		if(StrUtil::Str2Int(mMonitorDevInfo[i].s_deviceindex) == ChassisID
			&&StrUtil::Str2Int(mMonitorDevInfo[i].s_cardindex) == Slot)
		{
			num++;
		}
	}
	return num;
}
//
 bool PropManager::GetCACardInfoByCardNo( std::string strCardNo, CACardInfo& cardInfo )
 {
	bool bRet = false;
	ACE_Guard<ACE_Thread_Mutex> guard(g_TunerDevVecMutex);
	std::vector<TunerDev>::iterator iter = mTunerDevVec.begin();
	for (; iter!=mTunerDevVec.end(); iter++)
	{
		if(strCardNo == iter->s_ICcardNO)
		{
			sDeviceInfo deviceInfo;
			PROPMANAGER::instance()->GetDeviceInfo(iter->s_monitorindex,iter->s_cardindex,"",deviceInfo);
			cardInfo.ip = deviceInfo.s_deviceip;
			cardInfo.position = iter->s_monitorindex+"_"+iter->s_cardindex+"_"+iter->s_chanindex;
			cardInfo.desc = iter->s_monitorindex+"机箱"+iter->s_cardindex+"卡槽"+iter->s_chanindex+"通道";
			break;
		}
	}
	return bRet;
 }
std::string PropManager::GetCardType(enumDVBType dvbtype)
{
	string ret = "";
	if(dvbtype==DVBC)
	{
		ret = "0";
	}
	else if(dvbtype==CTTB)
	{
		ret = "1";
	}
	else if(dvbtype==DVBS)
	{
		ret = "2";
	}
	else if(dvbtype==RADIO)
	{
		ret = "3";
	}
	else if(dvbtype==AM)
	{
		ret = "4";
	}
	return ret;
}

void PropManager::CreatPathTS()
{
	std::map<std::string, std::list<int>>::iterator pIter = mDeviceIdToPath.begin();
	for(;pIter != mDeviceIdToPath.end();pIter++)
	{
		std::list<int> m_deviceIDpath = pIter->second;
		for(std::list<int>::iterator pListIter = m_deviceIDpath.begin();pListIter!=m_deviceIDpath.end();pListIter++)
		{
			int tmpDeviceID = *pListIter;
			string shareTsFile = pIter->first + string("/") + StrUtil::Int2Str(tmpDeviceID);
			if(CreateDirectory(shareTsFile.c_str(), NULL))
			{
				std::cout<<"文件夹 ["<<shareTsFile<<"] 创建成功"<<std::endl;
			}
		}
	}
}

bool PropManager::GetCoderInfobyServiceID( int seviceid,CoderDev& CoderDevInfo )
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_CoderDevVecMutex);
	bool ret = false;
	int codersize = mCoderDevVec.size();
	for (int i=0; i<codersize; i++)
	{
		if(seviceid == StrUtil::Str2Int(mCoderDevVec[i].s_spro))
		{
			CoderDevInfo = mCoderDevVec[i];
			ret = true;
			break;
		}
	}
	return ret;
}
