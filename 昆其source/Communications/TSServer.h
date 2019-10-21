#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "RTSPServerInter.h"
#include <map>
#include <string>
class TSServer
{
public:
	TSServer(void);
	~TSServer(void);
public:
	// 创建RTSP服务器
	bool CreateTsServer(char *szIP, int nPort);
	// 创建实时视频流
	int CreateTsStream(char *szStrName);
	// 删除实时视频流
	bool DeleteTsStream(char *szStrName);
	bool DeleteTsStream(int nStrID);
	// 给实时视频流添加数据
	bool AddTsData(std::string szStrName, char *szBuf, int nDataLen);
	bool DelTsData(std::string szStrName);
	// 释放RTSP服务器
	bool ReleaseTsServer();
	// 设置文件访问路径
	bool SetTsFilePath(char *szFilePath);
private:
	std::map<std::string,int> mapStreamID;//流id与名字对应项
	//ACE_Thread_Mutex StreamMutex;
};
typedef ACE_Singleton<TSServer,ACE_Mutex>  TSSERVERMGR;