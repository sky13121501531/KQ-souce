#include "QuaDevInfoTCPDeviceAccess.h"
#include "../Foundation/StrUtil.h"


CQuaDevInfoTCPDeviceAccess::CQuaDevInfoTCPDeviceAccess(int deviceid, std::string strIP, int nPort) : TCPDeviceAccess(deviceid, strIP, nPort)
{
	m_pTcpClt = NULL;
	m_strIP = strIP;
	m_iPort = nPort;
	m_pTcpClt = new CTcpClt();
}
CQuaDevInfoTCPDeviceAccess::~CQuaDevInfoTCPDeviceAccess(void)
{
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if (m_pTcpClt != NULL)
	{
		delete m_pTcpClt;
		m_pTcpClt = NULL;
	}
}

//场强 测量
bool CQuaDevInfoTCPDeviceAccess::SendQuaDevInfoCmd(string s_freq, string &s_out_cmd)
{
	string freq = s_freq;
	if (freq.length() > 3)
	{
		freq = freq.substr(0, 3);
	}
	char c_in_cmd[50] = {0};
	sprintf(c_in_cmd, "qulity: %s, aaaa", freq.c_str());
	int i_incmd_len = strlen(c_in_cmd);
	char c_out_cmd[50] = {0};
	bool bret = SendCmdToServer((void*)c_in_cmd, i_incmd_len, (void*)c_out_cmd, 50);
	if (bret)
	{
		std::string str = c_out_cmd;
		int len = str.length();
		int ipos = str.find(":");
		s_out_cmd = str.substr(ipos+2, len);
	}

	return bret;
}

//频谱 测量
bool CQuaDevInfoTCPDeviceAccess::SendSpecDevInfoCmd(string s_freq_start, string s_freq_end,int i_step_len, string &s_out_cmd)
{
	string sfreqstart = s_freq_start;
	string sfreqend = s_freq_end;
	if (sfreqstart.length() > 3)//48000k - 954000k
	{
		float f_freqstart = StrUtil::Str2Float(sfreqstart);
		sfreqstart = StrUtil::Float2Str(f_freqstart/1000);
	}
	if (sfreqend.length() > 3)
	{
		float f_freqend = StrUtil::Str2Float(sfreqend);
		sfreqend = StrUtil::Float2Str(f_freqend/1000);
	}

	char inbuf[50] = {0};
	sprintf(inbuf, "spectrum:%s,%s,%d,aaa2", sfreqstart.c_str(), sfreqend.c_str(), i_step_len);
	int i_inbuf_len = strlen(inbuf);
	char* outbuf = new char[lQuaRecvBufSize];
	memset(outbuf,0,sizeof(char)*lQuaRecvBufSize);

	m_pTcpClt->Connect2Srv(m_strIP, m_iPort);
	bool bret = m_pTcpClt->SendMsg(inbuf, i_inbuf_len);
	int iRecvLen = lQuaRecvBufSize;
	bret = m_pTcpClt->RecvMsg(outbuf, iRecvLen);
	m_pTcpClt->CloseSocket();

	if (bret)
	{
		s_out_cmd = outbuf;
	}
	delete[] outbuf;
	outbuf = NULL;

	return bret;
}
