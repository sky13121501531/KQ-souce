///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DeviceServer.h
// 创建者：gaoxd
// 创建时间：2009-11-30
// 内容描述：板卡主动上报信息服务器端处理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "ace/Event_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include <string>
#include <map>
#include "../Foundation/TypeDef.h"


class DeviceServer: public ACE_Event_Handler
{
public:
	DeviceServer(void);
	~DeviceServer(void);
public:
	int Open(ACE_Addr &addr);
	int handle_input(ACE_HANDLE handle);
	ACE_HANDLE get_handle(void) const;
	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	bool RecvDevReXml( ACE_SOCK_Stream& newclient,std::string& taskxml );
	string GetDevRet(string ReplyID);
public:
	int Stop();
private:
	ACE_SOCK_Acceptor peer_acceptor;
	ACE_Thread_Mutex RecDevRetServerMutex;
	bool bFlag;
};
typedef ACE_Singleton <DeviceServer, ACE_Mutex>  DEVRETSERVER;