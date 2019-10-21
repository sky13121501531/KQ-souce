#pragma once
#include <ace/Task.h>
#include <ace/SOCK_Dgram.h>
#include <string>
class DeviceLog:public ACE_Task<ACE_MT_SYNCH>
{
public:
	DeviceLog(void);
	~DeviceLog(void);
public:
	virtual int open(void*);

	virtual int svc();

	virtual int Stop();
private:
	bool bFlag;
    ACE_SOCK_Dgram sock;
};
