#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "CardType.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/OSFunction.h"
#include "ace/Singleton.h"
#include <string>

class FetchQualityLevel
{
public:
	FetchQualityLevel(enumDVBType dvbtype,std::string strIP,int nPort);
	virtual ~FetchQualityLevel(void);
protected:
	FetchQualityLevel(void);
public:
	 int  GetLevelFromCard(int Freq);
private:
	enumDVBType DvbType;
	static ACE_Thread_Mutex DVBCLevelMutex;
	static ACE_Thread_Mutex CTTBLevelMutex;
	static ACE_Thread_Mutex DVBSLevelMutex;
	std::string strIPAddress;
	int port;
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;
	ACE_INET_Addr server;
};