#pragma once
#include "ace/Event_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include <string>
class XMLReceiverServer:public ACE_Event_Handler
{
public:
	XMLReceiverServer(void);
	~XMLReceiverServer(void);
public:
	int Open(ACE_Addr &addr);
	int handle_input(ACE_HANDLE handle);
	ACE_HANDLE get_handle(void) const;
	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	int Stop();
private:
	bool RecvTaskXml(ACE_SOCK_Stream& newclient,std::string& taskxml);
	bool SendHttpHeader(ACE_SOCK_Stream& newclient);
private:
	ACE_SOCK_Acceptor peer_acceptor;
	bool bFlag;
};
