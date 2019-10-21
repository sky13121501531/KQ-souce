#ifndef _PARAMSETMANAGER_H_
#define _PARAMSETMANAGER_H_



#include <string>
#include <list>
#include <map>
#include "ace/Singleton.h"
#include "ParamSetConfig.h"
#include "TypeDef.h"

#ifdef ZONG_JU_ZHAO_BIAO

class ParamSetManager
{
public:
	ParamSetManager(void);
	~ParamSetManager(void);
	bool  GetStreamRate(RateParam &rate){rate=StreamRate;return true;};
	bool  GetHDRecordRate(RateParam &rate){rate=HDRecordRate;return true;};
	bool  GetSDRecordRate(RateParam &rate){rate=SDRecordRate;return true;};
	bool  GetHDAudioParam(AudioParam & param){param=HDAudioParam;return true;};
	bool  GetAudioParam(std::string channelid,AudioParam & param);
	bool  GetCACardInfo(std::vector<CACardInfo> &cardinfo){cardinfo=vecCardInfo;return true;};
	bool  GetMHPInfo(MHPInfo &info){info=InfoMhp;return true;};

	bool GetCACardInfoByCardNo(std::string strCardNo, CACardInfo& cardInfo);
	bool GetCardNoByPosition(std::string strPosition,std::string& strCardNo);

	bool  SetStreamRate(const RateParam rate);
	bool  SetHDRecordRate(const RateParam rate);
	bool  SetSDRecordRate(const RateParam rate);
	bool  SetHDAudioParam(const AudioParam param);
	bool  SetAudioParam(std::string channelid,const AudioParam param);

	void  InitParamInfo();
	void  Start();
private:
	RateParam StreamRate;
	RateParam HDRecordRate;
	RateParam SDRecordRate;

	MHPInfo  InfoMhp;
	AudioParam HDAudioParam;
	ParamSetConfig *mConfig;
	std::vector<CACardInfo> vecCardInfo;
	std::map<std::string,AudioParam> AudioParamMap;

};
typedef ACE_Singleton <ParamSetManager, ACE_Thread_Mutex>  PARAMSETMANAGER;


#endif

#endif