#include "DeviceServer.h"
#include <string>
#include <sstream>
#include <iostream>
#include "../Foundation/OSFunction.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/Applog.h"
#include "CommunicationMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <ace/OS.h>

using namespace std;
std::vector<std::string> g_vecRecXMl;
DeviceServer::DeviceServer(void)
{
	bFlag=false;
}

DeviceServer::~DeviceServer(void)
{
}

int DeviceServer::Open( ACE_Addr &addr )
{
	if (peer_acceptor.open(addr) == -1)//监听服务器端口
	{
		cout<<"DeviceServer error:"<<endl;
		return -1;
	}
	return 0;
}

int DeviceServer::handle_input( ACE_HANDLE handle )
{
	ACE_SOCK_Stream client;											//连接socket
	ACE_INET_Addr clientAddr;										//客户端地址信息

	if (this->peer_acceptor.accept(client,&clientAddr) == -1)		//接受客户端连接
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)Error in connection\n"));
	}
	else
	{
		string TaskXml = "";
		if(false == RecvDevReXml(client,TaskXml))//接受任务
		{ 
			client.close();
			string msg = string("接受设备上报结果命令数据错误");
			APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			return 0;
		}
		client.close();
		if (TaskXml.length() <= 0)
		{
			string msg = string("接受设备上报结果命令数据为空");
			APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			return 0;
		}
		RecDevRetServerMutex.acquire();
		g_vecRecXMl.push_back(TaskXml);
		RecDevRetServerMutex.release();
	}
	return 0;
}

ACE_HANDLE DeviceServer::get_handle( void ) const
{
	return peer_acceptor.get_handle();
}

int DeviceServer::handle_close( ACE_HANDLE handle, ACE_Reactor_Mask close_mask )
{
	close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	delete this;
	return 0;
}

int DeviceServer::Stop()
{
	ACE_Reactor_Mask close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	return 0;
}
bool DeviceServer::RecvDevReXml( ACE_SOCK_Stream& newclient,std::string& taskxml )
{
	int countTimes = 10;//接收发送数据的次数
	char RecvBuf[4096] = {0};
	int RecvCount = -1;
	bool ret = false;
	try
	{
		ACE_Time_Value TimeOut(0,100);
		while(--countTimes)
		{
			Sleep(100);
			ssize_t RecvCount = newclient.recv(RecvBuf,4096,&TimeOut);

			if(RecvCount < 4 || RecvCount == SOCKET_ERROR)
			{
				OSFunction::Sleep(0,50);
				continue;
			}
			cout<<"DeviceServer收到信息:"<<endl;
			cout<<RecvBuf<<endl;
			ret = true;
			char* p = RecvBuf;
			//找到xml字符串的开始之处
			bool bfind = false;
			for (int i=0;i<RecvCount;++i)
			{
				if (*p == '<')
				{
					bfind = true;
					break;
				}
				p++;
			}
			if(bfind)
			{
				taskxml = p;
				return true;
			}
		}
	}
	catch(...)
	{
		;
	}
	return ret;
}
string DeviceServer::GetDevRet(string ReplyID)
{
	string ret;
	RecDevRetServerMutex.acquire();
	if(g_vecRecXMl.size()>0)
	{
		std::vector<string>::iterator ptr=g_vecRecXMl.begin();
		for (;ptr!=g_vecRecXMl.end();ptr++)
		{
			string retReplyID;
			XmlParser headParser((*ptr).c_str());
			pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );
			headParser.GetAttrNode(headRootNode,"ReplayID",retReplyID);
			if(ReplyID == retReplyID)
			{
				ret = (*ptr);
				g_vecRecXMl.erase(ptr);
				break;
			}
		}
	}
	RecDevRetServerMutex.release();
	return ret;
}