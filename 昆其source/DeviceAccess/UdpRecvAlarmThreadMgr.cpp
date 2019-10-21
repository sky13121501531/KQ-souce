///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvAlarmThreadMgr.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "UdpRecvAlarmThread.h"
#include "UdpRecvAlarmThreadMgr.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/StrUtil.h"
#include "../Alarm/AlarmMgr.h"
#include "UdpRecvDevCardThread.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <IPExport.h>
#include <iphlpapi.h>
#include "../DeviceAccess/cping.h"
#include "../Foundation/AppLog.h"
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"wsock32.lib")
UdpRecvAlarmThreadMgr::UdpRecvAlarmThreadMgr()
{
}

UdpRecvAlarmThreadMgr::~UdpRecvAlarmThreadMgr()
{
}

int UdpRecvAlarmThreadMgr::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpRecvAlarmThreadMgr::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
int UdpRecvAlarmThreadMgr::svc()
{
	memset(m_lockalarm,'0',sizeof(char)*300);
	memset(m_caalarm,'0',sizeof(char)*300);
	memset(m_tsalarm,'0',sizeof(char)*300);
	int iTunnum = PROPMANAGER::instance()->GetTunerDevNum();
	int iCodnum = PROPMANAGER::instance()->GetCoderDevNum();
	time_t m_SetCardTime = time(0);
	time_t m_SetPingTime = time(0);
	while (bFlag)
	{
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
		{
			iTunnum = PROPMANAGER::instance()->GetTunerDevNum();
			iCodnum = PROPMANAGER::instance()->GetCoderDevNum();
		}
		for (int i = 0;i < iTunnum;i++)
		{
			//
			TunerDev tdev;
			PROPMANAGER::instance()->GetTunerInfo(i,tdev);
			//
			if((tdev.b_user)&&(tdev.s_freq != strHDMIFreq))
			{
				std::map<int,UdpRecvAlarmThread*>::iterator iter = Alarm290UdpRecvThreadMap.find(i);//根据通道号查找硬件设备
				if (iter == Alarm290UdpRecvThreadMap.end())
				{
					UdpRecvAlarmThread* UdpRecvAlarm = NULL;
					UdpRecvAlarm = new UdpRecvAlarmThread(tdev.s_290ip,tdev.s_290port,0);
					if (UdpRecvAlarm != NULL)
					{
						Alarm290UdpRecvThreadMap.insert(make_pair(i,UdpRecvAlarm));
						UdpRecvAlarm->SetAlarmParam(tdev.e_tunerdvbtype,"",tdev.s_freq,"","",tdev.s_tunerdeviceid);
						UdpRecvAlarm->Start();
					}
				}
			}
			else
			{
				std::map<int,UdpRecvAlarmThread*>::iterator iter = Alarm290UdpRecvThreadMap.find(i);//根据通道号查找硬件设备
				if (iter != Alarm290UdpRecvThreadMap.end())
				{
					UdpRecvAlarmThread* at = iter->second;
					at->Stop();
					Alarm290UdpRecvThreadMap.erase(iter);
					delete(at);
				}
				else
				{
					Sleep(5);
				}
			}
		}
		//
		for (int j = 0;j < iCodnum;j++)
		{
			//
			CoderDev cdev;
			PROPMANAGER::instance()->GetCoderInfo(j,cdev);
			//
			if(cdev.b_user)
			{
				if(cdev.s_freq != strHDMIFreq)
				{
					string Channelid = "";
					CHANNELINFOMGR::instance()->GetChannelID(cdev.e_tunerdvbtype,cdev.s_freq,cdev.s_spro,Channelid);
					string Videopid = "";
					CHANNELINFOMGR::instance()->GetVideoID(cdev.e_tunerdvbtype,cdev.s_freq,cdev.s_spro,Videopid);
					string SecretPro = "0";
					sCheckParam sProCheck;
					sProCheck.AlarmType	= ALARM_PROGRAM;
					sProCheck.DVBType		= cdev.e_tunerdvbtype;
					sProCheck.ChannelID	= Channelid;
					sProCheck.Freq			= cdev.s_freq;
					sProCheck.ServiceID		= cdev.s_spro;
					sProCheck.STD			= "";
					sProCheck.SymbolRate	= "";
					sProCheck.TypedValue	= "";
					sProCheck.DeviceID		= StrUtil::Int2Str(j);
					//
					bool block = true,bca = true,err = false,alarmUdp = false;
					//
					string mindex,mip,mport;
					std::map<int,UdpRecvAlarmThread*>::iterator iter = AlarmProUdpRecvThreadMap.find(j);//根据通道号查找硬件设备
					if (iter == AlarmProUdpRecvThreadMap.end())
					{
						UdpRecvAlarmThread* UdpRecvAlarm = NULL;
						UdpRecvAlarm = new UdpRecvAlarmThread(cdev.s_alarmip,cdev.s_alarmport,1);
						if (UdpRecvAlarm != NULL)
						{
							AlarmProUdpRecvThreadMap.insert(make_pair(j,UdpRecvAlarm));
							UdpRecvAlarm->SetAlarmParam(cdev.e_tunerdvbtype,Channelid,cdev.s_freq,cdev.s_spro,Videopid,cdev.s_coderdeviceid,SecretPro);
							UdpRecvAlarm->Start();
						}
					}
					else
					{
						UdpRecvAlarmThread* at = iter->second;
						if((!block)||(!bca)||(err))
						{
							at->SetCoderAlarm(false);
						}
						else
						{
							at->SetCoderAlarm(true);
						}
					}
				}
			}
			else
			{
				std::map<int,UdpRecvAlarmThread*>::iterator iter = AlarmProUdpRecvThreadMap.find(j);//根据通道号查找硬件设备
				if (iter != AlarmProUdpRecvThreadMap.end())
				{
					UdpRecvAlarmThread* at = iter->second;
					at->Stop();
					AlarmProUdpRecvThreadMap.erase(iter);
					delete(at);
				}
				else
				{
					Sleep(5);
				}
			}
		}
		if(time(0)-m_SetPingTime > 3)
		{
			bool m_RetVal;  
			m_SetPingTime = time(0);
			int Monnum = PROPMANAGER::instance()->GetMonitorDevNum();
			for(int i=0;i<Monnum;i++)
			{
				sDeviceInfo Devinfo;
				PROPMANAGER::instance()->GetDevMonitorInfo(i,Devinfo);
				char* DestIp = (char*)Devinfo.s_deviceip.c_str();
				CPing cping;
				PingReply reply;
				m_RetVal = cping.Ping(DestIp,&reply);
				//*************故障报警开始******************
				//能ping通 为真
				if(m_RetVal)
				{
					//cout<<"======>"<<DestIp<<"------["<<reply.m_dwBytes<<"]------["<<reply.m_dwRoundTripTime<<"]------["<<reply.m_dwTTL<<"]"<<endl;

					sCheckParam sEquCheck;
					sEquCheck.AlarmType	= ALARM_EQUIPMENT;
					sEquCheck.DVBType		= DVBC;
					sEquCheck.ChannelID	= "";
					sEquCheck.Freq			= "";
					sEquCheck.STD			= "";
					sEquCheck.SymbolRate	= "";
					sEquCheck.TypedValue	= "";
					sEquCheck.DeviceID		= Devinfo.s_deviceindex;
					sEquCheck.mode = "1";
					sEquCheck.TypeDesc		= "监测设备故障报警解除:"+Devinfo.s_deviceip;
					sEquCheck.TypeID		= "60";
					sEquCheck.CheckTime	= time(0);
					//ALARMMGR::instance()->CheckAlarm(sEquCheck,false);
				}
				else
				{
					string strcmd = string("------ [") + Devinfo.s_deviceip + string("] ---- [") + StrUtil::Int2Str(i) + string("] 号 监测设备 故障 报警");
					APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_WARNNING,strcmd,LOG_OUTPUT_BOTH);
					sCheckParam sEquCheck;
					sEquCheck.AlarmType	= ALARM_EQUIPMENT;
					sEquCheck.DVBType		= DVBC;
					sEquCheck.ChannelID	= "";
					sEquCheck.Freq			= "";
					sEquCheck.STD			= "";
					sEquCheck.SymbolRate	= "";
					sEquCheck.TypedValue	= "";
					sEquCheck.DeviceID		= Devinfo.s_deviceindex;
					sEquCheck.mode = "0";
					sEquCheck.TypeDesc		= "监测设备故障报警:"+Devinfo.s_deviceip;
					sEquCheck.TypeID		= "60";
					sEquCheck.CheckTime	= time(0);
					//
					//ALARMMGR::instance()->CheckAlarm(sEquCheck,false);
				}
			}
		}
	}
	bFlag = false;
	return 0;
}
int UdpRecvAlarmThreadMgr::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}



