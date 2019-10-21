#pragma once




#include "./DeviceRelatedTask.h"
#include "../Foundation/Typedef.h"



class EncryptCheckTask : public DeviceRelatedTask
{
public:
	EncryptCheckTask(std::string strTaskXml);
	
	virtual ~EncryptCheckTask(void);


public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

public:

private:
	bool ChangeCardConfig();
	bool RecoverCardConfig();
	bool CheckEncryptInfo();

	void GetAllChanInfo();
	std::string CreateTunerXml(sChannelInfo chanInfo);
	bool AcceptCheckInfo();

	bool RecvEncryInfo();
	bool RecvInfo(char* Info,const int buflen,ssize_t& rcvlen);
	bool ProcessEncryInfo(const char* buf,const int len);
	bool CollectAlarmType(int AlarmType);

private:
	int m_nListenPort;
	std::string m_strCardIp;
	std::string m_strCardNum;
	std::string m_strPosition;
	bool m_bChangeCardConfig;
	//
	string m_strMonitorIndex;
	string m_strCardIndex;
	string m_strChanIndex;
	string m_strMonitorip;
	string m_strMonitorport;
	//
	std::vector<sChannelInfo> m_vecChanInfo;

	ACE_SOCK_Stream m_client;
	ACE_INET_Addr m_clientAddr;

	//std::map<std::string, std::string> m_mapEncryInfo;

	bool m_bVideoCAFailed;
	bool m_bAudioCAFailed;
	bool m_bNoVideo;
	bool m_bNoAudio;
	bool m_bUnLock;

	//int m_nCAFailedCount;

	//bool m_bAllChecked;

};
