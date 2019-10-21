#pragma once
#include "ComReadWrite.h"
#include "ace/singleton.h"
///<summary>
///     场强基本功能类
///<summary>
            
class CBaseAnalyser
{
    //Constructors
public:
    CBaseAnalyser(void);
    virtual ~CBaseAnalyser(void);

    //Operations
private:
	HRESULT SendComman(char* bDsCommand,char bCmdLen);      //发送数据
	HRESULT ReadData(char* szTextFloat,int nReceive);       //接收场强数据

    //Attributes
public:
    CComReadWrite mComRW;                                   //串口操作对象

    //Operations
public:
	HRESULT InitDevice(int nComPort=3);                     //初始化指定串口
	HRESULT CloseDevice(void);                              //关闭指定串口
	HRESULT GetFreqLevel(float fFreq, float &fLevel);       //得到某一频点电平
	HRESULT GetCN(float fFreq, float &fCN);                 //得到载噪比
	HRESULT GetFreqOffset(float fFreq, float &fOffset);     //得到频移量
	HRESULT CommandCommucation();                           //场强通讯握手

    //Overrides
public:

    //Implementation
public:

};


typedef ACE_Singleton<CBaseAnalyser,ACE_Mutex>  DEVICEFREQLEVEL;