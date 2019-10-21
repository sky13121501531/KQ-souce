///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvChanScanThread.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "UdpRecvChanScanThread.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
//
//
UdpRecvChanScanThread::~UdpRecvChanScanThread()
{
}
UdpRecvChanScanThread::UdpRecvChanScanThread()
{
	m_DataBuf = NULL;
}

int UdpRecvChanScanThread::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpRecvChanScanThread::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
int UdpRecvChanScanThread::svc()
{
	long DataBufSize = 64*1024*1024;
	m_DataBuf = new unsigned char[DataBufSize];
	memset(m_DataBuf,0,DataBufSize);
	m_BufDataNum = 0;
	//
	int ReadSize = 1400;
	unsigned char RcvBuf[1400];
	memset(RcvBuf,0,ReadSize);
	//
	ACE_Time_Value RecvTimeOut(5);
	//
	string curudpIp;
	string curudpPort;
	//
	m_bActive = false;
	bool reset = false;
	bool bact = false;
	//
	while (bFlag)
	{
		if((m_UdpIP.size()>0)&&(m_UdpPort.size()>0))
		{
			if((curudpIp != m_UdpIP)||(curudpPort != m_UdpPort)||m_bActive != bact)
			{
				bact = m_bActive;
				if(bact)
				{
					m_BufDataNum = 0;
					memset(m_DataBuf,0,DataBufSize);
					//
					string leavemulitcase = curudpIp + string(":")+ curudpPort;
					ACE_INET_Addr leavemulticast_addr_(leavemulitcase.c_str()); 
					DeviceSock.leave(leavemulticast_addr_,(PROPMANAGER::instance()->GetDeviceIp()).c_str());
					//
					curudpIp = m_UdpIP;
					curudpPort = m_UdpPort;
					//
					string mulitcase = m_UdpIP + string(":")+ m_UdpPort;
					ACE_INET_Addr multicast_addr_(mulitcase.c_str()); 
					//DeviceSock.join(multicast_addr_);
					DeviceSock.join(multicast_addr_,1,(PROPMANAGER::instance()->GetDeviceIp()).c_str());
					DeviceSock.set_option(IP_MULTICAST_TTL, 5);
					int nSocketBuffSize = 1024*500;
					ACE_SOCK* sk= &DeviceSock;
					sk->set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int));
				}
			}
			//
			memset(RcvBuf,0,ReadSize);
			int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
			if(RecvLen <= 0)
			{
				OSFunction::Sleep(0,10);
				continue;
			}
			else
			{
				if(m_bActive)
				{
					if(m_BufDataNum+RecvLen < DataBufSize)
					{
						if(m_DataBuf != NULL)
						{
							memcpy(m_DataBuf+m_BufDataNum,RcvBuf,RecvLen);
							m_BufDataNum = m_BufDataNum + RecvLen;
						}
					}
					else
					{
						//暂不处理
					}
					reset = true;
				}
				else
				{
					if(reset)
					{
						memset(m_DataBuf,0,DataBufSize);
						m_BufDataNum = 0;
						reset = false;
					}
					OSFunction::Sleep(0,2);
				}
			}
		}
		else
		{
			OSFunction::Sleep(0,10);
		}
	}
	delete []m_DataBuf;
	bFlag = false;
	return 0;
}
int UdpRecvChanScanThread::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
