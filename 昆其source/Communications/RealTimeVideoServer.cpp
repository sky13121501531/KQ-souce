///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RealTimeVideoServer.cpp
// 创建者：jiangcheng
// 创建时间：2009-05-20
// 内容描述：实时视频服务器端处理类
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "RealTimeVideoServer.h"
#include "CommunicationMgr.h"
#include "TsSenderMgr.h"
#include <sstream>
#include <iostream>
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/PropManager.h"
#include "../DeviceAccess/TsFetcherMgr.h"
ACE_Thread_Mutex g_CodecMutex;
int g_iCurCodecUserID = -1;
int g_iCurCodecUserNum = 0;
const int MAXDEVICEIDNUM = 2;
extern int g_icodecchan;//用于1路转码
RealTimeVideoServer::RealTimeVideoServer()
{
	bFlag = false;
}

RealTimeVideoServer::~RealTimeVideoServer()
{

}

int RealTimeVideoServer::Open(ACE_Addr &addr)
{
	if (peer_acceptor.open(addr) == -1)//监听实时视频服务器端口
	{
		cout<<"RealTimeVideoServer error"<<endl;
		return -1;
	}
	return 0;
}

int RealTimeVideoServer::handle_input(ACE_HANDLE handle)
{
	ACE_SOCK_Stream client;//连接socket
	ACE_INET_Addr clientAddr;//客户端地址信息
	ACE_Time_Value TimeOut(8);

	if (this->peer_acceptor.accept(client,&clientAddr,&TimeOut) == -1)//接受客户端连接
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t) 接收用户连接错误\n"));
		client.close();
		return 0;
	}
	
	const int PKGSIZE = 188;
	int value =PKGSIZE*1024;	

	//设置socket发送接收缓冲
	client.set_option(SOL_SOCKET,SO_SNDBUF,(char*)&value,sizeof(value));
	client.set_option(SOL_SOCKET,SO_RCVBUF,(char*)&value,sizeof(value));

	list<int> userdeviceilist;
	if (false == RecvDeviceID(client,userdeviceilist)) //接收用户数据,得到通道号
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户(%s:%d):接收数据失败\n",clientAddr.get_host_addr(),clientAddr.get_port_number()));
		client.close();
		return 0;
	}
	
	if (userdeviceilist.empty())
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户(%s:%d):无通道错误\n",clientAddr.get_host_addr(),clientAddr.get_port_number()));
		client.close();
		return 0;
	}
	//分析用户通道数据
	list<int>::iterator ptr_user = userdeviceilist.begin();
	for (;ptr_user!=userdeviceilist.end();++ptr_user)
	{
		if (false == PROPMANAGER::instance()->IsDeviceAvaiable(*ptr_user)&&
			false == PROPMANAGER::instance()->IsRoundChannel(*ptr_user) &&((*ptr_user)!=99))
		{
			ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户(%s:%d):通道[%d]错误\n",clientAddr.get_host_addr(),clientAddr.get_port_number(),(*ptr_user)));
			client.close();
			return 0;
		}
	}
	
	if (false == SendHttpHeader(client))//发送回复数据
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户(%s:%d):发送回复数据失败\n",clientAddr.get_host_addr(),clientAddr.get_port_number()));
		client.close();
		return 0;
	}
	//保存连接的客户端信息
	ptr_user = userdeviceilist.begin();
	for (;ptr_user!=userdeviceilist.end();++ptr_user)
	{
		sVedioUserInfo tempuserinfo;
		tempuserinfo.client		= client;
		tempuserinfo.DeviceID	= StrUtil::Int2Str(*ptr_user);
		tempuserinfo.IP			= clientAddr.get_host_addr();
		tempuserinfo.Port		= StrUtil::Int2Str(clientAddr.get_port_number());
		
		//将客户添加到相应的数据发送队列
		if(this->isLAN(tempuserinfo.IP))//用于1路转码
		{//用于1路转码
			TSSENDERMGR::instance()->AddClient(*ptr_user,tempuserinfo);
			ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户连接(%s:%d):通道[%d]\n",clientAddr.get_host_addr(),clientAddr.get_port_number(),*ptr_user));
		}//用于1路转码
		else
		{//用于1路转码
			if(g_icodecchan != -1)//用于1路转码
			{//用于1路转码
				g_CodecMutex.acquire();
				g_iCurCodecUserID = *ptr_user;
				g_iCurCodecUserNum++;
				g_CodecMutex.release();
				TSFETCHERMGR::instance()->SetForDataToCodec(*ptr_user,true);//用于1路转码
				TSSENDERMGR::instance()->AddClient(g_icodecchan,tempuserinfo);//用于1路转码
				ACE_DEBUG((LM_DEBUG,"(%T| %t) 用户连接(%s:%d):编转码通道[%d]\n",clientAddr.get_host_addr(),clientAddr.get_port_number(),g_icodecchan));//用于1路转码
			}//用于1路转码
		}//用于1路转码
		
	}

	return 0;
}

