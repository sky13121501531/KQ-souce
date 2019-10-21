#include "TcpClt.h"
#include "../Foundation/TimeUtil.h"

CTcpClt::CTcpClt(void)
{
	m_sSocket = 0;
}

CTcpClt::~CTcpClt(void)
{
	CloseSocket();
	WSACleanup();

}

bool CTcpClt::Connect2Srv(const string strIp, const int iPort)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(strIp.c_str());
	addr.sin_port = htons(iPort);

	m_sSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(m_sSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR)) < 0)
	{
		CloseSocket();
		return false;
	}

	return true;

}
bool CTcpClt::SendMsg(char* pBuf, int iBufLen)
{
	if (pBuf == NULL || iBufLen <= 0)
	{
		CloseSocket();
		return false;
	}

	if (send(m_sSocket, pBuf, iBufLen, 0) <= 0)
	{
		CloseSocket();
		return false;
	}

	return true;
}
bool CTcpClt::RecvMsg(char* pBuf, int& iBufLen)
{
	int iRecvLen = 0;
	iRecvLen = recv(m_sSocket, pBuf, iBufLen, 0);
	iBufLen = iRecvLen;
	if (iRecvLen <= 0)
	{
		CloseSocket();
		return false;
	}
	
	return true;
}
void CTcpClt::CloseSocket(void)
{
	if (m_sSocket != INVALID_SOCKET)
	{
		closesocket(m_sSocket);
		m_sSocket = INVALID_SOCKET;
	}

	return ;
}
