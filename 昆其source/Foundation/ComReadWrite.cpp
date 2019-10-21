#include "ComReadWrite.h"


#if  defined(WIN32) || defined(__WIN32__)

	HRESULT CComReadWrite::InitDevice(int comport, DWORD dwbaud)
	{
		DCB    dcb;
		COMMTIMEOUTS TimeOuts; //串口操作超时结构体
		DWORD dwError = 0;
		char name[16];
		sprintf(name,"COM%d",comport);
	   
		hCom = CreateFile( name,
						GENERIC_READ | GENERIC_WRITE,
						0,					// comm devices must be opened w/exclusive-access
						NULL,				// no security attributes
						OPEN_EXISTING,		// comm devices must use OPEN_EXISTING
						0,					// no overlapped I/O
						NULL			);		// hTemplate must be NULL for comm devices 

		if (hCom == INVALID_HANDLE_VALUE) 
		{
			return S_FALSE;
		}// handle error 
		ZeroMemory(&dcb,sizeof(DCB));
		ZeroMemory(&TimeOuts,sizeof(COMMTIMEOUTS));

		//设置接收缓冲区和发送缓冲区的大小
		if (!SetupComm(hCom, 256, 256)) 
		{	goto ret0;		}			
		
		//GetCommState(hCom,&dcb);  //获取缺省的DCB结构的值
	    
		// Fill in the DCB: baud=9600, 8 data bits, even parity, 1 stop bit. 
		dcb.BaudRate	= dwbaud;         //设定波特率为9600 bps
		dcb.ByteSize	= 8;              //设定数据位为8
		dcb.Parity		= NOPARITY;       //设定无奇偶校验
		dcb.StopBits	= ONESTOPBIT;     //设定一个停止位
		dcb.DCBlength = sizeof(DCB);
		
		////设置串行设备控制参数
		if (!SetCommState(hCom, &dcb)) 
		{	goto ret0;		}			
		
		TimeOuts.ReadIntervalTimeout = 10;
		TimeOuts.ReadTotalTimeoutConstant = 30;
		TimeOuts.ReadTotalTimeoutMultiplier = 2;
		TimeOuts.WriteTotalTimeoutConstant = 0;
		TimeOuts.WriteTotalTimeoutMultiplier = 0;

		if (!SetCommTimeouts(hCom, &TimeOuts)) 
		{	goto ret0;		}			

		/*
		//监视串口的错误和接收到字符两种事件 
　		SetCommMask(hCom,EV_ERR|EV_RXCHAR);
　		//创建人工重设、未发信号的事件 
　		hEvent=CreateEvent(NULL,FALSE,FALSE,"WatchEvent"); 
		//创建一个事件监视线程来监视串口事件 
　		AfxBeginThread(CommWatchProc,pParam); 
		*/
		return	S_OK;

	ret0:
		CloseHandle(hCom);
		hCom=INVALID_HANDLE_VALUE;
		return S_FALSE;
	}

	HRESULT CComReadWrite::CloseDevice()
	{
		if(hCom==INVALID_HANDLE_VALUE)
		{
			return S_OK;
		}
		CloseHandle(hCom);
		hCom=INVALID_HANDLE_VALUE;
		return S_OK;
	}

	HRESULT CComReadWrite::WriteCom(char* str,int count)
	{
		DWORD tmp;
		//Mutex.acquire();
		//停止发送和接收数据，并清除发送和接收缓冲区 
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
		if (!WriteFile(hCom, str, count, &tmp, NULL))
		{
			//Mutex.release();
			return S_FALSE;
		}
	//	::FhtLog("写入COM口,数据长度为%d,数据为:%s", count,(LPCTSTR)::Bin2HexString((BYTE*)str, count, 1));
		//Mutex.release();
		//printf("写入数据成功！\n");
		return S_OK;
	}

	HRESULT CComReadWrite::ReadCom(char* str,int count,int nRetryCount)
	{
		DWORD dwTotalRead = 0;
		DWORD dwReadCount = 0;
		//Mutex.acquire();
		while(dwTotalRead < (DWORD)count && dwReadCount < (DWORD)nRetryCount)
		{
			DWORD tmp =0;
			ReadFile(hCom, str+dwTotalRead, count-dwTotalRead, &tmp, NULL);	
		    //printf("从COM读出数据长度为%d,数据为:%s", tmp,str);
			dwTotalRead += tmp;
			dwReadCount++;
		}
		//printf("读数据成功！\n");
		//return S_OK;
		if((DWORD)count!=dwTotalRead)
		{
			//Mutex.release();
			return S_FALSE;
		}
		else 
		{
			//Mutex.release();
			return S_OK;
		}
	}
