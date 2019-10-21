#pragma once
#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include <string>
#include <vector>
#include <string>
using namespace std;
class AllSetTime : public ACE_Task<ACE_MT_SYNCH>
{
public:
	AllSetTime();
	virtual~AllSetTime();
public:
	int Start();
	int open(void*);
	virtual int svc();
	int Stop();
public:
	void TimeSet(int First_time,int msec_time);
	void SendTime();
private:
	bool bFlag;
};

typedef  ACE_Singleton<AllSetTime,ACE_Mutex>  ALLSETTIME;