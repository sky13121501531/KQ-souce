#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <string>
#include <vector>
#include "../Foundation/TypeDef.h"
class RTSPServer;
class CStreamManager;
// RTSP服务器指针
extern RTSPServer* g_rtspServer;
// 流管理器指针
extern CStreamManager* g_pStrManager;
// 启动停止标识
extern char		g_cStart;
// 创建实时视频流
int RTSPCreateStream(char *szStrName);
// 删除实时视频流
bool RTSPDeleteStream(char *szStrName);
bool RTSPDeleteStream(int nStrID);
// 给实时视频流添加数据
bool RTSPAddData(char *szStrName, char *szBuf, int nDataLen);
bool RTSPAddData(int nStrID, char *szBuf, int nDataLen);
// 清空实时流
bool RTSPClearData(char *szStrName);
bool RTSPClearData(int nStrID);

// 创建RTSP服务器
bool RTSPCreateServer(char *szIP, int nPort = 554);
// 释放RTSP服务器
bool RTSPReleaseServer();
// 设置文件访问路径
bool RTSPSetFilePath(char *szFilePath);

//////////////////////////////////////////////////////////////////////////
//	用户管理接口
//	目前只有通道号、地址和端口有用

// 取得流用户的数目
int RTSPStrUserCount(int nStrID);
int RTSPStrUserCount(char *szStrName);

// 取得给定流的所有用户的信息
bool RTSPGetStrUser(int nStrID, std::vector<sVedioUserInfo> &vUsersInfo);
bool RTSPGetStrUser(char *szStrName, std::vector<sVedioUserInfo> &vUsersInfo);

#endif