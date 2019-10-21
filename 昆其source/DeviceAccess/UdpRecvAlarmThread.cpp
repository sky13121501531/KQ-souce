///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvAlarmThread.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "UdpRecvAlarmThread.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Alarm/AlarmMgr.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
//
extern int g_realstreamusedev;
standardRFResult g_standardRFResult[100];
time_t AlarmTime;
//
enum Tr290EventType
{
	TR_UNDEF = 0,
	TR_TSBW,
	TR_PIDBW,
	TR_TSSYNC_LST,
	TR_TSSYNC_ERR,
	TR_TSPID_ERR,
	TR_TSCNT_ERR,
	TR_TSTX_ERR,
	TR_TSCA_ERR,
	TR_TSPID_LST,
	TR_PCR0,
	TR_PCR1,
	TR_PTS,
	TR_TAB,
	TR_RFSET,
	TR_RFSAT,
	TR_RFIQ
};
struct TR290Event
{
	unsigned char evt_type;
	unsigned char evt_unused;
	union EVT_PID_
	{
		unsigned short evt_tepid;
		unsigned short rf_qam;
		struct RF_IQ_
		{
			unsigned char iqy;
			unsigned char iqx;
		}rf_iq;
	}evt_pid;
	//
	union EVT_TIM_
	{
		unsigned int evt_time;
		unsigned int rf_ber;
	}evt_tim;
	//
	union EXT_VAL_
	{
		unsigned int tspkt_cnt;
		unsigned int synclst_cnt;
		unsigned int syncerr_cnt;
		unsigned int piderr_cnt;
		unsigned int cnterr_cnt;
		unsigned int txerr_cnt;
		unsigned int caerr_cnt;
		unsigned int pcr_cnt;
		unsigned int pts_cnt;
		//
		struct rf_set_
		{
			unsigned short rf_synbol;
			unsigned short rf_frequency;

		}rf_set;
		//
		struct rf_sat0_
		{
			unsigned char rf_mse;
			unsigned char rf_snr;
			unsigned char rf_strength;
			unsigned char rf_locked;
		}rf_sat0;
		//
		struct rf_sat1_
		{
			unsigned char rf_acc;
			unsigned char rf_afc;
			unsigned char rf_agcif;
			unsigned char rf_agccrf;
		}rf_sat1;
		//
		struct tab_info_
		{
			unsigned char tabid;
			unsigned char unused0;
			unsigned char unused1;
			unsigned char crcerr;
		}tab_info;
		//
	}ext_val;
};
//
UdpRecvAlarmThread::~UdpRecvAlarmThread()
{
}
UdpRecvAlarmThread::UdpRecvAlarmThread(string ip,string port,int wtype)
{
	iWorktype = wtype;
	string mulitcase = ip + string(":")+ port;
	ACE_INET_Addr multicast_addr_(mulitcase.c_str()); 
	DeviceSock.join(multicast_addr_);
	DeviceSock.set_option(IP_MULTICAST_TTL, 5);
	int nSocketBuffSize = 1024*500;
	ACE_SOCK* sk= &DeviceSock;
	sk->set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int));
	//
	memset(m_picalarm,-1,sizeof(char)*6);
	memset(m_picalarmtimes,25,sizeof(char)*3);
	memset(m_picalarmtime_t,0,sizeof(time_t)*3);
	//
	m_errCnttime = time(0);
	m_errPattime = time(0);
	m_errPmttime = time(0);
}

