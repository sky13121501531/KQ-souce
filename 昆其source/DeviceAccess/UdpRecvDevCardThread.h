///////////////////////////////////////////////////////////////////////////////////////////
// ÎÄ¼þÃû£ºUdpRecvDevCardThread.h
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
class UdpRecvDevCardThread;
class UdpRecvDevCardThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpRecvDevCardThread();
	UdpRecvDevCardThread(string ip,string port);
	virtual ~UdpRecvDevCardThread();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	bool GetChannelResource(string addr,string &startTime);

private:
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	bool bFlag;
	//
	bool SetChResourceInfo(standardChannelResource chres);
	bool SetBoardCheckInfo(char* boche);
	std::vector<standardChannelResource> vecstandardChannelResource;
};
