


#ifndef AM_TCP_ACCESS_H
#define	AM_TCP_ACCESS_H


#include "./TCPDeviceAccess.h"
#include <string>
using namespace std;

class AmTcpDeviceAccess : public TCPDeviceAccess
{
public:
	AmTcpDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~AmTcpDeviceAccess();


	/**	各测量任务总接口
	*/
private:
	virtual bool GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg);


	/**	发送任务命令接口
	*/
private:
	//virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)	{return -1 ; }	//电视频道扫描接口接口
	virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen);
	virtual int  SendCmdForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen);


	/**	中短波频道扫描和频谱扫描相关接口
	*/
private:
	bool SpecScan(int freq,RadioSpecRetMessage_Obj& RadioSpec);
	void CaculateCenterFreq(int startfreq,int endfreq,int *centerfreqs,int &len);	//计算AM的中心频点
	void FindChannels(int startfreq, int endfreq, const std::vector<float>& vecSpecValue, std::vector<int>& vecChanFreq);	//获得频道扫描结果
	//频谱扫描
	bool GetAMSpec(float fFreq, RadioSpecRetMessage_Obj &rqr);


	/**	中短波指标测量相关接口
	*/
public:
	virtual bool GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr);		//(会被外部调用，定义为public)
//private:
	//bool QueryQuality(int freq,RadioQuaRetMessage_Obj& RadioQua);      //中短波实时指标
	vector<RadioQuaRetMessage_Obj> m_vecRQRhistory;//缓存数据 N秒内不重复操作硬件

	/**	调频相关接口
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo);			//调频
	bool SetAudioRate(const MediaConfig_Obj& rateinfo);			//音频码率
};





























#endif