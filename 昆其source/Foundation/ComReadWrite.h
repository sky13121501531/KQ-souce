#ifndef __SERICAL_COM__
#define __SERICAL_COM__


#include "ace/Task.h"
#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
//#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include 		 <string.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <errno.h>      /*错误号定义*/
//#include "SystemImpl.h"
//#include "Thread.h"

#if  defined(WIN32) || defined(__WIN32__)
#include <Windows.h>
#include <time.h>    /*PPSIX 终端控制定义*/
#else
#define HRESULT 				long
#define DWORD   				unsigned long
#define S_OK                                   	((HRESULT)0x00000000L)
#define S_FALSE            											((HRESULT)0x00000001L)
#include     <termios.h>    /*PPSIX 终端控制定义*/
#endif        

#if  defined(WIN32) || defined(__WIN32__)
	class CComReadWrite
	{
	private:
		HANDLE hCom;  //串行设备句柄
		static ACE_Thread_Mutex Mutex;
	public:
		HRESULT InitDevice(int comport=1, DWORD dwbaud=9600);
		HRESULT CloseDevice();
		HRESULT WriteCom(char* str,int count);
		HRESULT ReadCom(char* str,int count,int nRetryCount=10);
	};
#else
	class CComReadWrite
	{
	private:
		int hCom;  //串行设备句柄
		bool initbool;
		int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop,int nTimeout);
	public:
		static SynClass* _Syn;		
		static CComReadWrite* _Instance;
		static CComReadWrite* GetInstance();
	public:
		CComReadWrite(){ initbool = false; };
		virtual ~CComReadWrite(){};
		//interface
	public:
		HRESULT InitDevice(int comport=1, unsigned long dwbaud=9600, unsigned long dReadIntervalTimeout=100, unsigned long dReadTotalTimeoutConstant=30);
		HRESULT CloseDevice();
		HRESULT WriteCom(char* str,int count,int nRetryCount=10);
		HRESULT ReadCom(char* str,int count,int nRetryCount=10);
	};
#endif
#endif