int UdpRecvAlarmThread::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpRecvAlarmThread::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
int UdpRecvAlarmThread::svc()
{
	int ReadSize = 480+2;
	if(iWorktype == 1)
	{
		ReadSize = 60;
	}
	//
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		ReadSize = 1400;
	}
	//
	m_bSetCoderAlarm = false;
	unsigned char* RcvBuf	= new unsigned char[ReadSize];		
	memset(RcvBuf,0,ReadSize);
	ACE_Time_Value RecvTimeOut(3);
	//
	sCheckParam sCheck;
	sCheck.AlarmType	= ALARM_PROGRAM;
	sCheck.DVBType		= m_dvbtype;
	sCheck.ChannelID	= m_strchannelid;
	sCheck.Freq			= m_strfreq;
	sCheck.ServiceID	= m_strServiceID;
	sCheck.STD			= "";
	sCheck.SymbolRate	= "";
	sCheck.TypedValue	= "";
	sCheck.DeviceID		= m_strdeviceid;
	//
	int DeviceID =  StrUtil::Str2Int(m_strdeviceid);
	//
	unsigned int m_PatTime;//PAT错误时间记录,两PAT间时间超过500ms既PAT错误 初始化0
	unsigned int m_290errcount[2];//g_290errcount[0]表示同步丢失错误计数 g_290errcount[1] 表示同步字节错误计数 初始化0
	vector<TR290Event> m_PMTErrList;//记录PMTPID和时间
	vector<TR290Event> m_CNTErrList;//记录CNT错误PID和个数
	m_PatTime = 0;
	m_290errcount[0] = 0;
	m_290errcount[1] = 0;
	time_t startbacktime = 0;
	//
	for(int i = 0;i<7;++i)
	{
		if(i<6)
		{
			for(int j=0;j<3;++j)
			{
				aud_isAlarm[i][j] = true;
			}
		}
		pic_isAlarm[i] = true;
	}
	while (bFlag)
	{
		if((iWorktype == 1)&&((!m_bSetCoderAlarm)||(g_realstreamusedev == DeviceID)))
		{
			Sleep(3000);
			continue;
		}
		//
		
		memset(RcvBuf,0,ReadSize);
		int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
		if(RecvLen <= 0)
		{
			continue;
		}
		else
		{
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "3.0")
			{
				if(RecvLen == 59)
				{
					//vid=%d&aud0=%d&aud1=%d&aud2=%d&aud3=%d&aud4=%d&aud5=%d&signalsta=%d
					//ProMutex.acquire();
					sCheck.AlarmType = ALARM_PROGRAM;
					string tem = (char*)RcvBuf;
					//cout<<"////////////////=:"<<tem<<endl;
					//APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_WARNNING,tem,LOG_OUTPUT_BOTH);
					if(tem.size()>0)
					{
						/*vid 0正常、1黑场、2静帧、3视频丢失、4视频解码异常、5彩场、6马赛克、7彩条。 
						aud0~ aud5 0正常、1音量过低、2音量过高、3音频丢失(如果声道不存在值为0) 
						signalsta 0 为无信号1 为有信号 */
						char va[32]={0};
						sprintf(va,"%c",RcvBuf[4]);
						char *arrAlarm[7] = {"黑场","静帧","视频丢失","视频解码异常","彩场","马赛克","彩条"};
						char *arrid[7] = {"0x1","0x2","0x3","0x4","0x5","0x6","0x7"};
						int tempNum = atoi(va);
						for(int i = 0;i<7;++i)
						{
							if(tempNum==0)
							{
								if(pic_isAlarm[i])
								{
									pic_isAlarm[i] = false;
									sCheck.mode = "1";//全部正常
									sCheck.TypeDesc = arrAlarm[i];
									sCheck.TypeID = arrid[i];
									sCheck.CheckTime	= time(0);
									ALARMMGR::instance()->CheckAlarm(sCheck,true);
								}
							}
							else
							{
								int RetNum = tempNum - 1;
								if(RetNum==i)
								{
									if(!pic_isAlarm[i])
									{
										pic_isAlarm[i] = true;
										sCheck.mode = "0";//一种报警
										sCheck.TypeDesc = arrAlarm[i];
										sCheck.TypeID = arrid[i];
										sCheck.CheckTime	= time(0);
										ALARMMGR::instance()->CheckAlarm(sCheck,true);
										break;
									}
								}
							}
						}
						//aud0=%d&aud1=%d&aud2=%d&aud3=%d&aud4=%d&aud5=%d
						/*aud0~ aud5 0正常、1音频丢失、2音量过高、3音量过低(如果声道不存在值为0) 
						signalsta 0 为无信号1 为有信号 */
						char *arrAudio[3] = {"音频丢失","音量过高","音量过低"};
						char *arrAudioID[3] = {"0x8","0x9","0x10"};
						tem = tem.substr(6,tem.length()-18);
						char *pAudio = (char*)tem.c_str();
						for(int i=0;i<tem.size();++i)
						{
							char AC[16] ={0};
							char AudioNum[16] ={0};
							if(*pAudio=='=')
							{
								char *ptemp = pAudio;
								char *backtemp = pAudio;
								++ptemp;
								--backtemp;
								sprintf(AC,"%c",ptemp[0]);
								sprintf(AudioNum,"%c",backtemp[0]);
								int tempID = atoi(AC);//报警ID
								int tempAudioNum = atoi(AudioNum);//声道号
								std::string strNum = "";
								int numCot = tempAudioNum + 1;
								switch (numCot)
								{
								case 1:strNum="第1个声道"; break;
								case 2:strNum="第2个声道"; break; 
								case 3:strNum="第3个声道"; break;
								case 4:strNum="第4个声道"; break;
								case 5:strNum="第5个声道"; break;
								case 6:strNum="第6个声道"; break;
								default:
									break;
								}
								for(int j =0;j<6;++j)
								{
									if(j==tempAudioNum)
									{
										for(int i=0;i<3;++i)
										{
											//
											if(tempID!=0)
											{
												int tpID = tempID - 1;
												if(tpID==i)
												{
													if(!aud_isAlarm[j][i])
													{
														aud_isAlarm[j][i] = true;
														sCheck.mode = "0";
														sCheck.TypeDesc = strNum + arrAudio[i];
														sCheck.TypeID = arrAudioID[i];
														sCheck.CheckTime	= time(0);
														ALARMMGR::instance()->CheckAlarm(sCheck,true);
														//break;
													}
												}
											}
											else
											{
												if(aud_isAlarm[j][i])
												{
													aud_isAlarm[j][i] = false;
													sCheck.mode = "1";
													sCheck.TypeDesc = strNum + arrAudio[i];
													sCheck.TypeID = arrAudioID[i];
													sCheck.CheckTime	= time(0);
													ALARMMGR::instance()->CheckAlarm(sCheck,true);
												}
											}
										}
										break;
									}
								}
							}
							pAudio++;
						}
					}
					//ProMutex.release();
				}	
			}///////////******
		}
	}
	bFlag = false;
	return 0;
}
int UdpRecvAlarmThread::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
