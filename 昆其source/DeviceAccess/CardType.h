#pragma once

#ifndef  _CARDTYPE_H_
#define _CARDTYPE_H_

#include <time.h>
#include <iostream>
//协议头,两个字节
#define PROTOHEAD0 0xFF
#define PROTOHEAD1 0xFE
//命令码，一个字节
#define SETDSTNET 0x10
#define GETDSTNET 0x11

#define SETSRCNET 0x12 
#define GETSRCNET 0x13

#define SETDSTPID 0x20
#define GETDSTPID 0x21
 
#define SETSRCPID 0x22
#define GETSRCPID 0x23
 
#define SETCHNCON 0x30
#define GETCHNCON 0x31
 
#define SETGLOBALCON 0x40
#define GETGLOBALCON 0x41

//时间设置模式
#define OSD_CYEAR       0x00100000
#define OSD_YEAR4       0x00020000
#define OSD_YEAR2       0x00010000

#define OSD_EMONTH      0x00002000
#define OSD_MONTH       0x00001000

#define OSD_EWEEK       0x00000200
#define OSD_CWEEK       0x00000100

#define OSD_CHOUR       0x00000010
#define OSD_HOUR24      0x00000002
#define OSD_HOUR12      0x00000001

//报警定义
#define V_BLACK     0x1          //视频黑场
#define C_SLICE     0x2          //彩条
#define V_STATIC	0x4          //画面静止
#define V_MISS      0x8          //无视频信号
#define S_UNLOCK	0xE          //无载波
#define A_SILENCE	0x10         //音频静音

#define NO_SIGNAL   0x81         //无信号
#define NO_VIDEO    0x82         //无视频
#define NO_AUDIO    0x84         //无音频
#define UNCA_VIDEO  0x88         //视频CA失败
#define UNCA_AUDIO  0x90         //音频CA失败


#define V_BLACK_V_STATIC     0x5          //视频黑场+画面静止
#define C_SLICE_V_STATIC     0x6          //彩条+画面静止
#define CARDREBOOT			 0xFFFF		  //板卡重启


//消息类型定义
enum MSG_TYPE
{
	MSG_UNKNOWN					= 0x0,				//未知消息
	
	MSG_SET_REBOOT				= 0x100,			//重启
	MSG_RET_REBOOT				= 0x200,

	MSG_SET_INIT				= 0x101,			//初始化
	MSG_RET_INIT				= 0x201,
	
	MSG_SET_START				= 0x102,			//开始转码
	MSG_RET_START				= 0x202,
	
	MSG_SET_STOP				= 0x103,			//停止转码
	MSG_RET_STOP				= 0x203,
	
	MSG_SET_VIDEOBITRATE		= 0x104,			//视频码率
	MSG_RET_VIDEOBITRATE		= 0x204,
	
	MSG_SET_AUDIOBITRATE		= 0x105,			//音频码率
	MSG_RET_AUDIOBITRATE		= 0x205,
	
	MSG_SET_OSD0	        	= 0x106,			//0通道OSD
	MSG_RET_OSD0				= 0x206,
	
	MSG_SET_OSD1	        	= 0x107,			//1通道OSD
	MSG_RET_OSD1				= 0x207,
	
	MSG_SET_OSD2	       		= 0x108,			//2通道OSD
	MSG_RET_OSD2				= 0x208,
	
	MSG_SET_OSD3	        	= 0x109,			//3通道OSD
	MSG_RET_OSD3				= 0x209,
	
	MSG_SET_TIME				= 0x110,			//系统时间
	MSG_RET_TIME				= 0x210,
	
	MSG_SET_DSTINFO        		= 0x111,			//上位机信息
	MSG_RET_DSTINFO				= 0x211,

	MSG_SET_VIDEOTUNER			= 0x112,			//视频（电视）调频
	MSG_RET_VIDEOTUNER			= 0x212,
	
	MSG_SET_FMTUNER				= 0x113,			//音频（FM）调频
	MSG_RET_FMTUNER				= 0x213,

	MSG_SET_AMTUNER				= 0x120,			//音频（AM）调频
	MSG_RET_AMTUNER				= 0x220,
	
	MSG_SET_ALERTVIDEO			= 0x114,			//视频报警参数
	MSG_RET_ALERTVIDEO			= 0x214,
	
	MSG_SET_ALERTAUDIO     		= 0x115,			//音频报警参数
	MSG_RET_ALERTAUDIO			= 0x215,
	
	MSG_GET_RUNSTATUS			= 0x116,			//运行状态
	MSG_RET_RUNSTATUS			= 0x216,
	
	MSG_GET_STDCHANSCAN			= 0x117,			//标准频道扫描
	MSG_RET_STDCHANSCAN			= 0x217,
	
	MSG_GET_CHANSCAN			= 0x118,			//全频段扫描
	MSG_RET_CHANSCAN			= 0x218,

	MSG_GET_CHANFIX				= 0x119,			//获取是否锁定
	MSG_RET_CHANFIX				= 0x219,
	
