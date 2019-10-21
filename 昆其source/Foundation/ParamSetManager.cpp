#include "ParamSetManager.h"

#ifdef ZONG_JU_ZHAO_BIAO

ParamSetManager::ParamSetManager()
{
	InitParamInfo();
}
ParamSetManager::~ParamSetManager()
{

	AudioParamMap.clear();
	if(mConfig!=NULL)
		delete mConfig;
}


void  ParamSetManager::InitParamInfo()
{
	mConfig=new ParamSetConfig("c:/vscttb/paramset.xml");

	StreamRate.width=mConfig->GetNodeAttribute("SteamRateSet","StreamRateParam","Width");
	StreamRate.height=mConfig->GetNodeAttribute("SteamRateSet","StreamRateParam","Height");
	StreamRate.fps=mConfig->GetNodeAttribute("SteamRateSet","StreamRateParam","Fps");
	StreamRate.bps=mConfig->GetNodeAttribute("SteamRateSet","StreamRateParam","Bps");

	HDRecordRate.width=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamHD","Width");
	HDRecordRate.height=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamHD","Height");
	HDRecordRate.fps=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamHD","Fps");
	HDRecordRate.bps=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamHD","Bps");

	SDRecordRate.width=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamSD","Width");
	SDRecordRate.height=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamSD","Height");
	SDRecordRate.fps=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamSD","Fps");
	SDRecordRate.bps=mConfig->GetNodeAttribute("RrecordParamSet","RecordParamSD","Bps");

	HDAudioParam.audioencode=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","AudioEncode");
	HDAudioParam.left=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Left");
	HDAudioParam.center=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Center");
	HDAudioParam._right=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Right");
	HDAudioParam.leftsurround=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","LeftSurround");
	HDAudioParam.rightsurround=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","RightSurround");
	HDAudioParam.subwoofer=mConfig->GetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Subwoofer");

	InfoMhp.ftpurl=mConfig->GetNodeAttribute("MHPQuery","MHP","Ftp");
	InfoMhp.username=mConfig->GetNodeAttribute("MHPQuery","MHP","UserName");
	InfoMhp.pass=mConfig->GetNodeAttribute("MHPQuery","MHP","Pass");


	XmlParser mXmlProp;
	std::string nodePath;

	mXmlProp.LoadFromFile("c:/vscttb/paramset.xml");
	std::string childpath[]={"AudioParamSet","ICInfoQuery"};
	for(int k=0;k<2;k++)
	{
		nodePath="ParamSet/"+childpath[k];
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if(node==NULL)
			return ;
		pXMLNODELIST nodeList=mXmlProp.GetNodeList(node);		
		for (int i=0;i<nodeList->Size();i++)
		{
			pXMLNODE nodeNext=mXmlProp.GetNextNode(nodeList);
			switch(k)
			{
			case 0:
				{
					string Channelid,Freq,ServiceID,VideoPID,AudioPID,Left,Center,Right,LeftSurround,RightSurround,Subwoofer;
					AudioParam TempParam;
					mXmlProp.GetAttrNode(nodeNext,"Freq",Freq);
					mXmlProp.GetAttrNode(nodeNext,"ServiceID",ServiceID);
					mXmlProp.GetAttrNode(nodeNext,"VideoPID",TempParam.videopid);
					mXmlProp.GetAttrNode(nodeNext,"AudioPID",TempParam.audiopid);

					mXmlProp.GetAttrNode(nodeNext,"Left",TempParam.left);
					mXmlProp.GetAttrNode(nodeNext,"Center",TempParam.center);
					mXmlProp.GetAttrNode(nodeNext,"Right",TempParam._right);
					mXmlProp.GetAttrNode(nodeNext,"LeftSurround",TempParam.leftsurround);
					mXmlProp.GetAttrNode(nodeNext,"RightSurround",TempParam.rightsurround);
					mXmlProp.GetAttrNode(nodeNext,"Subwoofer",TempParam.subwoofer);

					Channelid=Freq+"_"+ServiceID;
					AudioParamMap.insert(make_pair(Channelid,TempParam));
					break;
				}
			case 1:
				{
					CACardInfo  cardinfo;
					mXmlProp.GetAttrNode(nodeNext,"CardNO",cardinfo.cardno);
					mXmlProp.GetAttrNode(nodeNext,"Position",cardinfo.position);
					mXmlProp.GetAttrNode(nodeNext,"Desc",cardinfo.desc);
					mXmlProp.GetAttrNode(nodeNext,"DeviceIp",cardinfo.ip);
					vecCardInfo.push_back(cardinfo);
					break;
				}
			default:
				break;
				
			}

		}
		
	}


}
bool  ParamSetManager::SetStreamRate(const RateParam rate)
{
	 StreamRate.width=rate.width;
	 StreamRate.height=rate.height;
	 StreamRate.bps=rate.bps;
	 StreamRate.fps=rate.fps;

	mConfig->SetNodeAttribute("SteamRateSet","StreamRateParam","Width",rate.width.c_str());
	mConfig->SetNodeAttribute("SteamRateSet","StreamRateParam","Height",rate.height.c_str());
	mConfig->SetNodeAttribute("SteamRateSet","StreamRateParam","Fps",rate.fps.c_str());
	mConfig->SetNodeAttribute("SteamRateSet","StreamRateParam","Bps",rate.bps.c_str());

	return true;
}
bool  ParamSetManager::SetHDRecordRate(const RateParam rate)
{
	HDRecordRate.width=rate.width;
	HDRecordRate.height=rate.height;
	HDRecordRate.bps=rate.bps;
	HDRecordRate.fps=rate.fps;

	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamHD","Width",rate.width.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamHD","Height",rate.height.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamHD","Fps",rate.fps.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamHD","Bps",rate.bps.c_str());
	return true;
}
bool  ParamSetManager::SetSDRecordRate(const RateParam rate)
{
	SDRecordRate.width=rate.width;
	SDRecordRate.height=rate.height;
	SDRecordRate.bps=rate.bps;
	SDRecordRate.fps=rate.fps;

	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamSD","Width",rate.width.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamSD","Height",rate.height.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamSD","Fps",rate.fps.c_str());
	mConfig->SetNodeAttribute("RrecordParamSet","RecordParamSD","Bps",rate.bps.c_str());
	return true;
}
bool  ParamSetManager::SetHDAudioParam(const AudioParam param)
{
	HDAudioParam.audioencode=param.audioencode;
	HDAudioParam.left=param.left;
	HDAudioParam.center=param.center;
	HDAudioParam._right=param._right;
	HDAudioParam.leftsurround=param.leftsurround;
	HDAudioParam.rightsurround=param.rightsurround;
	HDAudioParam.subwoofer=param.subwoofer;

	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","AudioEncode",param.audioencode.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Left",param.left.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Center",param.center.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Right",param._right.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","LeftSurround",param.leftsurround.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","RightSurround",param.rightsurround.c_str());
	mConfig->SetNodeAttribute("HDDefAudioParamSet","HDDefAudioParam","Subwoofer",param.subwoofer.c_str());

	return true;
}
bool  ParamSetManager::SetAudioParam(std::string channelid,const AudioParam param)
{

	map<std::string,AudioParam>::iterator ptr=AudioParamMap.find(channelid);
	if (ptr!=AudioParamMap.end())
	{
		ptr->second.videopid=param.videopid;
		ptr->second.audiopid=param.audiopid;
		ptr->second.left=param.left;
		ptr->second.center=param.center;
		ptr->second._right=param._right;
		ptr->second.leftsurround=param.leftsurround;
		ptr->second.rightsurround=param.rightsurround;
		ptr->second.subwoofer=param.subwoofer;

		mConfig->SetNodeAttribute(channelid,"AudioParamSet","VideoPID",param.videopid.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","AudioPID",param.audiopid.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Left",param.left.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Center",param.center.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Right",param._right.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","LeftSurround",param.leftsurround.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","RightSurround",param.rightsurround.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Subwoofer",param.subwoofer.c_str());

	}
	else
	{
		string Freq,Serviceid;
		AudioParamMap.insert(make_pair(channelid,param));

		size_t pos=channelid.find("_");
		Freq=channelid.substr(0,pos);
		Serviceid=channelid.substr(pos+1);
		
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Freq",Freq.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","ServiceID",Serviceid.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","VideoPID",param.videopid.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","AudioPID",param.audiopid.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Left",param.left.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Center",param.center.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Right",param._right.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","LeftSurround",param.leftsurround.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","RightSurround",param.rightsurround.c_str());
		mConfig->SetNodeAttribute(channelid,"AudioParamSet","Subwoofer",param.subwoofer.c_str());

	}
	
	
	return true;
}
 void ParamSetManager::Start()
{
	cout<<"加载参数配置文件"<<endl;
}

 bool  ParamSetManager::GetAudioParam(std::string channelid,AudioParam & param)
 {
	 map<std::string,AudioParam>::iterator ptr=AudioParamMap.find(channelid);
	 if (ptr!=AudioParamMap.end())
	 {
		param=AudioParamMap[channelid];
		return true;
	 }
	 else
	 {
		return false;
	 }
 }
bool ParamSetManager::GetCardNoByPosition(std::string strPosition,std::string& strCardNo)
{
	 bool bRet = false;
	 std::vector<CACardInfo>::iterator iter = vecCardInfo.begin();
	 for (; iter!=vecCardInfo.end(); iter++)
	 {
		 if (iter->position == strPosition)
		 {
			 strCardNo = iter->cardno;
			 bRet = true;
			 break;
		 }
	 }

	 return bRet;
}

 bool ParamSetManager::GetCACardInfoByCardNo( std::string strCardNo, CACardInfo& cardInfo )
 {
	 bool bRet = false;
	 std::vector<CACardInfo>::iterator iter = vecCardInfo.begin();
	 for (; iter!=vecCardInfo.end(); iter++)
	 {
		 if (iter->cardno == strCardNo)
		 {
			 cardInfo = *iter;
			 bRet = true;
			 break;
		 }
	 }

	 return bRet;
 }

#endif