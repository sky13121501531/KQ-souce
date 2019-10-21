#pragma once

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <string>

class ReSendAlarm : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ReSendAlarm();
	virtual ~ReSendAlarm();
public:
	int open(void*);

	virtual int svc();

	int Stop();
protected:
	int ProcessMessage();//±¨¾¯xml´¦Àí
private:
	bool bFlag;

};