	MSG_RET_ALERT				= 0x301,			//异态上报

	MSG_I2C_OPEN				= 0x401,			//打开I2C设备
	MSG_RET_I2C_OPEN			= 0x501,
	
	MSG_I2C_CLOSE				= 0x402,			//关闭I2C设备
	MSG_RET_I2C_CLOSE			= 0x502,
	
	MSG_I2C_SWITCH				= 0x403,			//选择...
	MSG_RET_I2C_SWITCH			= 0x503,
	
	MSG_I2C_WRITE				= 0x404,			//写I2C设备
	MSG_RET_I2C_WRITE			= 0x504,
	
	MSG_I2C_READ				= 0x405,			//读I2C设备
	MSG_RET_I2C_READ			= 0x505,

	//DVB
	MSG_SET_DIGITALINIT			= 0x150,			//数字卡初始化
	MSG_RET_DIGITALINIT			= 0x250,

	MSG_SET_TUNERPID			= 0x151,			//调频的PID
	MSG_RET_TUNERPID			= 0x251,

	MSG_SET_CODECPID			= 0x152,			//编码后TS流中的PID
	MSG_RET_CODECPID			= 0x252,

	MSG_SET_DIGITALTUNER		= 0x153,			//数字卡调频
	MSG_RET_DIGITALTUNER		= 0x253,

	MSG_SET_HDSD				= 0x154,			//高清节目标识
	MSG_RET_HDSD				= 0x254,

	//指标相关
	MSG_GET_QUA					=0x415,
	MSG_RET_QUA					=0x515,

	MSG_GET_RADIO				=0x416,
	MSG_RET_RADIO				=0x516,

	MSG_GET_SPECIAL_AUDIO_CHANSCAN		=0x417,
	MSG_RET_SPECIAL_AUDIO_CHANSCAN		=0x517,
};

//命令结果标识定义
enum MSG_RETURN
{      
	RET_OK		=	0x1,       //表示设置信息设置成功
	RET_FAIL 	=	0x0,       //表示设置信息设置失败
};

//前端设备信息结构定义
typedef struct _tagDstInfo
{
	char dstIP[30];
	int  dstport;
	int  alarmport;
}DstInfo_Obj, *DstInfo_Handle;

//板卡初始化信息结构定义
typedef struct _tagInitBoard
{
	DstInfo_Obj	dstinfo;		//目标地址信息
	int video_bitrate;			//视频转码默认码率
	int audio_idx;				//音频转码默认码率序号；
	int videosize;				//初始画面大小  videosize=1 为CIF，videosize=2为QCIF
	time_t systime;				//系统时间

	_tagInitBoard()
	{
		video_bitrate	= 700;
		audio_idx		= 5;
		videosize		= 1;
		systime			= time(0);
	};
}InitBoard_Obj, *InitBoard_Handle;

//调频信息结构定义
typedef struct _tagTunerConfig
{
	int chan;  //通道号
	int freq;   //频点 
}TunerConfig_Obj, *TunerConfig_Handle;

//码率信息结构定义
typedef struct _tagMediaConfig
{
	int chan;					//通道
	int video_bitrate;			//视频码率（对广播转码无效）
	int audio_idx;				//音频码率索引

	_tagMediaConfig()
	{
		chan = 0;
		video_bitrate = 400;
		audio_idx = 5;
	};
}MediaConfig_Obj, *MediaConfig_Handle;

//OSD信息结构定义
typedef struct _tagSubtitleConfig
{
	int size;				// 字体大小，默认使用16号，推荐使用16号字

	int subtitle0_Enable;  //是否显示
	char subtitle0[40];    //显示的字幕内容
	int subtitle0_dislen;   //每行显示的长度，字节为单位，每个汉字2个字节
	int subtitle0_x;       //显示位置
	int subtitle0_y;

	int subtitle1_Enable;
	char subtitle1[40];
	int subtitle1_dislen;
	int subtitle1_x;
	int subtitle1_y;

	int time_Enable;     //是否显示时间
	int mode;           //时间显示模式
	int time_x;          //时间显示位置
	int time_y;

	_tagSubtitleConfig()
	{
		size				= 16;
		subtitle0_Enable	= 1;		
		subtitle0_dislen	= 20;
		subtitle0_x			= 10;
		subtitle0_y			= 10;

		subtitle1_Enable	= 1;		
		subtitle1_dislen	= 20;
		subtitle1_x			= 250;
		subtitle1_y			= 260;

		time_Enable			= 1;
		mode				= OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH | OSD_HOUR24;
		time_x				= 160;
		time_y				= 10;
		memset(subtitle0,0,40);
		memset(subtitle1,0,40);
	};
}SubtitleConfig_Obj, *SubtitleConfig_Handle;

//频道扫描结构定义
typedef struct _tagChannelScanConfig
{
	int chan;  			//通道号
	int Startfreq;   	//开始频点
	int Endfreq;		//结束频点
	int step;			//频点累加步长 （标准频点扫描时无意义）
}ChannelScan_Obj, *ChannelScan_Handle;

