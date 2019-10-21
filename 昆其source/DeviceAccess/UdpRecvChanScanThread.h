///////////////////////////////////////////////////////////////////////////////////////////
// ÎÄ¼þÃû£ºUdpRecvChanScanThread.h
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
class UdpRecvChanScanThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpRecvChanScanThread();
	virtual ~UdpRecvChanScanThread();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetActive(bool bactve,string udpIp,string udpPort)
	{
		m_bActive = bactve;
		m_UdpIP = udpIp;
		m_UdpPort = udpPort;
	}
	unsigned char* GetTsData(long& datasize)
	{
		datasize = m_BufDataNum;
		return m_DataBuf;
	}

private:
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	bool bFlag;
	//
	bool m_bActive;
	string m_UdpIP;
	string m_UdpPort;
	unsigned char* m_DataBuf;
	long m_BufDataNum;
};
typedef  ACE_Singleton<UdpRecvChanScanThread,ACE_Mutex>  UDPRECVCHANSCANTHREAD;
