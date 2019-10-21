#include "C99IRsetSerial.h"

C99IRsetSerial::C99IRsetSerial(void)
{
	m_hCom = 0;
}

C99IRsetSerial::~C99IRsetSerial(void)
{
	CloseSerial();
}
void C99IRsetSerial::CloseSerial(void)
{
	CloseHandle(m_hCom);
	m_hCom=INVALID_HANDLE_VALUE;
	return;
}

bool C99IRsetSerial::InitSerialDevice(string str)
{
	DCB dcb;
	COMMTIMEOUTS timeOuts; //串口操作超时结构体
	DWORD dwError = 0;
	m_hCom = CreateFile(str.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}
	ZeroMemory(&dcb, sizeof(DCB));
	ZeroMemory(&timeOuts, sizeof(COMMTIMEOUTS));

	//设置接收缓冲区和发送缓冲区的大小
	if (!SetupComm(m_hCom, 2048, 2048)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	dcb.BaudRate	= 9600;         //设定波特率为9600 bps
	dcb.ByteSize	= 8;              //设定数据位为8
	dcb.Parity		= NOPARITY;       //设定无奇偶校验
	dcb.StopBits	= ONESTOPBIT;     //设定一个停止位
	dcb.DCBlength = sizeof(DCB);

	////设置串行设备控制参数
	if (!SetCommState(m_hCom, &dcb)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	timeOuts.ReadIntervalTimeout = 20;
	timeOuts.ReadTotalTimeoutConstant = 30;
	timeOuts.ReadTotalTimeoutMultiplier = 2;
	timeOuts.WriteTotalTimeoutConstant = 0;
	timeOuts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(m_hCom, &timeOuts)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	return true;
}


bool C99IRsetSerial::SendSerial(unsigned char* cSend, int iSendLen, 
								unsigned char*& cRecv, int& iRecvLen)
{
	bool bRet = false;
	bRet = WriteCom(cSend, iSendLen);
	if (!bRet)
		return false;

	unsigned char buf[iBufSize] = {0};
	bRet = ReadCom(buf, iBufSize);

	if (!bRet )
		return false;
	int ibuflen = ReadFrameLen(buf);
	if (ibuflen <= 0)
		return false;
	unsigned char* p = new unsigned char[ibuflen+1];
	memset(p, 0, ibuflen+1);
	memcpy(p, buf, ibuflen);
	cRecv = p;
	iRecvLen = ibuflen;
	
	return true;
}

bool C99IRsetSerial::WriteCom(unsigned char* pBuf, int iBufLen)
{

	DWORD tmp;
	//停止发送和接收数据，并清除发送和接收缓冲区 
	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!WriteFile(m_hCom, pBuf, iBufLen, &tmp, NULL))
	{
		return false;
	}

	return true;
}

bool C99IRsetSerial::ReadCom(unsigned char* pBuf, int iBufLen)
{
	DWORD tmp ;
	if (!ReadFile(m_hCom, pBuf, iBufLen, &tmp, NULL))
	{
		return false;
	}

	return true;
}

unsigned char* C99IRsetSerial::GetSendBuf(unsigned char* cCmdCode,
										  unsigned char* buf, int buflen)
{
	unsigned char cFrameHead[2] = {0xF7, 0x7F};
	unsigned char cFrameLen[2] = {0};
	unsigned char cCheckCode = 0;
	int  i1 = 0;
	unsigned char* bfl= NULL;
	bfl = GetFrameLength(buflen,i1);
	memcpy(cFrameLen, bfl, 2);
	delete bfl;
	bfl = NULL;

	unsigned char* pbuf = new unsigned char[i1];
	memset(pbuf, 0, i1);
	int i2 = 0;
	memcpy(pbuf+i2,	cFrameHead, 2);
	i2 += 2;
	memcpy(pbuf+i2, cFrameLen, 2);
	i2 += 2;
	memcpy(pbuf+i2, cCmdCode, 2);
	i2 += 2;
	memcpy(pbuf+i2, buf, buflen);
	i2 += buflen;
	cCheckCode = GetCheckCode(pbuf, i1);
	memcpy(pbuf+i2, &cCheckCode, 1);

	return pbuf;
}



//获取 校验码
unsigned char C99IRsetSerial::GetCheckCode(unsigned char* pBuf, int iBufLen)
{
	unsigned char iNum = 0;
	for (int i = 0; i < iBufLen; i++)
	{
		iNum += (unsigned char)(pBuf[i]);
	}
	iNum += 1;
	unsigned char ch = iNum & 0xFF;
	return ch;
}


//获取 帧长度
unsigned char* C99IRsetSerial::GetFrameLength(int iContentsLen, int& iFrameLen)
{
	unsigned char* p = new unsigned char[2];
	memset(p, 0, 2);
	int inum = 7 + iContentsLen; //7 = 2+2+2+1;
	int i1 = inum >> 8;
	int i2 = inum & 0xFF;
	*p = (unsigned char)i1;
	*(p+1) = (unsigned char)i2;
	iFrameLen = inum;
	return p;
}


//读取文件长度
int C99IRsetSerial::ReadFrameLen(unsigned char* pBuf)
{
	if (pBuf == NULL)
		return -1;
	int iLen = -1;
	iLen = (*(pBuf+2))*256 + *(pBuf+3);
	return iLen;
}

unsigned char* C99IRsetSerial::char2mem(unsigned char* p, int len, int &iOutlen)
{
	if (len <= 1)
		return NULL;
	int i1 = len >> 1;
	unsigned char* p1 = new unsigned char[i1+1];
	memset(p1, 0, i1+1);
	for (int i = 0; i < i1; i++)
	{
		char b[3] = {0};
		memcpy(b, p+2*i, 2);
		int inum = 0;
		sscanf(b, "%02x", &inum);
		*(p1+i) = inum;
	}
	iOutlen = i1;
	return p1;
}

unsigned char* C99IRsetSerial::mem2char(unsigned char* p, int len, int &iOutlen)
{
	if (len == 0)
		return NULL;
	unsigned char buf[iBufSize] = {0};
	memcpy(buf, p, len);
	int i1 = len*2;
	unsigned char* pb1 = new unsigned  char[i1+1];
	memset(pb1, 0, i1+1);
	for (int i = 0; i < len; i++)
	{
		char b11[3] = {0};
		sprintf(b11, "%02x", buf[i]);
		memcpy(pb1+2*i, b11, 2);
	}
	iOutlen = i1;
	return pb1;
}



//状态码 解析
string C99IRsetSerial::StateParser(unsigned char cState)
{
	string buf = "";
	switch(cState)
	{
	case 0x00:
		buf = "正常，成功";
		break;
	case 0x01:
		buf = "存储出错";
		break;
	case 0x02:
		buf = "该按键未学习";
		break;
	case 0x03:
		buf = "帧内容参数错误 ";
		break;
	case 0x04:
		buf = "不支持此类通讯";
		break;
	case 0x05:
		buf = "通讯帧数据长度不符";
		break;
	case 0x06:
		buf = "响应超时";
		break;
	case 0x07:
		buf = "校验和出错";
		break;
	case 0x08:
		buf = "版本错误";
		break;
	case 0x09:
		buf = "当前模块忙";
		break;
	case 0x0A:
		buf = "其他错误";
		break;
	default:
		break;
	}
	return buf;
}

//命令码 解析
unsigned char C99IRsetSerial::CmdCodeParser(unsigned char* pCmdCode)
{
	if (pCmdCode == NULL)
		return NULL;
	unsigned char cmdcode = *(pCmdCode+1);

	return cmdcode;
}

string C99IRsetSerial::ContestsParser(unsigned char CmdCodeLow,
					  unsigned char* cContests, int iContestsLen)
{
	if (cContests == NULL)
		return NULL;
	string str;
	switch(CmdCodeLow)
	{
	case 0x81:
		{
			str = "学习红外指令 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x82:
		{
			str = "单个红外发射 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x83:
		{
			str = "两个红外发射 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x84:
		{
			str = "遍历红外发射 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x85:
		{
			str = "设置红外发送间隔时间 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x86:
		{
			str = "读取单个红外按键存储码 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x87:
		{
			str = "写入单个红外按键存储码 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x88:
		{
			str = "批量写入红外码指令 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x89:
		{
			str = "批量读取红外码指令 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8A:
		{
			str = "设置LED2工作状态灯 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8B:
		{
			str = "设置波特率 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8C:
		{
			str = "获取软件版本号 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8D:
		{
			str = "校验码检查设置 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8E:
		{
			str = "查询校验码状态 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x91:
		{
			str = "恢复出厂设置 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x92:
		{
			str = "查询LED2灯工作状态 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x93:
		{
			str = "查询间隔时间 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x94:
		{
			str = "设置模块ID号 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x95:
		{
			str = "读取模块ID号 应答帧: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	}

	return str;
}


//读取模块ID号 帧
bool C99IRsetSerial::ReadIDCode(unsigned char& ucIDCode)
{
	unsigned char cIDNum[10] = {0xF7, 0x7F, 0x00, 0x07, 0x00, 0x15, 0x93};
	unsigned char* pSend = new unsigned char[8];
	memset(pSend, 0, 8);
	memcpy(pSend, cIDNum, 7);
	unsigned char* pRecv = NULL;
	int iOutLen = 0;
	bool bRet = SendSerial(pSend, 7, pRecv, iOutLen);
	delete []pSend;
	pSend = NULL;
	if (bRet && (pRecv != NULL) )
	{
		int ilen = ReadFrameLen(pRecv);
		if (ilen == iOutLen)
		{
			unsigned char cIDCode = *(pRecv+7);
			ucIDCode = cIDCode;
		}
		delete []pRecv;
		pRecv = NULL;
		return true;
	}
	return false;
}

//读取间隔时间 
bool C99IRsetSerial::ReadCmdIntervalTime(unsigned char ucIDCode, int& iIntervalTime)
{
	unsigned char cCmdCode[2] = {0};
	cCmdCode[0] = ucIDCode;
	cCmdCode[1] = 0x13;		//0x13 查询间隔时间指令
	unsigned char* pFrames = GetSendBuf(cCmdCode, NULL, 0);
	int iFramesLen = ReadFrameLen(pFrames);
	unsigned char* pRecv = NULL;
	int iOutLen = 0;
	bool bRet = SendSerial(pFrames, iFramesLen, pRecv, iOutLen);
	delete []pFrames;
	pFrames = NULL;
	if (bRet && (pRecv != NULL) )
	{
		int ilen = ReadFrameLen(pRecv);
		if (ilen == iOutLen)
		{
			unsigned char cIntervalTime = *(pRecv+7);
			iIntervalTime = (int)cIntervalTime;
		}
		delete []pRecv;
		pRecv = NULL;
		return true;
	}
	return false;
}


//接收到的数据 进行解析
unsigned char C99IRsetSerial::RecvBufParser(unsigned char* pRecvBuf, int iBufLen,
											 int& iContestsLen, unsigned char* pContests )
{
	if (pRecvBuf == NULL)
		return 0;
	
	int iFrameLen = ReadFrameLen(pRecvBuf);
	if (iFrameLen != iBufLen)
		return 0;
	if (iFrameLen > 7)
	{
		int iContestsLen = iFrameLen -7;
		unsigned char cStateCode = *(pRecvBuf+6);
		if (iContestsLen > 1)
		{
			int ilen = iContestsLen - 1;
			unsigned char* pFrameContests = new unsigned char[ilen+1];
			memset(pFrameContests, 0, ilen+1);
			memcpy(pFrameContests, pRecvBuf+7, ilen);
			pContests = pFrameContests;
			iContestsLen = ilen;
		}
		return cStateCode;		
	}


	return 0;
}