//报警配置信息结构定义
typedef struct _tagAlertConfig
{
	int video_black;		//黑场
	int color_slice;		//彩条
	int video_static;		//静帧
	int video_miss;			//视频丢失
	int audio_silence;		//静音
}AlertConfig_Obj, *AlertConfig_Handle;

//命令头结构定义
typedef struct _tagPkgHeader
{
	char header;
	int  msg_type;
}PkgHeader_Obj, *PkgHeader_Handle;

//命令结果结构定义
typedef struct _tagRetMessage
{
	PkgHeader_Obj ph;
	int	status;
}
RetMessage_Obj, *RetMessage_Handle;

// 以下是电视广播指标与控制卡通讯的结构定义

//板卡返回特定频点的电平值结构
typedef struct _tagRadioInfo
{
	char flag;      // 区别指标或者频谱 01 频谱 02 指标 
	int freq;		//频点
	int gain;       //增益
}Radiopayload_Obj, * Radiopayload_Handle;

typedef struct _tagSpecialRadioInfo  //针对陕西项目实现的临时解决方案
{
	int startfreq;  //起始频点
	int endfreq;    //结束频点
	int step;       //步长
	int chan;    //通道号
}SpecialRadiopayload_Obj,* SpecialRadiopayload_Handle;

typedef struct _tagSpecialRadioRetMessage  
{
	PkgHeader_Obj ph;
	int	status;
	int len;              //value的长度，
	int value[240];       //如果是频道扫描返回的数据为锁定的频点，如果是频谱扫描返回的为对应频点的电平值
}SpecialRadioRetMessage_Obj,* SpecialRadioRetMessage_Handle;

typedef struct _tagTVQuaRetMessage
{
	PkgHeader_Obj ph;
	int	status;
	int freq;
	int level;
}TVQuaRetMessage_Obj, * TVQuaRetMessage_Handle;

typedef struct _tagRadioQuaRetMessage
{
	PkgHeader_Obj ph;
	int status;			//返回值状态 0失败 1正确
	int freq;			//频点
	int level_int;		//信号电平
	int dev_int;		//调制度
	int dF;				//频偏
	time_t getTime; 
}RadioQuaRetMessage_Obj, *RadioQuaRetMessage_Handle;

typedef struct _tagRadioSpecRetMessage
{
	PkgHeader_Obj ph;
	int status;
	int freq;
	int level_int;
	int gain;
	char spec[1024];	//16个为一个频点的，共64个频点	
}RadioSpecRetMessage_Obj, *RadioSpecRetMessage_Handle;

typedef struct _tagChannelScanRetMessage
{
	PkgHeader_Obj ph;
	int status;
	char freq[110];
}ChannelScanRetMessage_Obj,*ChannelScanRetMessage_Handle;
typedef struct _tagChannelScanAudio
{
	int startfreq;
	int endfreq;
}ChanelScanAudio_Obj,*ChannelScanAudio_Handle;

typedef struct _tagChannelScanAudioRetMessage
{
	PkgHeader_Obj ph;
	int status;
	int freqnum;
	int freqs[100];
}ChannelScanAudioRetMessage_Obj,*ChannelsScanAudioRetMessage_Handle;

typedef struct 
{
	int Freq;
	float ImageLevel;  //图像电平,单位:dbuv
	float AudioLevel;  //伴音电平,单位:dbuv
	float I2AOffLevel; //图像载波与伴音载波的电平差,单位:dbuv
	float CN;          //载噪比,单位:dbuv
	float FreqOffSet;  //载频频偏,单位:kHz
	float Slope;       //斜率
	time_t getTime; 
}TVQuality;

typedef struct  
{
	std::string Type;
	std::string Desc;
	std::string Value;
}QualityDesc;


typedef struct TypeDesc
{
	std::string Type;
	std::string Desc;
}sTypeDesc;

//四画面使用
typedef struct _tagChanNetCfg
{
	unsigned int u32ChanID; //(in)标识通道号
    char u8ChanIp[16]; //(in/out)ts流组播IP
    unsigned int u32ChanPort; //(in/out)ts流组播端口
} ChanNetCfgObj, *ChanNetCfgHandle;

//PID信息结构体

typedef struct _tagChanPIDCfg
{
	unsigned int u32PreChanID;//(in)标示在原来板卡上的通道号（仅在第一阶段）
						//为了和第二阶段兼容建议用 0x10标识通道0、0x11标识通道1、0x12标识通道2、0x13标识通道3.
    unsigned int u32ChanID;//(in)标识通道号
    unsigned int u32PmtPid;//(in/out)标识PMT PID
    unsigned int u32PcrPid;//(in/out)标识 PCR PID
    unsigned int u32VideoPid; //(in/out)标识VIDEO PID
    unsigned int u32AudioPid; //(in/out)标识AUDIO PID
} ChanPIDObj, *ChanPIDHandle;
#endif