#pragma once
#ifndef TCPCLIENT_H_H_H
#define TCPCLIENT_H_H_H

#include <iostream>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
using namespace std;

class CTcpClt
{
public:
	CTcpClt(void);
	~CTcpClt(void);
	bool Connect2Srv(const string strIp, const int iPort);
	bool SendMsg(char* pBuf, int iBufLen);
	bool RecvMsg(char* pBuf, int& iBufLen);
	void CloseSocket(void);

public:
	SOCKET m_sSocket;
};

#endif
