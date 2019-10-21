
#include "UdpTsSender.h"
#include "../Foundation/OSFunction.h"
#include "ace/Synch.h"
#include "ace/OS.h"
#include <string>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
//wz_0217
#include <list>
#include <map>
#include "../Foundation/PropManager.h"
//wz_0217

UdpTsSender::UdpTsSender()
{
	bFlag = false;
}

UdpTsSender::~UdpTsSender()
{

}
int UdpTsSender::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpTsSender::open(void*)
{
	bFlag = true;
	//设置发送队列缓冲区大小
	msg_queue()->high_water_mark(188*2048);
	msg_queue()->low_water_mark(188*256);

	activate();

	return 0;
}

int UdpTsSender::svc()
{
	ACE_Message_Block *mb = 0;
	m_bSetAddr = false;
	m_bClealAddr = false;
	ACE_INET_Addr local_addr;
	ACE_SOCK_Dgram MultiDeviceSock(local_addr);//用于将视频数据转发给多画面卡
	while (bFlag)
	{
		try 
		{
			if(m_bSetAddr)
			{
				m_bSetAddr = false;
				m_UdpSendTsAddrlist.clear();
				m_UdpSendTsAddrlist = m_TempUdpSendTsAddrlist;
			}
			if(m_bClealAddr)
			{
				m_bClealAddr = false;
				m_UdpSendTsAddrlist.clear();
				m_TempUdpSendTsAddrlist.clear();
			}
			//
			if (m_UdpSendTsAddrlist.size()<=0)
			{
				msg_queue()->flush();
				OSFunction::Sleep(0,10);
				continue;
			}

			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));

			if (getq(mb,&OutTime) != -1 && mb != NULL)
			{
				if (mb->msg_type() == ACE_Message_Block::MB_BREAK)//判断是否是终止发送消息
				{
					mb->release();
					OSFunction::Sleep(0,10);
					continue;	
				}
				if (mb->length() > 0)
				{
					ACE_Guard<ACE_Thread_Mutex> guard(QueueMutex);
					ACE_Time_Value nowaittime (ACE_Time_Value::zero);
					std::vector<UdpSendTsAddr>::iterator ptr = m_UdpSendTsAddrlist.begin();
					for (;ptr!=m_UdpSendTsAddrlist.end();)
					{
						ACE_INET_Addr remoteAddr(ptr->port,ptr->destip.c_str());
						MultiDeviceSock.send(mb->rd_ptr(),mb->length(),remoteAddr);
						ptr++;
					}
				}
				mb->release();
			}
			else
			{
				OSFunction::Sleep(0,10);
			}
		}
		catch (...)
		{
		}
	}
	bFlag = false;
	return 0;
}

void UdpTsSender::SetClient(std::vector<UdpSendTsAddr> UdpSendTsAddrlist)	
{
	m_TempUdpSendTsAddrlist = UdpSendTsAddrlist;
	m_bSetAddr = true;
}
bool UdpTsSender::ClearClient()
{
	m_bClealAddr = true;
	return true;
}
int UdpTsSender::Stop()
{
	bFlag = false;
	ACE_Message_Block* pMsgBreak;
	ACE_NEW_NORETURN(pMsgBreak, ACE_Message_Block (0, ACE_Message_Block::MB_BREAK) );
	msg_queue()->flush();
	msg_queue()->enqueue_head(pMsgBreak);
	this->wait();

	return 0;
}



