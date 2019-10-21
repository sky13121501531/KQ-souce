#include "XMLReceiverServer.h"
#include "CommunicationMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TypeDef.h"
#include "ace/OS.h"
#include <string>
#include <sstream>
#include <iostream>

XMLReceiverServer::XMLReceiverServer(void)
{
	bFlag=false;
}

XMLReceiverServer::~XMLReceiverServer(void)
{
}

int XMLReceiverServer::Open( ACE_Addr &addr )
{
	if (peer_acceptor.open(addr) == -1)//监听端口
	{
		cout<<"XMLReceiverServer error"<<endl;
		return -1;
	}

	return 0;
}

int XMLReceiverServer::handle_input( ACE_HANDLE handle )
{
	ACE_SOCK_Stream client;
	ACE_INET_Addr clientAddr;//保存客户端连接地址信息

	if (this->peer_acceptor.accept(client,&clientAddr) == -1)//接收客户端连接
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)Error in connection\n"));
	}
	else
	{
		string TaskXml = "";
		if(false == RecvTaskXml(client,TaskXml))//接受任务
		{ 
			client.close();
			string msg = string("XML命令接受服务器接受数据错误");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			return 0;
		}
		if(false == SendHttpHeader(client))//发送回复数据
		{ 
			client.close();

			string msg = string("XML命令接受服发送回复数据错误");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return 0;
		}
		
		client.close();
		
		if (TaskXml.length() <= 0)
		{
			string msg = string("XML命令接受服接收到的xml为空");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return 0;
		}
		//SYSMSGSENDER::instance()->SendMsg(TaskXml);

		ACE_Message_Block *mbXML = new ACE_Message_Block(TaskXml.length());
		memcpy(mbXML->wr_ptr(),TaskXml.c_str(),TaskXml.length());
		mbXML->wr_ptr(TaskXml.length());

		if (mbXML != NULL)
		{
			ACE_Time_Value OutTime(ACE_OS::time(0)+1);
			COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mbXML,&OutTime);
		}
	}

	return 0;
}

ACE_HANDLE XMLReceiverServer::get_handle( void ) const
{
	return peer_acceptor.get_handle();
}

int XMLReceiverServer::handle_close( ACE_HANDLE handle, ACE_Reactor_Mask close_mask )
{
	close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	//从反应器中移除指标服务器
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	delete this;
	return 0;
}

int XMLReceiverServer::Stop()
{
	ACE_Reactor_Mask close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();
	return 0;
}

bool XMLReceiverServer::RecvTaskXml( ACE_SOCK_Stream& newclient,std::string& taskxml )
{
	int countTimes = 10;//接收发送数据的次数
	char RecvBuf[51200] = {0};
	int RecvCount = -1;
	bool ret = false;
	try
	{
		ACE_Time_Value TimeOut(0,300);
		while(--countTimes)
		{
			Sleep(500);
			memset(RecvBuf,0,sizeof(char)*51200);
			ssize_t RecvCount = newclient.recv(RecvBuf,51200,&TimeOut);
			if(RecvCount < 4 || RecvCount == SOCKET_ERROR)
			{
				OSFunction::Sleep(0,50);
				continue;
			}
			cout<<"收到信息:"<<endl;
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

bool XMLReceiverServer::SendHttpHeader( ACE_SOCK_Stream& newclient )
{
	std::string HttpHeader = "HTTP/1.1 200 OK\r\n";//http响应头
	std::string msg="Receive port msg.";
	HttpHeader += std::string("Server: Apache-Coyote/1.1\r\n") + \
				  std::string("Content-Type: text/html;charset=GBK\r\n") +		\
				  std::string("Content-Length: 17\r\n") + \
				  std::string("Date: Wed, 09 Aug 2018 10:43:00 GMT\r\n") + \
				  std::string("Connection: close\r\n") + \
				  std::string("\r\n")+msg;

	ACE_Time_Value TimeOut(0,100);
	ACE_Message_Block *MBHttpHeader = new ACE_Message_Block(HttpHeader.length());
	memcpy(MBHttpHeader->wr_ptr(),HttpHeader.c_str(),HttpHeader.length());
	MBHttpHeader->wr_ptr(HttpHeader.length());

	int nCount = newclient.send_n(MBHttpHeader,&TimeOut);//发送http响应头数据
	if(nCount<=0)
		return false;	

	return true;
}