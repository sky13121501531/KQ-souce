///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvAlarmThread.h
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include <string>
#include "../Foundation/TypeDef.h"
using namespace std;
class UdpRecvAlarmThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpRecvAlarmThread(string ip,string port,int wtype);
	virtual ~UdpRecvAlarmThread();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetAlarmParam(eDVBType dvbtype,string channelid,string freq,string serviceid,string videopid,string devid,string secretpro = "")
	{
		m_strchannelid = channelid;
		m_strfreq = freq;
		m_strdeviceid = devid;
		m_dvbtype = dvbtype;
		m_strSecretPro = secretpro;
		m_strVideoPID = videopid;
		m_strServiceID = serviceid;
	}

	void SetCoderAlarm(bool codealarm)
	{
		m_bSetCoderAlarm = codealarm;
	}
private:
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	bool bFlag;
	bool m_bSetCoderAlarm;
	int iWorktype;//0 tuner 1 coder
	char m_picalarm[11];//m_picalarm[0]静帧标识 m_picalarm[1]黑场标识 m_picalarm[2]无伴音0标识 (m_picalarm[3]无伴音1标识)不使用 (m_picalarm[4]无伴音2标识)不使用 m_picalarm[5]无信号解码失败标识
	char m_picalarmtimes[3];//m_picalarmtimes[0]静帧标识数量 m_picalarmtimes[1]黑场标识数量 m_picalarmtimes[2]无伴音标识数量     小于20是报警计数    大于等于30是解除报警计数   初始化为25 当前状态为报警值大于等于20表示上次是解除报警重新计数  当前状态为解除报警值小于30表示上次是报警重新计数   
	time_t m_picalarmtime_t[3];//m_picalarmtime_t[0]静帧标识时间 m_picalarmtime_t[1]黑场标识时间 m_picalarmtime_t[2]无伴音标识时间
	string m_strchannelid;
	string m_strfreq;
	string m_strdeviceid;
	string m_strSecretPro;//1表示加密流 0表示清流
	string m_strServiceID;
	string m_strVideoPID;
	eDVBType m_dvbtype;
	time_t m_errCnttime;
	time_t m_errPattime;
	time_t m_errPmttime;
	bool pic_isAlarm[7];
	bool aud_isAlarm[6][3];
	ACE_Thread_Mutex ProMutex;
};
typedef  ACE_Singleton<UdpRecvAlarmThread,ACE_Mutex>  UDPRECVALARMTHREAD;
