///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvAlarmThread.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "UdpRecvDevCardThread.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../Foundation/StrUtil.h"
//
standardBoardCheckInfo g_standardBoardCheckInfo[ChassisMaxNum][SlotMaxNum];//机框号（范围0-7）卡槽号（范围0-31）
//
UdpRecvDevCardThread::UdpRecvDevCardThread()
{
}
UdpRecvDevCardThread::~UdpRecvDevCardThread()
{
}
UdpRecvDevCardThread::UdpRecvDevCardThread(string ip,string port)
{
	string mulitcase = ip + string(":")+ port;
	ACE_INET_Addr multicast_addr_(mulitcase.c_str()); 
	DeviceSock.join(multicast_addr_);
	DeviceSock.set_option(IP_MULTICAST_TTL, 5);
	int nSocketBuffSize = 1024*500;
	ACE_SOCK* sk= &DeviceSock;
	sk->set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int));
	//
}

int UdpRecvDevCardThread::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpRecvDevCardThread::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
int UdpRecvDevCardThread::svc()
{
	int ReadSize = 1400;
	unsigned char RcvBuf[1400];
	memset(RcvBuf,0,ReadSize);
	ACE_Time_Value RecvTimeOut(5);
	//
	bool end = false;
	//只操作本机箱卡
	string dip = PROPMANAGER::instance()->GetDeviceIp();
	int str172 = dip.find("172.");
	int ChassisIDIP = StrUtil::Str2Int(dip.substr(4,2));
	//
	while (bFlag)
	{
		if(end)
		{
			break;
		}
		memset(RcvBuf,0,ReadSize);
		int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
		if(RecvLen <= 0)
		{
			OSFunction::Sleep(0,10);
			continue;
		}
		else
		{
			for(int i=0;i<RecvLen;i++)
			{
				if(RcvBuf[i] == 0x5a)
				{
					for(int j=i+4;j<RecvLen;)
					{
						unsigned char DataType = RcvBuf[j];
						unsigned char DataLength = RcvBuf[j+1];
						if(DataType == 0x01)
						{
							if(DataLength == 8||DataLength == 12)
							{
								unsigned char BoardCheckInfo[8];
								unsigned char* temBuf = &(RcvBuf[j+4]);
								memcpy(&BoardCheckInfo,temBuf,8);
								//
								if((BoardCheckInfo[0]<ChassisMaxNum)&&(BoardCheckInfo[1]<SlotMaxNum))
								{
									if(!(g_standardBoardCheckInfo[BoardCheckInfo[0]][BoardCheckInfo[1]].ExistCard))
									{
										memcpy(&(g_standardBoardCheckInfo[BoardCheckInfo[0]][BoardCheckInfo[1]]),temBuf,8);
										g_standardBoardCheckInfo[BoardCheckInfo[0]][BoardCheckInfo[1]].ExistCard = true;
										if(BoardCheckInfo[3] == 2 || BoardCheckInfo[3] == 3)
										{
											SetBoardCheckInfo((char*)BoardCheckInfo);
										}
									}
								}
								//
								j=j+12;
							}
							else
							{
								j++;
							}
						}
						else if(DataType == 0x02)
						{
							if(DataLength == 16)
							{
								standardChannelResource chanresource;
								unsigned char* temBuf = &(RcvBuf[j+4]);
								memcpy(&chanresource,temBuf,16);
								if(chanresource.ucharIPAddr[0] != 172)
								{
									int tem=chanresource.ucharIPAddr[0];
									chanresource.ucharIPAddr[0]=chanresource.ucharIPAddr[3];
									chanresource.ucharIPAddr[3] = tem;
									tem=chanresource.ucharIPAddr[1];
									chanresource.ucharIPAddr[1]=chanresource.ucharIPAddr[2];
									chanresource.ucharIPAddr[2] = tem;
								}
								if(chanresource.ucharIPAddr[1] == ChassisIDIP)
								{
									SetChResourceInfo(chanresource);
								}
								j=j+20;
							}
							else
							{
								j++;
							}
						}
						else if(DataType == 0x03)
						{
						}
						else if(DataType == 0x04)
						{
						}
						else if(DataType == 0x05)
						{
						}
						else if(DataType == 0x06)
						{
						}
						else
						{
							break;
						}
					}
					break;
				}
			}
		}
	}
	bFlag = false;
	return 0;
}
int UdpRecvDevCardThread::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}

bool UdpRecvDevCardThread::SetBoardCheckInfo(char* boche)
{
	standardChannelResource chres;
	memcpy(&chres,boche,8);
	bool bfind = false;
	for(int i=0;i<vecstandardChannelResource.size();i++)
	{
		if(vecstandardChannelResource[i].ucharChassis==boche[0] && vecstandardChannelResource[i].ucharSlot==boche[1])
		{
			vecstandardChannelResource[i].startTime = time(0);
			bfind = true;
		}
	}
	if(!bfind)
	{
		vecstandardChannelResource.push_back(chres);
		PROPMANAGER::instance()->SetStandardChResourceInfo(chres);
	}
	return true;
}


bool UdpRecvDevCardThread::SetChResourceInfo(standardChannelResource chres)
{
	bool bfind = false;
	for(int i=0;i<vecstandardChannelResource.size();i++)
	{
		if(vecstandardChannelResource[i].ucharIPAddr[0]==chres.ucharIPAddr[0]
		&&vecstandardChannelResource[i].ucharIPAddr[1]==chres.ucharIPAddr[1]
		&&vecstandardChannelResource[i].ucharIPAddr[2]==chres.ucharIPAddr[2]
		&&vecstandardChannelResource[i].ucharIPAddr[3]==chres.ucharIPAddr[3]
		&&vecstandardChannelResource[i].ushortPort==chres.ushortPort)
		{
			vecstandardChannelResource[i].startTime = time(0);
			bfind = true;
		}
	}
	if(!bfind)
	{
		if (chres.ucharIPAddr[0] == 0)
		{
			cout<<"板卡配置IP地址错误"<<endl;
		}

		vecstandardChannelResource.push_back(chres);
		PROPMANAGER::instance()->SetStandardChResourceInfo(chres);
	}
	return true;
}

bool UdpRecvDevCardThread::GetChannelResource(string addr,string &startTime)
{
	bool ret = false;
	std::vector<standardChannelResource>::iterator iter = vecstandardChannelResource.begin();
	string channelResourceaddr;
	for (int i=0; iter!=vecstandardChannelResource.end(); iter++,i++)
	{
		channelResourceaddr =  StrUtil::Int2Str(vecstandardChannelResource[i].ucharIPAddr[0]);
		channelResourceaddr += ".";
		channelResourceaddr +=  StrUtil::Int2Str(vecstandardChannelResource[i].ucharIPAddr[1]);
		channelResourceaddr += ".";
		channelResourceaddr +=  StrUtil::Int2Str(vecstandardChannelResource[i].ucharIPAddr[2]);
		channelResourceaddr += ".";
		channelResourceaddr +=  StrUtil::Int2Str(vecstandardChannelResource[i].ucharIPAddr[3]);
		if (channelResourceaddr == addr)
		{
			startTime = StrUtil::Int2Str(iter->startTime);
			ret = true;
			break;
		}
	}
	return ret;
}