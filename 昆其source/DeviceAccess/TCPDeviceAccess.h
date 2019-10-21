///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：HTTPDeviceAccess.h
// 创建者：gaoxd
// 创建时间：2010-07-06
// 内容描述：硬件设备访问类，基于TCP协议
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <string>
#include <vector>
#include "CardType.h"
#include "DeviceAccess.h"

class TCPDeviceAccess : public DeviceAccess
{
public:
	TCPDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~TCPDeviceAccess(void);
private:
	TCPDeviceAccess(void);

public:
	bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg);
	bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle){return false;};
	std::string GetDeviceIP();
	bool CheckFreqLock();					//判断该通道是否锁定
	//设置ts流输入输出网络参数
	unsigned int SetMultiScrNetParam(unsigned char *dest, const ChanNetCfgObj& netcfg, unsigned char cmdCode);
	//设置ts流输入输出PID
	unsigned int SetMultiScrPIDParam(unsigned char *dest, const ChanPIDObj& channelpid, unsigned char cmdCode);
	unsigned short CRC16(unsigned char *ptr,unsigned int len);
protected:
	bool ConnectToServer();													//连接服务器
	bool SendCmdToServer(MSG_TYPE msg_type,void* info,int infolen);			//发送数据并判断是否成功
	bool SendCmdToServer(void* indata,int indatalen,void* outdata,int outdatalen);												
	

	/**	各测量任务总接口
	*/
private:
	virtual bool GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg) { return true ; }
	virtual bool GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }
	virtual bool GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }
	virtual bool GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }


	/**	发送任务指令接口
	*/
protected:
	virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)	{return -1 ; }	//电视频道扫描
	virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen)	{return -1 ; }
	virtual int  SendCmdComForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen)	{return -1 ; }
public:
	int  SendCmdToTVCom(MSG_TYPE msg_type,void* info,int infolen);


	/**	获取指标接口(只提供广播和AM)
	*/
public:
	virtual bool GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }
	//virtual bool GetRadioQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }
	//virtual bool GetAMQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }


	/**	调频
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo) { return true ; }
protected:
	bool SetOSD(int chan,const SubtitleConfig_Obj& osdinfo);	//设置OSD
	bool SetSysTime();

private:
	bool InitCard();						//初始化板卡

	bool StartTranscode();					//开始转码
	bool StopTranscode();					//停止转码<基本不会调用>

	bool SetVideoAlertInfo( AlertConfig_Obj& alertinfo );		//设置视频异态参数
	bool SetAudioAlertInfo( AlertConfig_Obj& alertinfo );		//设置音频异态参数
	

protected:
	int GetAudioIndex(int audiorate);		//获取音频码率编号
protected:
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;
	ACE_INET_Addr server;

	int mChannelID;
	//std::string mstrDeviceIP;
};