#else
	HRESULT CComReadWrite::InitDevice(int comport, unsigned long dwbaud, unsigned long dReadIntervalTimeout, unsigned long dReadTotalTimeoutConstant)
	{
		if (initbool)
			return S_OK;
		bool reval =S_FALSE;
		char dev[12]= {0};
		sprintf(dev,"/dev/ttyS%d",comport-1);
		hCom=open(dev,O_RDWR  |  O_NOCTTY);
		if(hCom==-1)
		{
			perror("can't open dev");
		}
		if(0 == set_opt(hCom,dwbaud,8,'N',1,2))
			{
			reval = S_OK;
			initbool = true;
		}
		else
			{
				close(hCom);
			}
		return reval;
	}
	HRESULT CComReadWrite::CloseDevice()
	{
		initbool = false;
		close(hCom);
		return S_OK;
	}
	HRESULT CComReadWrite::WriteCom(char* str,int count,int nRetryCount)
	{
		HRESULT reval = S_FALSE;
		_Syn->Lock();
		int sendByte = write(hCom,str ,count);

		_Syn->UnLock();
		if ( sendByte == count)
	 		reval = S_OK;
		return reval;
	}

	HRESULT CComReadWrite::ReadCom(char* str,int count,int nRetryCount)
	{	
		
		int dwTotalRead = 0;
		int dwReadCount = 0;

 		_Syn->Lock();
		while(dwTotalRead < count && dwReadCount < nRetryCount)
		{
			int readByte =0;
			readByte = read(hCom, str+dwTotalRead, count-dwTotalRead);	
	        
			dwTotalRead += readByte;
			dwReadCount++;
			//printf("readByte =%d  dwTotalRead = %d\n",readByte,dwTotalRead);
	       
		}
		_Syn->UnLock();
		if(count!=dwTotalRead)
		{
			return S_FALSE;
		}
		else 
		{
			return S_OK;
		}
		/*int reval;
		int  readByte = 1;
		while(readByte > 0)
		{
		readByte = read(hCom,str,8);
		}
		return reval;*/
	}

	int CComReadWrite::set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop,int nTimeout)
	{
		struct termios newtio,oldtio;
		/*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
		if ( tcgetattr( fd,&oldtio) != 0)
		{
			perror("SetupSerial 1");
			return -1;
		}
		bzero( &newtio, sizeof( newtio ) );
		/*步骤一，设置字符大小*/
		newtio.c_cflag |= CLOCAL | CREAD;
		newtio.c_cflag &= ~CSIZE;
		/*设置停止位*/
		switch( nBits )
		{
			case 7:
				newtio.c_cflag |= CS7;
				break;
			case 8:
				newtio.c_cflag |= CS8;
				break;
		}
	/*设置奇偶校验位*/
		switch( nEvent )
		{
			case 'O': //奇数
				newtio.c_cflag |= PARENB;
				newtio.c_cflag |= PARODD;
				newtio.c_iflag |= (INPCK | ISTRIP);
			break;
			case 'E': //偶数
				newtio.c_iflag |= (INPCK | ISTRIP);
				newtio.c_cflag |= PARENB;
				newtio.c_cflag &= ~PARODD;
			break;
				case 'N': //无奇偶校验位
				newtio.c_cflag &= ~PARENB;
			break;
		}
		/*设置波特率*/
		switch( nSpeed )
		{
			case 2400:
				cfsetispeed(&newtio, B2400);
				cfsetospeed(&newtio, B2400);
			break;
			case 4800:
				cfsetispeed(&newtio, B4800);
				cfsetospeed(&newtio, B4800);
			break;
			case 9600:
				cfsetispeed(&newtio, B9600);
				cfsetospeed(&newtio, B9600);
			break;
			case 115200:
				cfsetispeed(&newtio, B115200);
				cfsetospeed(&newtio, B115200);
			break;
			case 460800:
				cfsetispeed(&newtio, B460800);
				cfsetospeed(&newtio, B460800);
			break;
			default:
				cfsetispeed(&newtio, B9600);
				cfsetospeed(&newtio, B9600);
			break;
		}
		/*设置停止位*/
		if( nStop == 1 )
			newtio.c_cflag &= ~CSTOPB;
		else if ( nStop == 2 )
			newtio.c_cflag |= CSTOPB;
		/*设置等待时间和最小接收字符*/
		/*nTimeout = nTimeout / 1000;
		if (nTimeout <= 0)
    		nTimeout = 1;*/
		newtio.c_cc[VTIME] = 10 ;
		newtio.c_cc[VMIN] = 0;
		/*处理未接收字符*/
		tcflush(fd,TCIFLUSH);
		/*激活新配置*/
		if((tcsetattr(fd,TCSANOW,&newtio))!=0)
		{
			perror("com set error");
			return -1;
		}
		return 0;
	} 
#endif
