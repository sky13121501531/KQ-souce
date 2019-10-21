#pragma once

#include "ace/Task.h"
#include <string>

class TaskXMLReSend : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskXMLReSend();
	~TaskXMLReSend();
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();
private:
	int ProcessMessage(ACE_Message_Block* mb);
	std::string GetURL(std::string& upXML);
private:	
	bool bFlag;
};