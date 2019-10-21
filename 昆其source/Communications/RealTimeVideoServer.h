///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RealTimeVideoServer.h
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：实时视频服务器端处理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Event_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include <list>
using namespace std;
class RealTimeVideoServer : public ACE_Event_Handler
{
public:
	RealTimeVideoServer();
	virtual ~RealTimeVideoServer();
public:
	int Open(ACE_Addr &addr);

	int handle_input(ACE_HANDLE handle);

	ACE_HANDLE get_handle(void) const;

	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

public:
	bool RecvDeviceID(ACE_SOCK_Stream& newclient,std::list<int>& deviceIDlist);
	bool SendHttpHeader(ACE_SOCK_Stream& newclient);
	int Stop();
	bool isLAN(const string& ipstring);
private:
	ACE_SOCK_Acceptor peer_acceptor;
	ACE_Thread_Mutex videoServerMutex;
	bool bFlag;
};
