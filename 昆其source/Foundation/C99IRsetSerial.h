//红外学习串口
#ifndef _99IRSETSERIAL_H_
#define _99IRSETSERIAL_H_
#pragma once
#include <Windows.h>
#include <iostream>
using namespace std;

const int iBufSize = 2048;
class C99IRsetSerial
{
public:
	C99IRsetSerial(void);
	~C99IRsetSerial(void);
	bool InitSerialDevice(string str);
	void CloseSerial(void);
	bool SendSerial(unsigned char* cSend, int iSendLen, 
		unsigned char*& cRecv, int& iRecvLen);
	//获取 发送 数据
	unsigned char* GetSendBuf(unsigned char* cCmdCode,
		unsigned char* buf, int buflen);
	//接收到的数据 进行解析
	unsigned char RecvBufParser(unsigned char* pRecvBuf, int iBufLen,
		int& iContestsLen, unsigned char* pContests = NULL);

	//状态码 解析
	string StateParser(unsigned char cState);

	//命令码 解析
	unsigned char CmdCodeParser(unsigned char* pCmdCode);
	//内容 解析
	string ContestsParser(unsigned char CmdCodeLow,
		unsigned char* cContests, int iContestsLen);


	//读取文件长度
	int ReadFrameLen(unsigned char* pBuf);
	//读取模块ID号 帧
	bool ReadIDCode(unsigned char& ucIDCode);
	//读取间隔时间 
	bool ReadCmdIntervalTime(unsigned char ucIDCode, int& iIntervalTime);

	/*将字符串的值 2个字符一组 转化成另一个字符串的地址
	**例： 值为(四个字符) "0001" －－－转成－－－ 0x00, 0x01 (两个地址)
	*/
	unsigned char* char2mem(unsigned char* p, int len, int &iOutlen);

	/*将字符串的地址 转化成 另一个字符串的值
	**例： 0x00, 0x01 (两个地址) －－－转成－－－ 值为(四个字符) "0001"
	*/
	unsigned char* mem2char(unsigned char* p, int len, int &iOutlen);

private:
	bool WriteCom(unsigned char* pBuf, int iBufLen);
	bool ReadCom(unsigned char* pBuf, int iBufLen);
	//获取 校验码
	unsigned char GetCheckCode(unsigned char* pBuf, int iBufLen);

	//获取 帧长度
	unsigned char* GetFrameLength(int iContentsLen, int& iFrameLen);

private:
	HANDLE m_hCom;
};

#endif