int RealTimeVideoServer::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	//移除视频服务器
	close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	delete this;
	return 0;
}

ACE_HANDLE RealTimeVideoServer::get_handle(void) const
{
	return this->peer_acceptor.get_handle();
}

int RealTimeVideoServer::Stop()
{
	ACE_Reactor_Mask close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	return 0;
}
bool RealTimeVideoServer:: RecvDeviceID(ACE_SOCK_Stream& newclient,list<int>& deviceIDlist)
{
	int countTimes = 10;//接收客户端发送数据的次数
	char RecvBuf[4096] = {0};
	int RecvCount = -1;
	try
	{
		ACE_Time_Value TimeOut(0,100);
		while(--countTimes)
		{
			ssize_t RecvCount = newclient.recv(RecvBuf,4096,&TimeOut);//接收数据
		
			if(RecvCount < 4 || RecvCount == SOCKET_ERROR)//socket错误
			{
				OSFunction::Sleep(0,10);
				continue;
			}
			
			char szSeps[] = "\n";

			//wz_110125:解决cpToken为NULL时，string dsp(cpToken)出现的异常
			char *cpToken = strtok(RecvBuf, szSeps);
			if (cpToken == NULL)
			{
				return false;
			}
			string dsp(cpToken);

			dsp = dsp.substr(dsp.find_first_of("/"));
			dsp = dsp.substr(0,dsp.find_first_of(" "));
			std::stringstream stream(dsp);
			string strTemp;
			//客户通过端连接获得用户所连接的通道号，http://192.168.100.56:8080/1/2
			while (getline(stream,strTemp,'/'))
			{
				if (strTemp != "")
				{
					int ps = strTemp.find("_");
					if(/*(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")&&*/(ps !=-1))
					{
						int cChassisID = StrUtil::Str2Int(strTemp.substr(0,ps));
						string tem = strTemp.substr(ps+1,strTemp.size()-ps-1);
						ps = tem.find("_");
						int cPosition = StrUtil::Str2Int(tem.substr(0,ps));
						tem = tem.substr(ps+1,tem.size()-ps-1);
						ps = tem.find("_");
						int cModuleID = StrUtil::Str2Int(tem.substr(0,ps));
						tem = tem.substr(ps+1,tem.size()-ps-1);
						ps = tem.find("_");
						int cTranscode = -1;
						if(ps==-1&&tem.size()>0)
						{
							cTranscode = StrUtil::Str2Int(tem.substr(0,ps));
						}
						int devid = PROPMANAGER::instance()->GetCoderDeviceID(cChassisID,cPosition,cModuleID,cTranscode);
						deviceIDlist.push_back(devid);
						break;
					}
					else
					{
						int deviceid = StrUtil::Str2Int(strTemp);//得到连接的通道号
						deviceIDlist.push_back(deviceid);
					}
				}
			}
			break;
		}
	}
	catch(...)
	{
		;
	}
	return true;
}
bool RealTimeVideoServer::SendHttpHeader(ACE_SOCK_Stream& newclient)
{
	std::string HttpHeader = "HTTP/1.1 200 OK\r\n";//http响应头
	HttpHeader += std::string("Content-Type: video/mpeg-ts\r\n") + std::string("Cache-Control: no-cache\r\nContent-Length: 0-\r\n") + std::string("\r\n");

	ACE_Message_Block *MBHttpHeader = new ACE_Message_Block(HttpHeader.length());
	memcpy(MBHttpHeader->wr_ptr(),HttpHeader.c_str(),HttpHeader.length());
	MBHttpHeader->wr_ptr(HttpHeader.length());

	ACE_Time_Value TimeOut(0,100);
	int nCount = newclient.send_n(MBHttpHeader,&TimeOut);//发送http响应头数据
	if(nCount<=0)
		return false;	
	return true;
}
bool RealTimeVideoServer::isLAN(const string& ipstring)
{
	istringstream st(ipstring);
	int ip[2];
	for (int i = 0; i < 2; i++)
	{
		string temp;
		getline(st, temp, '.');
		istringstream a(temp);
		a >> ip[i];
	}
	if ((ip[0] == 10) || (ip[0] == 172/* && ip[1] >= 16 && ip[1] <= 31*/) || (ip[0] == 192/* && ip[1] == 168*/))
		return true;
	else 
		return false;
}