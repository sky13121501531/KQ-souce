///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TsFetcherMgr.h
// 创建者：gaoxd
// 创建时间：2009-06-18
// 内容描述：数据获取线程管理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class ChanScanThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ChanScanThread();
	virtual ~ChanScanThread();
public:
	int Start();

	int open(void*);

	virtual int svc();
	string CreatePreChanScanRet(eDVBType dvbType);
	string CreatePreChanScanRetTwo(eDVBType dvbType);
	bool ChanScanForDVBC(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq, std::string sCheckHD);//DVBC 频道扫描
	bool ChanScanForCTTB(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq);//CTTB 频道扫描
	bool ChanScanForFM(std::string sDevID);//FM 频道扫描
	bool ChanScanForAM(std::string sDevID);//AM 频道扫描
	bool ChanScanForDVBS(std::string sDevID);//DVBS 频道扫描
	bool PSISIForDVBC(std::string sDevID,std::string sFreq);
	//跳过 HTTP 头
	string SkipHTTPHead(string http);
	bool GetDVBTypeAndCardIndex(string sDevList, map<string, string>& dvbCardIndex);
	bool ValidProXml(string& strXml);//节目名称中有空格的XML, 去掉空格
	bool ValidEpgDur(string strEpgDur);//判断epg播放时长是否仅0~9和:字符 是返回true 否false
	void SetChanScanNow(string NoticeXml,string NowScanFreq = "",string NowScanDevid = "");//设置 即时采集 频道表 EPG 
	bool CheckHDProgramForDVBC(string sRetXMl, string& sHDTV);//检测高清节目
	bool GetHDInfo(string freq,string servid,std::vector<HDInfoForScan>& HDInfoForScanList,string& hdi);//更新节目信息文件
	bool GetDvbsFreqInfo(string freq,sdvbsfreqinfo& dvbsfreq);
	int Stop();
    void DoStringFreq(string &strstr,vector<string>& vStr);
private:
	bool bFlag;
	bool m_bChanScanWorkType;
	string m_sNoticeXml;
	string m_sNoticeXmlTwo;//用于epg和频道同时同通道
	string m_sOpVersion;
	string m_sNowScanFreq;
	string m_sNowScanDevid;
	std::vector<sdvbsfreqinfo> sdvbsfreqinfoList;
};
typedef  ACE_Singleton<ChanScanThread,ACE_Mutex>  CHANSCANTHREAD;